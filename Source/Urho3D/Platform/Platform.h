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

/// \file

#pragma once

#include "../Container/FlagSet.h"
#include "../Container/HashSet.h"
#include "../Container/List.h"
#include "../Core/Mutex.h"
#include "../Core/Object.h"
#include "../Input/InputEvents.h"
#include "../UI/Cursor.h"

#include "PlatformEvents.h"

namespace Urho3D
{

#ifdef __ANDROID__
bool PostCommandToAndroidPlatform(const JSONFile& data);
#endif

extern int SDL_PLATFORM_EVENT;

URHO3D_API bool PostCommandToPlatform(const JSONFile& data);

URHO3D_API bool PostCommandToPlatform(const String& method, JSONFile& data);

URHO3D_API bool PostCommandToPlatform(const String& clazz, const String& method, JSONFile& data);

URHO3D_API void OnPlatformEvent(String strData);

URHO3D_API void InitializePlatform(Context* context);

}

