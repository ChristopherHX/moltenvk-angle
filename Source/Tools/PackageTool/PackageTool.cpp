//
// Copyright (c) 2008-2019 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <Urho3D/Core/Context.h>
#include <Urho3D/Container/ArrayPtr.h>
#include <Urho3D/Core/ProcessUtils.h>
#include <Urho3D/IO/File.h>
#include <Urho3D/IO/FileSystem.h>
#include <Urho3D/IO/PackageFile.h>

#include <sys/stat.h>
#include <cstdio>

#ifdef WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <cerrno>
#include <unistd.h>
#include <utime.h>
#include <sys/wait.h>
#endif

#include <LZ4/lz4.h>
#include <LZ4/lz4hc.h>

#include <Urho3D/DebugNew.h>

using namespace Urho3D;

static const unsigned COMPRESSED_BLOCK_SIZE = 32768;

struct FileEntry
{
    String name_;
    unsigned offset_{};
    unsigned size_{};
    unsigned checksum_{};
};

SharedPtr<Context> context_(new Context());
SharedPtr<FileSystem> fileSystem_(new FileSystem(context_));
String basePath_;
Vector<FileEntry> entries_;
unsigned checksum_ = 0;
bool compress_ = false;
bool quiet_ = false;
unsigned blockSize_ = COMPRESSED_BLOCK_SIZE;

bool unpack_ = false;

String ignoreExtensions_[] = {
    ".bak",
    ".rule",
    ""
};

int main(int argc, char** argv);
void Run(const Vector<String>& arguments);
void ProcessFile(const String& fileName, const String& rootDir);
void WritePackageFile(const String& fileName, const String& rootDir);
void WriteHeader(File& dest);
void Unpack(const Vector<String>& arguments);

int main(int argc, char** argv)
{
    Vector<String> arguments;

    #ifdef WIN32
    arguments = ParseArguments(GetCommandLineW());
    #else
    arguments = ParseArguments(argc, argv);
    #endif

    Run(arguments);
    return 0;
}


void Run(const Vector<String>& arguments)
{
    if (arguments.Size() < 2)
        ErrorExit(
            "Usage: packing  PackageTool <directory to process> <package name> [basepath] [options]\n"
			"Usage: unpacking  PackageTool <package name> <directory to output> <-u> \n"
            "\n"
            "Options:\n"
            "-c      Enable package file LZ4 compression\n"
            "-q      Enable quiet mode\n"
            "\n"
            "Basepath is an optional prefix that will be added to the file entries.\n\n"
            "Alternative output usage: PackageTool <output option> <package name>\n"
            "Output option:\n"
            "-i      Output package file information\n"
            "-l      Output file names (including their paths) contained in the package\n"
            "-L      Similar to -l but also output compression ratio (compressed package file only)\n"
			"-u      unpack package file\n"
        );

    const String& dirName = arguments[0];
    const String& packageName = arguments[1];
    bool isOutputMode = arguments[0].Length() == 2 && arguments[0][0] == '-';
    if (arguments.Size() > 2)
    {
        for (unsigned i = 2; i < arguments.Size(); ++i)
        {
            if (arguments[i][0] != '-')
                basePath_ = AddTrailingSlash(arguments[i]);
            else
            {
                if (arguments[i].Length() > 1)
                {
                    switch (arguments[i][1])
                    {
                    case 'c':
                        compress_ = true;
                        break;
                    case 'q':
                        quiet_ = true;
                        break;
					case 'u':
						unpack_ = true;
						break;
                    default:
                        ErrorExit("Unrecognized option");
                    }
                }
            }
        }
    }

	if (unpack_ == true)
	{
		Unpack(arguments);
	}
	else
    if (!isOutputMode)
    {
        if (!quiet_)
            PrintLine("Scanning directory " + dirName + " for files");

        // Get the file list recursively
        Vector<String> fileNames;
        fileSystem_->ScanDir(fileNames, dirName, "*.*", SCAN_FILES, true);
        if (!fileNames.Size())
            ErrorExit("No files found");

        // Check for extensions to ignore
        for (unsigned i = fileNames.Size() - 1; i < fileNames.Size(); --i)
        {
            String extension = GetExtension(fileNames[i]);
            for (unsigned j = 0; j < ignoreExtensions_[j].Length(); ++j)
            {
                if (extension == ignoreExtensions_[j])
                {
                    fileNames.Erase(fileNames.Begin() + i);
                    break;
                }
            }
        }

        for (unsigned i = 0; i < fileNames.Size(); ++i)
            ProcessFile(fileNames[i], dirName);

        WritePackageFile(packageName, dirName);
    }
    else
    {
        SharedPtr<PackageFile> packageFile(new PackageFile(context_, packageName));
        bool outputCompressionRatio = false;
        switch (arguments[0][1])
        {
        case 'i':
            PrintLine("Number of files: " + String(packageFile->GetNumFiles()));
            PrintLine("File data size: " + String(packageFile->GetTotalDataSize()));
            PrintLine("Package size: " + String(packageFile->GetTotalSize()));
            PrintLine("Checksum: " + String(packageFile->GetChecksum()));
            PrintLine("Compressed: " + String(packageFile->IsCompressed() ? "yes" : "no"));
            break;
        case 'L':
            if (!packageFile->IsCompressed())
                ErrorExit("Invalid output option: -L is applicable for compressed package file only");
            outputCompressionRatio = true;
            // Fallthrough
        case 'l':
            {
                const HashMap<String, PackageEntry>& entries = packageFile->GetEntries();
                for (HashMap<String, PackageEntry>::ConstIterator i = entries.Begin(); i != entries.End();)
                {
                    HashMap<String, PackageEntry>::ConstIterator current = i++;
                    String fileEntry(current->first_);
                    if (outputCompressionRatio)
                    {
                        unsigned compressedSize =
                            (i == entries.End() ? packageFile->GetTotalSize() - sizeof(unsigned) : i->second_.offset_) -
                            current->second_.offset_;
                        fileEntry.AppendWithFormat("\tin: %u\tout: %u\tratio: %f", current->second_.size_, compressedSize,
                            compressedSize ? 1.f * current->second_.size_ / compressedSize : 0.f);
                    }
                    PrintLine(fileEntry);
                }
            }
            break;
        default:
            ErrorExit("Unrecognized output option");
        }
    }
}

