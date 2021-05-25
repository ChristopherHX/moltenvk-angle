// Copyright (c) 2020-2021 Eli Aloni (A.K.A elix22).
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

#include "AdmobPlugin.h"

#if defined(IOS)
void * IOS_AdmobInit(Context * context,AdmobPlugin * plugin);
#endif

AdmobPlugin::AdmobPlugin(Context* context) :
    Plugin(context)
{
#if defined(IOS)
    ios_plugin_handle = NULL;
#endif
#ifdef __ANDROID__
    LoadAndroidPlugin();
#endif
}

AdmobPlugin::~AdmobPlugin()
{

}


void RegisterAdmobPlugin(Context * context)
{
    context->RegisterPlugin<AdmobPlugin>();
}
