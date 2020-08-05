#define HL_NAME(n) Urho3D_##n
extern "C"
{
#if defined(URHO3D_HAXE_HASHLINK)
#include <hashlink/hl.h>
#else
#include <hl.h>
#endif
}

#include "global_types.h"


HL_PRIM  int HL_NAME(_graphics_get_width)(urho3d_context *context)
{
   Graphics *graphics = context->GetSubsystem<Graphics>();
   if(graphics)
   {
       return graphics->GetWidth();
   }
   else
   {
       return 0;
   }
   
}

HL_PRIM  int HL_NAME(_graphics_get_height)(urho3d_context *context)
{
   Graphics *graphics = context->GetSubsystem<Graphics>();
   if(graphics)
   {
       return graphics->GetHeight();
   }
   else
   {
       return 0;
   }
   
}


DEFINE_PRIM(_I32, _graphics_get_width, URHO3D_CONTEXT );
DEFINE_PRIM(_I32, _graphics_get_height, URHO3D_CONTEXT );
