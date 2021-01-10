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


#if  defined(IOS) || defined(TVOS)
#include "ios_support.h"
#elif defined(__APPLE__)
#include "macos/macos_support.h"
#endif

using namespace Urho3D;


static void main_function(MonoDomain* domain, const char* file, int argc, char** argv)
{

    MonoAssembly* assembly;
    assembly = mono_domain_assembly_open(domain, file);
    if (!assembly)
        exit(2);

    mono_jit_exec(domain, assembly, argc, argv);

}


int start_urho_mono_main(Platform platform) {

    // copy mono assemblies to the user documents folder , mono will be configured to this folder
    Urho3D::SharedPtr<Urho3D::Context> context(new Urho3D::Context());
    context->RegisterSubsystem(new FileSystem(context));
    context->RegisterSubsystem(new ResourceCache(context));
    FileSystem* fileSystem = context->GetSubsystem<FileSystem>();
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();
    String programDir = fileSystem->GetProgramDir();
    cache->AddResourceDir(fixPathString(programDir+"/Data"));
    CopyMonoFilesToDocumentDir(context,platform);
    String userDocumentsDir = fileSystem->GetUserDocumentsDir();
#if !defined(ANDROID)
    userDocumentsDir += "/temp/DotNet";
    fixPath(userDocumentsDir);
#endif
    context.Reset();

  //  SetCurrentDirectory("D:\\Urho3D-Dev\\DotNet\\Urho3D\\build_vs2019\\bin");

  //  mono_dllmap_insert(NULL, "System.Runtime.CompilerServices.IAsyncStateMachine", NULL, "__Internal", NULL);
    // start Mono
    if (platform == _ANDROID_)
    {
#if defined(__ANDROID__)
    //    setenv("MONO_THREADS_SUSPEND", "preemptive", /* overwrite: */ true) == 0;
#endif
    //    mono_jit_set_aot_mode(MONO_AOT_MODE_NONE);
        mono_dllmap_insert(NULL, "java-interop", "java_interop_jvm_list", "Urho3D", NULL);

        mono_dllmap_insert(NULL, "System.Native", NULL, "mono-native", NULL);
        mono_dllmap_insert(NULL, "System.Net.Security.Native", NULL, "mono-native", NULL);
        
        mono_dllmap_insert(NULL, "MonoPosixHelper", NULL, "MonoPosixHelper", NULL);
        mono_dllmap_insert(NULL, "libmono-btls-shared", NULL, "mono-btls-shared", NULL);

    }

    if (platform == MACOS)
    {
        mono_dllmap_insert(NULL, "System.Native", NULL, "mono-native", NULL);
        mono_dllmap_insert(NULL, "System.Security.Cryptography.Native.Apple", NULL, "mono-native", NULL);
    }
    
  //  mono_debug_init (MONO_DEBUG_FORMAT_MONO);
  //  mono_trace_set_level_string("debug");
  //  mono_trace_set_log_handler(mono_log_callback, NULL);

   
   
    String asssemblyName =  "Game.dll";
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
    
    String mono_paths = userDocumentsDir.CString();
    mono_set_dirs(mono_paths.CString(), "");
    mono_set_assemblies_path(mono_paths.CString());

    mono_config_parse(NULL);
    
    MonoDomain* domain = NULL;
#if defined(IOS) || defined(TVOS) || defined(__ANDROID__)
    // I don't know whether this actually matters or not
    const char* runtime_version = "mobile";
#else
    const char* runtime_version = "v4.0.30319";
#endif

    domain = mono_jit_init_version("UrhoEngine.RootDomain", runtime_version);

   // domain = mono_jit_init(file);

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
    return start_urho_mono_main(WINDOWS);
}
#elif defined(_MSC_VER) && defined(URHO3D_MINIDUMPS) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    return start_urho_mono_main(WINDOWS);
}
#elif defined(_WIN32) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    return start_urho_mono_main(WINDOWS);
}
#elif defined(__ANDROID__)
extern "C" __attribute__((visibility("default"))) int SDL_main(int argc, char** argv);
extern "C" {
    int SDL_main(int argc, char** argv)
    {
        return start_urho_mono_main(_ANDROID_);
    }
}
#elif  defined(IOS) || defined(TVOS)
#define GD_PINVOKE_EXPORT extern "C" __attribute__((visibility("default")))
extern "C" __attribute__((visibility("default"))) int SDL_main(int argc, char** argv);
extern "C" {
    int SDL_main(int argc, char** argv)
    {
        ios::support::initialize();
        int res =  start_urho_mono_main(_IOS_);
        return res;
        
    }
}
#else
int main(int argc, char** argv)
{
#if defined(__APPLE__)
    Platform platform =  MACOS ;
    macos::support::initialize();
    return start_urho_mono_main(platform);
#elif   defined(__linux__)
    Platform platform =  LINUX ;
    return start_urho_mono_main(platform);
#endif
    
    return 0;
}
#endif

