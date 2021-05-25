// Copyright (c) 2020-2021 Eli Aloni (A.K.A elix22).
// Copyright (c) 2008-2021 the Urho3D project.
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

#ifdef __ANDROID__
#include "../IO/IOEvents.h"
#include "../IO/Log.h"
#include "../Input/Input.h"
#include "../Precompiled.h"
#include "../Resource/JSONFile.h"
#include <SDL/SDL_events.h>
#include <jni.h>
#include "../Core/Plugin.h"
#include "AdmobPlugin.h"

using namespace Urho3D;

extern "C"
{
    JNIEnv* Android_JNI_GetEnv(void);

    JNIEXPORT void Java_com_urho3d_plugin_AdmobPlugin_OnNativePluginEvent(JNIEnv* env, jclass cls, jstring json)
    {
        const char* str = env->GetStringUTFChars(json, 0);
        String strData(str);
        Plugin::OnPluginEvent(strData);
        if (str)
        {
            env->ReleaseStringUTFChars(json, str);
        }  
    }
}

extern "C" __attribute__((visibility("default"))) 
bool PostCommandToAndroidAdmobJava(const JSONFile& data)
{
    JNIEnv* mEnv = Android_JNI_GetEnv();
    if (!mEnv)
    {
        URHO3D_LOGERROR("No mEnv in PostCommandToAndroidAdmob");
        return false;
    }
    static jclass mainClass = mEnv->FindClass("com/urho3d/plugin/AdmobPlugin");
    if (!mainClass)
    {
        URHO3D_LOGERROR("No mainClass in PostCommandToAndroidAdmob");
        return false;
    }
    static jmethodID postCommand = mEnv->GetStaticMethodID(mainClass, "postCommand", "(Ljava/lang/String;)V");
    if (!postCommand)
    {
        URHO3D_LOGERROR("No postCommand in PostCommandToAndroidAdmob");
        return false;
    }
    jstring jparam = (jstring)mEnv->NewStringUTF(data.ToString(String()).CString());
    if (!jparam)
    {
        URHO3D_LOGERROR("No jparam in PostCommandToAndroidAdmob");
        return false;
    }
    mEnv->CallStaticVoidMethod(mainClass, postCommand, jparam);
    mEnv->DeleteLocalRef(jparam);

    return true;
}

extern "C" __attribute__((visibility("default"))) 
bool LoadPluginJava()
{
    JNIEnv* mEnv = Android_JNI_GetEnv();
    if (!mEnv)
    {
        URHO3D_LOGERROR("No mEnv in PostCommandToAndroidAdmob");
        return false;
    }
    static jclass mainClass = mEnv->FindClass("com/urho3d/plugin/AdmobPlugin");
    if (!mainClass)
    {
        URHO3D_LOGERROR("No mainClass in AdmobPlugin");
        return false;
    }
    static jmethodID loadPlugin = mEnv->GetStaticMethodID(mainClass, "LoadPlugin", "()V");
    if (!loadPlugin)
    {
        URHO3D_LOGERROR("No LoadPlugin in AdmobPlugin");
        return false;
    }

    mEnv->CallStaticVoidMethod(mainClass, loadPlugin, NULL);

    return true;
}

bool AdmobPlugin::PostCommandToAndroid(Context * context,const String& method,JSONFile& data)
{
    bool res = false;
    data.GetRoot()["method"] = method;
    res = PostCommandToAndroidAdmobJava(data);
    return res;
}

 bool AdmobPlugin::LoadAndroidPlugin()
 {
     return LoadPluginJava();
 }

#endif // __ANDROID__