void ProcessFile(const String& fileName, const String& rootDir)
{
    String fullPath = rootDir + "/" + fileName;
    File file(context_);
    if (!file.Open(fullPath))
        ErrorExit("Could not open file " + fileName);
    if (!file.GetSize())
        return;

    FileEntry newEntry;
    newEntry.name_ = fileName;
    newEntry.offset_ = 0; // Offset not yet known
    newEntry.size_ = file.GetSize();
    newEntry.checksum_ = 0; // Will be calculated later
    entries_.Push(newEntry);
}

void WritePackageFile(const String& fileName, const String& rootDir)
{
    if (!quiet_)
        PrintLine("Writing package");

    File dest(context_);
    if (!dest.Open(fileName, FILE_WRITE))
        ErrorExit("Could not open output file " + fileName);

    // Write ID, number of files & placeholder for checksum
    WriteHeader(dest);

    for (unsigned i = 0; i < entries_.Size(); ++i)
    {
        // Write entry (correct offset is still unknown, will be filled in later)
        dest.WriteString(basePath_ + entries_[i].name_);
        dest.WriteUInt(entries_[i].offset_);
        dest.WriteUInt(entries_[i].size_);
        dest.WriteUInt(entries_[i].checksum_);
    }

    unsigned totalDataSize = 0;
    unsigned lastOffset;

    // Write file data, calculate checksums & correct offsets
    for (unsigned i = 0; i < entries_.Size(); ++i)
    {
        lastOffset = entries_[i].offset_ = dest.GetSize();
        String fileFullPath = rootDir + "/" + entries_[i].name_;

        File srcFile(context_, fileFullPath);
        if (!srcFile.IsOpen())
            ErrorExit("Could not open file " + fileFullPath);

        unsigned dataSize = entries_[i].size_;
        totalDataSize += dataSize;
        SharedArrayPtr<unsigned char> buffer(new unsigned char[dataSize]);

        if (srcFile.Read(&buffer[0], dataSize) != dataSize)
            ErrorExit("Could not read file " + fileFullPath);
        srcFile.Close();

        for (unsigned j = 0; j < dataSize; ++j)
        {
            checksum_ = SDBMHash(checksum_, buffer[j]);
            entries_[i].checksum_ = SDBMHash(entries_[i].checksum_, buffer[j]);
        }

        if (!compress_)
        {
            if (!quiet_)
                PrintLine(entries_[i].name_ + " size " + String(dataSize));
            dest.Write(&buffer[0], entries_[i].size_);
        }
        else
        {
            SharedArrayPtr<unsigned char> compressBuffer(new unsigned char[LZ4_compressBound(blockSize_)]);

            unsigned pos = 0;

            while (pos < dataSize)
            {
                unsigned unpackedSize = blockSize_;
                if (pos + unpackedSize > dataSize)
                    unpackedSize = dataSize - pos;

                auto packedSize = (unsigned)LZ4_compress_HC((const char*)&buffer[pos], (char*)compressBuffer.Get(), unpackedSize, LZ4_compressBound(unpackedSize), 0);
                if (!packedSize)
                    ErrorExit("LZ4 compression failed for file " + entries_[i].name_ + " at offset " + String(pos));

                dest.WriteUShort((unsigned short)unpackedSize);
                dest.WriteUShort((unsigned short)packedSize);
                dest.Write(compressBuffer.Get(), packedSize);

                pos += unpackedSize;
            }

            if (!quiet_)
            {
                unsigned totalPackedBytes = dest.GetSize() - lastOffset;
                String fileEntry(entries_[i].name_);
                fileEntry.AppendWithFormat("\tin: %u\tout: %u\tratio: %f", dataSize, totalPackedBytes,
                    totalPackedBytes ? 1.f * dataSize / totalPackedBytes : 0.f);
                PrintLine(fileEntry);
            }
        }
    }

    // Write package size to the end of file to allow finding it linked to an executable file
    unsigned currentSize = dest.GetSize();
    dest.WriteUInt(currentSize + sizeof(unsigned));

    // Write header again with correct offsets & checksums
    dest.Seek(0);
    WriteHeader(dest);

    for (unsigned i = 0; i < entries_.Size(); ++i)
    {
        dest.WriteString(basePath_ + entries_[i].name_);
        dest.WriteUInt(entries_[i].offset_);
        dest.WriteUInt(entries_[i].size_);
        dest.WriteUInt(entries_[i].checksum_);
    }

    if (!quiet_)
    {
        PrintLine("Number of files: " + String(entries_.Size()));
        PrintLine("File data size: " + String(totalDataSize));
        PrintLine("Package size: " + String(dest.GetSize()));
        PrintLine("Checksum: " + String(checksum_));
        PrintLine("Compressed: " + String(compress_ ? "yes" : "no"));
    }
}

