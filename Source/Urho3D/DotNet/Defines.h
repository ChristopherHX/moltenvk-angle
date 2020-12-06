#pragma once

#ifdef _MSC_VER
    #define __CDECL __cdecl
#else
    #define __CDECL
#endif

//#define bool int

#ifndef URHO3D_API
#if defined(WIN32)
        #define URHO3D_API __declspec(dllexport)
#else
        #define  URHO3D_API __attribute__((visibility("default")))
#endif  
#endif
