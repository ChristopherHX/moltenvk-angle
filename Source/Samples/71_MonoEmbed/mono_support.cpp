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
#include <mono/metadata/mono-debug.h>
#include <mono/utils/mono-logger.h>
#include <stdlib.h>

#include "../Container/Str.h"
#include "../IO/FileSystem.h"
#include "../IO/File.h"
#include "../Core/Context.h"
#include "../IO/Log.h"
#include "../Resource/ResourceCache.h"

#include "mono_support.h"

#ifdef __ANDROID__
#include <android/log.h>
#include <jni.h>
#endif

using namespace Urho3D;



static String make_text(const char *log_domain, const char *log_level, const char *message) {
    String text(message);
    text += " (in domain ";
    text += log_domain;
    if (log_level) {
        text += ", ";
        text += log_level;
    }
    text += ")";
    return text;
}

void mono_log_callback(const char *log_domain, const char *log_level, const char *message, mono_bool fatal, void *)
{
    
    String text = make_text(log_domain, log_level, message);
    text += "\n";
    
    #if defined(__ANDROID__)
    __android_log_print(2, "Urho3D", "%s", text.CString());
    #else
    printf("%s",text.CString());
    #endif
}




void fixPath(String& path)
{
    path.Replace("\\", "/");
    path.Replace("//", "/");
}

String fixPathString(String  path)
{
    path.Replace("\\", "/");
    path.Replace("//", "/");
    return path;
}

bool CopyFileToDocumentsDir(Urho3D::SharedPtr<Urho3D::Context> context_,  String sourceFile, bool overwrite_if_exist)
{
    FileSystem* fileSystem = context_->GetSubsystem<FileSystem>();
    ResourceCache* cache = context_->GetSubsystem<ResourceCache>();

    String DestFolder = fileSystem->GetUserDocumentsDir();
#if !defined(ANDROID)
    DestFolder += "/temp/DotNet";
    fixPath(DestFolder);
    if (!fileSystem->CreateDir(DestFolder))
    {
        return false;
    }
#endif

    String sourceFileName = GetFileNameAndExtension(sourceFile);
    String destFileName = fixPathString(DestFolder + "/" + sourceFileName);

    if (overwrite_if_exist == false && fileSystem->FileExists(destFileName))
    {
        return true;
    }

    SharedPtr<File>  srcFile = cache->GetFile(sourceFile);
    if (srcFile == NULL || !srcFile->IsOpen())
        return false;



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


void CopyMonoFilesToDocumentDir(Urho3D::SharedPtr<Urho3D::Context> context,Platform platform)
{
    FileSystem* fileSystem = context->GetSubsystem<FileSystem>();
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();

    CopyFileToDocumentsDir(context, String("DotNet/Game.exe"),true);
    
    String prefix = "";
    switch (platform) {
            
        case _ANDROID_:
            prefix = "DotNet/android/";
            break;
            
        case _IOS_:
        case TVOS:
            prefix = "DotNet/ios/";
            break;
            
        case MACOS:
            prefix = "DotNet/macos/";
            break;
 
        case LINUX:
            prefix = "DotNet/linux/";
            break;
            
        case WINDOWS:
            prefix = "DotNet/windows/";
            break;
            
        default:
            prefix = "";
            break;
    }
    
    CopyFileToDocumentsDir(context, String(prefix+"UrhoDotNet.dll"),true);
    CopyFileToDocumentsDir(context, String(prefix+"mscorlib.dll"));
    CopyFileToDocumentsDir(context, String(prefix+"System.dll"));
    CopyFileToDocumentsDir(context, String(prefix+"System.Xml.dll"));
    CopyFileToDocumentsDir(context, String(prefix+"System.Core.dll"));
    CopyFileToDocumentsDir(context, String(prefix+"Mono.Security.dll"));
    CopyFileToDocumentsDir(context, String(prefix+"System.Numerics.dll"));

    if (platform == _ANDROID_)
    {
        CopyFileToDocumentsDir(context, String(prefix + "System.Runtime.dll"));
        CopyFileToDocumentsDir(context, String(prefix + "System.Threading.dll"));
        CopyFileToDocumentsDir(context, String(prefix + "System.Collections.dll"));
        CopyFileToDocumentsDir(context, String(prefix + "System.Collections.Concurrent.dll"));
        CopyFileToDocumentsDir(context, String(prefix + "System.Diagnostics.Debug.dll"));
        CopyFileToDocumentsDir(context, String(prefix + "System.Linq.dll"));
        CopyFileToDocumentsDir(context, String(prefix + "System.Runtime.InteropServices.dll"));
        CopyFileToDocumentsDir(context, String(prefix + "System.Threading.Tasks.dll"));
    }
}