void WriteHeader(File& dest)
{
    if (!compress_)
        dest.WriteFileID("UPAK");
    else
        dest.WriteFileID("ULZ4");
    dest.WriteUInt(entries_.Size());
    dest.WriteUInt(checksum_);
}


bool DirExists(const String& pathName) 
{


#ifndef _WIN32
	// Always return true for the root directory
	if (pathName == "/")
		return true;
#endif

	String fixedName = GetNativePath(RemoveTrailingSlash(pathName));

#ifdef __ANDROID__
	if (URHO3D_IS_ASSET(fixedName))
	{
		// Split the pathname into two components: the longest parent directory path and the last name component
		String assetPath(URHO3D_ASSET((fixedName + "/")));
		String parentPath;
		unsigned pos = assetPath.FindLast('/', assetPath.Length() - 2);
		if (pos != String::NPOS)
		{
			parentPath = assetPath.Substring(0, pos);
			assetPath = assetPath.Substring(pos + 1);
		}
		assetPath.Resize(assetPath.Length() - 1);

		bool exist = false;
		int count;
		char** list = SDL_Android_GetFileList(parentPath.CString(), &count);
		for (int i = 0; i < count; ++i)
		{
			exist = assetPath == list[i];
			if (exist)
				break;
		}
		SDL_Android_FreeFileList(&list, &count);
		return exist;
	}
#endif

#ifdef _WIN32
	DWORD attributes = GetFileAttributesW(WString(fixedName).CString());
	if (attributes == INVALID_FILE_ATTRIBUTES || !(attributes & FILE_ATTRIBUTE_DIRECTORY))
		return false;
#else
	struct stat st {};
	if (stat(fixedName.CString(), &st) || !(st.st_mode & S_IFDIR))
		return false;
#endif

	return true;
}

bool CreateDir(const String& pathName)
{
	// Create each of the parents if necessary
	String parentPath = GetParentPath(pathName);
	if (parentPath.Length() > 1 && !DirExists(parentPath))
	{
		if (!CreateDir(parentPath))
			return false;
	}

#ifdef _WIN32
	bool success = (CreateDirectoryW(GetWideNativePath(RemoveTrailingSlash(pathName)).CString(), nullptr) == TRUE) ||
		(GetLastError() == ERROR_ALREADY_EXISTS);
#else
	bool success = mkdir(GetNativePath(RemoveTrailingSlash(pathName)).CString(), S_IRWXU) == 0 || errno == EEXIST;
#endif


	return success;
}

void Unpack(const Vector<String>& arguments)
{
	const String& fileName = arguments[0];
	const String& outDirName = arguments[1];

	FileSystem * fileSystem = context_->GetSubsystem<FileSystem>();

	SharedPtr<PackageFile> packageFile(new PackageFile(context_, fileName, 0));

	
	const HashMap<String, PackageEntry>& entries = packageFile->GetEntries();
	Vector<String> entryNames = packageFile->GetEntryNames();

	for (int i = 0; i < entryNames.Size(); i++)
	{
		File src(context_, packageFile, entryNames[i]);
		int srcSize = src.GetSize();
		unsigned char * buf = new unsigned char[srcSize+1];
		src.Read(buf, srcSize);

		String dstFileFullPath =  outDirName + "/" + entryNames[i];
		
		String texPath, texName, texExt;
		SplitPath(dstFileFullPath, texPath, texName, texExt);

		unsigned pathPos = texPath.FindLast('/');
		if (texPath[texPath.Length() - 1] == '/' )
		{
			texPath[texPath.Length() - 1] = ' ';
		}
	
		bool directoryCreated = CreateDir(texPath);
		
		if (directoryCreated == true)
		{
			File dest(context_);
			if (!dest.Open(dstFileFullPath, FILE_WRITE))
				ErrorExit("-Could not open output file " + dstFileFullPath);

			dest.Write(buf, srcSize);
			dest.Close();
		}
		else
		{
			PrintLine("failed creating directory :  " + texPath);
		}
		
		src.Close();
	}
	

}
