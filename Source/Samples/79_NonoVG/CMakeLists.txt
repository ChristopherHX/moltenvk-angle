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


if (NOT URHO3D_OPENGL)
    return ()
endif ()

# Define target name
set (TARGET_NAME 79_NonoVG)

add_definitions(-DSTB_IMAGE_STATIC=1)

if(ANDROID OR IOS OR TVOS OR WEB OR RPI4)
    add_definitions(-DNANOVG_GLES2_IMPLEMENTATION=1)
else()
    add_definitions(-DNANOVG_GL3_IMPLEMENTATION=1)
endif()

file (GLOB NANOVG_C_FILES nanovg/*.c)
file (GLOB NANOVG_URHO3D_FILES nanovgui/*.cpp)

set (INCLUDE_DIRS ${INCLUDE_DIRS} nanovg nanovgui nanosvg)

source_group("NanoVG Core"            FILES ${NANOVG_C_FILES})
source_group("NanoVG UI"            FILES ${NANOVG_URHO3D_FILES})

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES} ${NANOVG_C_FILES} ${NANOVG_URHO3D_FILES})

# Setup target with resource copying
setup_main_executable ()

# Setup test cases
setup_test ()
