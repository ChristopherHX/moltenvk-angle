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



#include "Hashlink.h"


extern "C" {
#include <hl.h>
#include <hlc.h>
}


static uchar *hlc_resolve_symbol( void *addr, uchar *out, int *outSize )
{
    return NULL;
}

static int hlc_capture_stack( void **stack, int size ) {
    int count = 0;
    return count;
}

int hashlink_main(int argc, char** argv);

#if defined(_MSC_VER) && defined(_DEBUG) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    char * str = ("hashlink_main");
    int argc=1;
    argv[1] = str;

    return hashlink_main(argc,argv);
}
#elif defined(_MSC_VER) && defined(URHO3D_MINIDUMPS) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    char * str = ("hashlink_main");
    int argc = 1;
    argv[1] = str;
    return hashlink_main(argc,argv);
}
#elif defined(_WIN32) && !defined(URHO3D_WIN32_CONSOLE)
static char* argv[1];
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
    char * str = ("hashlink_main");
    int argc = 1;
    argv[1] = str;
    return hashlink_main(argc,argv);
}
#elif defined(__ANDROID__) || defined(IOS) || defined(TVOS)
extern "C" __attribute__((visibility("default"))) int SDL_main(int argc, char** argv);
extern "C" {
int SDL_main(int argc, char** argv)
{
    return hashlink_main(argc,argv);
}
}
#else
int main(int argc, char** argv)
{
    return hashlink_main(argc,argv);
}
#endif

int hashlink_main(int argc, char** argv)
{
    #define sys_global_init()
    #define sys_global_exit()
    
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
    return 0;
}


