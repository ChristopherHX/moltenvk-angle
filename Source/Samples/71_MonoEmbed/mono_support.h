
#pragma once

#include "../Container/Str.h"
#include "../Core/Context.h"


typedef enum Platform
{
    ANDROID =1 ,
    _IOS_,
    TVOS,
    MACOS,
    WINDOWS,
    RASPBERRY_Pi,
    WEB,
    LINUX,
    UNKNOWN
}Platform;

using namespace Urho3D;


void mono_log_callback(const char *log_domain, const char *log_level, const char *message, mono_bool fatal, void *);

void fixPath(String& path);
String fixPathString(String  path);

bool CopyFileToDocumentsDir(Urho3D::SharedPtr<Urho3D::Context> context_,  String sourceFile);
void CopyMonoFilesToDocumentDir(Urho3D::SharedPtr<Urho3D::Context> context,Platform platform);
