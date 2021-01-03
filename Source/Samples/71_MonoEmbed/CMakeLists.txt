#
# Copyright (c) 2008-2020 the Urho3D project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Define target name
set (TARGET_NAME 71_MonoEmbed)


if(WIN32)
set (MONO_LIBPATH  ../../../DotNet/libs/windows/debug)  
set (LIBS 
    libmono-static-sgen
    libmonoruntime-sgen
    libmonoutils
)
endif()

if(ANDROID)
set (MONO_LIBPATH  ../../../DotNet/libs/android/release/lib/${ANDROID_ABI}) 
set (LIBS 
    libmonosgen-2.0.so
    libmono-native.so
    libMonoPosixHelper.so
    libmono-btls-shared.so
)
endif()




if(APPLE)
if (IOS)
set (MONO_LIBPATH  ../../../DotNet/libs/iphone)  
set (LIBS 
    lib-urho3d-mono-aot.a
    libmonosgen-2.0.iphone.fat.a
    libmono-native.iphone.fat.a
    libmono-ee-interp.iphone.fat.a
    libmono-icall-table.iphone.fat.a
    libmono-ilgen.iphone.fat.a
    libmono-profiler-log.iphone.fat.a
    "-framework Security"
    "-framework GSS"
    "-framework CFNetwork"
    "-framework SystemConfiguration"
)
set(BITCODE "")
set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO" CACHE INTERNAL "")
file (GLOB S_FILES *.s)
file (GLOB EXTRA_MM_FILES *.mm)
else()

file (GLOB MACOS_FILES macos/*.*)

set (MONO_LIBPATH  ../../../DotNet/libs/macos)  
set (LIBS 
    libmonosgen-2.0.a
    libmono-native.a
    libSystem.Native.a
    libSystem.Security.Cryptography.Native.Apple.a
)
endif()
endif()

if(APPLE)
set (LIBS  ${LIBS}  z ncurses)
endif()




link_directories(. ${MONO_LIBPATH})

set (INCLUDE_DIRS ${INCLUDE_DIRS} ../../../DotNet/include)


if(WIN32)
set (LIBS  ${LIBS}  bcrypt)
endif()


# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} ${S_FILES} ${EXTRA_MM_FILES} ${MACOS_FILES})

# Setup target with resource copying
setup_main_executable ()

# Setup test cases
setup_test ()
