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

# Set CMake minimum version
cmake_minimum_required (VERSION 3.10.2)

# Set project name
project (Urho3D-Launcher)

# Set CMake modules search path
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMake/Modules)

set (INCLUDE_DIRS ${INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../Mono/include)

set(MONO_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../../Mono/libs/android/release)

add_library(monosgen-2.0 SHARED IMPORTED)
set_target_properties(monosgen-2.0 PROPERTIES IMPORTED_LOCATION
        ${MONO_DIR}/lib/${ANDROID_ABI}/libmonosgen-2.0.so)

add_library(MonoPosixHelper SHARED IMPORTED)
set_target_properties(MonoPosixHelper PROPERTIES IMPORTED_LOCATION
        ${MONO_DIR}/lib/${ANDROID_ABI}/libMonoPosixHelper.so)

# Include UrhoCommon.cmake module after setting project name
include (UrhoCommon)

add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/../../Source/Tools/Urho3DPlayer Urho3DPlayer)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/../../Source/Samples Samples)
