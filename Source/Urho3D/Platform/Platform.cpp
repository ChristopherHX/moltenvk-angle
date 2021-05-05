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

#include "../Precompiled.h"

#include "../Core/Context.h"
#include "../Core/CoreEvents.h"
#include "../Core/Mutex.h"
#include "../Core/ProcessUtils.h"
#include "../Core/Profiler.h"
#include "../Core/StringUtils.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/GraphicsEvents.h"
#include "../IO/FileSystem.h"
#include "../IO/Log.h"
#include "../IO/RWOpsWrapper.h"
#include "../Input/Input.h"
#include "../Resource/JSONFile.h"
#include "../Resource/ResourceCache.h"
#include "../UI/Text.h"
#include "../UI/UI.h"
#include "../UI/UIEvents.h"
#include "../IO/IOEvents.h"
#include <SDL/SDL_events.h>

#include "Platform.h"

namespace Urho3D
{

int SDL_PLATFORM_EVENT = 0;

static Context* _context = NULL;

    // While there is an implementation only for Android, for other platforms while a noop
URHO3D_API bool PostCommandToPlatform(const JSONFile& data) 
{
    bool res = false;
#ifdef __ANDROID__
    res = PostCommandToAndroidPlatform(data);
#endif

    return res;
}

URHO3D_API bool PostCommandToPlatform(const String& method, JSONFile& data) 
{
    data.GetRoot()["class"] = "Internal";
    data.GetRoot()["method"] = method;
    return PostCommandToPlatform(data);
}

URHO3D_API bool PostCommandToPlatform(const String& clazz, const String& method, JSONFile& data) 
{
    data.GetRoot()["class"] = clazz;
    data.GetRoot()["method"] = method;
    return PostCommandToPlatform(data);
}

URHO3D_API void OnPlatformEvent(String strData)
{
    // will be deleted in Input
    VariantMap* pArgs = new VariantMap;
    (*pArgs)[PlatformNotify::P_DATA] = strData;
    SDL_Event event;
    SDL_zero(event);
    event.type = SDL_PLATFORM_EVENT;
    event.user.data2 = pArgs;
    event.user.code = E_PLATFORM_NOTIFY.Value();
    SDL_PushEvent(&event);

}

URHO3D_API void InitializePlatform(Context* context) {

     _context = context;
     SDL_PLATFORM_EVENT = SDL_RegisterEvents(1);
}

}
