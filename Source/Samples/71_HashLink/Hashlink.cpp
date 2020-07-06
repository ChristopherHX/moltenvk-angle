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

#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Input/Input.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/LineEdit.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/ToolTip.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/Window.h>
#include <Urho3D/IO/Log.h>

#include "Hashlink.h"

//#define HL_MAC

extern "C" {
#include <hl.h>
#include <hlc.h>
char * hl_get_log_buffer();
}
static uchar *hlc_resolve_symbol( void *addr, uchar *out, int *outSize )
{
    return NULL;
}

static int hlc_capture_stack( void **stack, int size ) {
    int count = 0;
    return count;
}

extern "C" {
void hashlink_urho3d_log(char * str)
{
    String message=String(str);
    Urho3D::Log::Write(Urho3D::LOG_DEBUG, message);
}
}

#include <Urho3D/DebugNew.h>

URHO3D_DEFINE_APPLICATION_MAIN(Hashlink)




Hashlink::Hashlink(Context* context) :
    Sample(context),
    uiRoot_(GetSubsystem<UI>()->GetRoot())
{
}

void Hashlink::Start()
{
    UI* ui = GetSubsystem<UI>();
    // Execute base class startup
    Sample::Start();
    
   // ui->SetScale(2.0f);

    // Enable OS cursor
    GetSubsystem<Input>()->SetMouseVisible(true);

    // Load XML file containing default UI style sheet
    auto* cache = GetSubsystem<ResourceCache>();
    auto* style = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");

    // Set the loaded style as default style
    uiRoot_->SetDefaultStyle(style);

    // Initialize Window
   // InitWindow();

    // Create and add some controls to the Window
   // InitControls();

    // Set the mouse mode to use in the sample
    Sample::InitMouseMode(MM_FREE);
    
    
    
    urho3DConsole_ = SharedPtr<Urho3DConsole>(new Urho3DConsole(context_));
    urho3DConsole_->SetDefaultStyle(style);
    urho3DConsole_->GetWindow()->SetOpacity(1.0f);
    urho3DConsole_->SetVisible(true);
    
    
 
 
    InitAndExectuteHashlink();
    char * hl_logs = hl_get_log_buffer();
    URHO3D_LOGDEBUGF("%s",hl_logs);
    
}



void Hashlink::InitAndExectuteHashlink()
{
    
     #define sys_global_init()
     #define sys_global_exit()
     int argc=1;
     char t[5]="test";
     char *argv[1]={t};
     
     vdynamic *ret;
     bool isExc = false;
     hl_type_fun tf = { 0 };
     hl_type clt = {  };
     vclosure cl = {  };
     sys_global_init();
     hl_global_init();
     hl_register_thread(&ret);
     hl_setup_exception((void*)hlc_resolve_symbol,(void*)hlc_capture_stack);
     hl_setup_callbacks((void*)hlc_static_call, (void*)hlc_get_wrapper);
     hl_sys_init((void**)(argv + 1),argc - 1,NULL);
     tf.ret = &hlt_void;
     clt.kind = HFUN;
     clt.fun = &tf;
     cl.t = &clt;
     cl.fun = (void*)hl_entry_point;
     ret = hl_dyn_call_safe(&cl, NULL, 0, &isExc);
     hl_global_free();
     sys_global_exit();
    
}



