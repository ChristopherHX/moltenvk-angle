//
// Copyright (c) 2008-2020 the Urho3D project.
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

#ifdef _WIN32
#ifndef _MSC_VER
#define _WIN32_IE 0x501
#endif
#include <windows.h>
#include <shellapi.h>
#include <direct.h>
#include <shlobj.h>
#include <sys/types.h>
#include <sys/utime.h>
#else
#include <dirent.h>
#include <cerrno>
#include <unistd.h>
#include <utime.h>
#include <sys/wait.h>
#endif

#include "MonoEmbed.h"


#ifndef _TESTCASE_
#include <mono/jit/jit.h>
#endif

#include <mono/metadata/environment.h>
#include <mono/utils/mono-publib.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/assembly.h>
#include <stdlib.h>

#include "../Container/Str.h"
#include "../IO/FileSystem.h"
#include "../IO/File.h"
#include "../Core/Context.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"

#ifdef __ANDROID__
#include <android/log.h>
#endif


using namespace Urho3D;


#if defined(__ANDROID__)
static void internal_log(String  message)
{
    __android_log_print(2, "Urho3D", "%s", message.CString());
}
#else
static void internal_log(String message)
{

}
#endif


static void fixPath(String& path)
{
    path.Replace("\\", "/");
    path.Replace("//", "/");
}

static String fixPathString(String  path)
{
    path.Replace("\\", "/");
    path.Replace("//", "/");
    return path;
}

static bool CopyFileToDocumentsDir(Urho3D::SharedPtr<Urho3D::Context> context_,  String sourceFile)
{
    FileSystem* fileSystem = context_->GetSubsystem<FileSystem>();
    ResourceCache* cache = context_->GetSubsystem<ResourceCache>();

    SharedPtr<File>  srcFile = cache->GetFile(sourceFile);
    if (!srcFile->IsOpen())
        return false;

    String userDocsDir  = fileSystem->GetUserDocumentsDir();
    String destFileName = fixPathString(userDocsDir + "/" + sourceFile);

    fileSystem->Delete(destFileName);

    SharedPtr<File> destFile(new File(context_, destFileName, FILE_WRITE));
    if (!destFile->IsOpen())
        return false;

    unsigned fileSize = srcFile->GetSize();
    SharedArrayPtr<unsigned char> buffer(new unsigned char[fileSize]);

    unsigned bytesRead = srcFile->Read(buffer.Get(), fileSize);
    unsigned bytesWritten = destFile->Write(buffer.Get(), fileSize);
    return bytesRead == fileSize && bytesWritten == fileSize;
}


static void CopyMonoFilesToDocumentDir(Urho3D::SharedPtr<Urho3D::Context> context)
{
    FileSystem* fileSystem = context->GetSubsystem<FileSystem>();
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();

    CopyFileToDocumentsDir(context, String("04_StaticScene.exe"));
    CopyFileToDocumentsDir(context, String("DotNetBindings.dll"));
    CopyFileToDocumentsDir(context, String("mscorlib.dll"));
    
}

static void main_function(MonoDomain* domain, const char* file, int argc, char** argv)
{

    MonoAssembly* assembly;
    assembly = mono_domain_assembly_open(domain, file);
    if (!assembly)
        exit(2);

    mono_jit_exec(domain, assembly, argc, argv);

}


int start_mono_main(void) {


    // copy mono assemblies to the user documents folder , mono will be configured to this folder
    Urho3D::SharedPtr<Urho3D::Context> context(new Urho3D::Context());
    context->RegisterSubsystem(new FileSystem(context));
    context->RegisterSubsystem(new ResourceCache(context));
    FileSystem* fileSystem = context->GetSubsystem<FileSystem>();
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();
    String programDir = fileSystem->GetProgramDir();
    cache->AddResourceDir(fixPathString(programDir+"/Data"));
    CopyMonoFilesToDocumentDir(context);
    String userDocumentsDir = fileSystem->GetUserDocumentsDir(); 
    context.Reset();


    // start Mono

    MonoDomain* domain;
   
    String asssemblyName =  "04_StaticScene.exe";
    String assemblyFullPath = userDocumentsDir + "/" + asssemblyName;

    int argc = 2;
    char* argv[] = {
                        (char*)asssemblyName.CString(),
                        (char*)assemblyFullPath.CString(),
                        NULL
    };
    const char* file;
    int retval;

    file = argv[1];

    mono_set_dirs(userDocumentsDir.CString(), "");
    mono_set_assemblies_path(userDocumentsDir.CString());

    mono_config_parse(NULL);

    domain = mono_jit_init(file);
    if (domain)
    {
        main_function(domain, file, argc - 1, argv + 1);
        retval = mono_environment_exitcode_get();
        mono_jit_cleanup(domain);
    }

    return retval;
}


#if defined(_MSC_VER) && defined(_DEBUG) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    return start_mono_main();
}
#elif defined(_MSC_VER) && defined(URHO3D_MINIDUMPS) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    return start_mono_main();
}
#elif defined(_WIN32) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    return start_mono_main();
}
#elif defined(__ANDROID__)
extern "C" __attribute__((visibility("default"))) int SDL_main(int argc, char** argv);
extern "C" {
    int SDL_main(int argc, char** argv)
    {
        return start_mono_main();
    }
}
#elif  defined(IOS) || defined(TVOS)
extern "C" __attribute__((visibility("default"))) int SDL_main(int argc, char** argv);
extern "C" {
    int SDL_main(int argc, char** argv)
    {
        return start_mono_main();
    }
}
#else
int main(int argc, char** argv)
{
    return start_mono_main();
}
#endif

