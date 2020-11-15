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
set (TARGET_NAME rttr)

add_definitions(-DNOMINMAX)

# Define source files
file (GLOB CPP_FILES rttr/*.cpp
          rttr/detail/base/*.cpp
          rttr/detail/comparison/*.cpp
          rttr/detail/constructor/*.cpp
          rttr/detail/conversion/*.cpp
          rttr/detail/default_arguments/*.cpp
          rttr/detail/destructor/*.cpp
          rttr/detail/enumeration/*.cpp
          rttr/detail/filter/*.cpp
          rttr/detail/impl/*.cpp
          rttr/detail/library/*.cpp
          rttr/detail/metadata/*.cpp
          rttr/detail/method/*.cpp
          rttr/detail/misc/*.cpp
          rttr/detail/parameter_info/*.cpp
          rttr/detail/policies/*.cpp
          rttr/detail/property/*.cpp
          rttr/detail/registration/*.cpp
          rttr/detail/type/*.cpp
          rttr/detail/variant/*.cpp
          rttr/detail/variant_associative_view/*.cpp
          rttr/detail/variant_sequential_view/*.cpp
          rttr/detail/visitorp/*.cpp )

file (GLOB H_FILES rttr/*.h           
          rttr/detail/base/*.h
          rttr/detail/comparison/*.h
          rttr/detail/constructor/*.h
          rttr/detail/conversion/*.h
          rttr/detail/default_arguments/*.h
          rttr/detail/destructor/*.h
          rttr/detail/enumeration/*.h
          rttr/detail/filter/*.h
          rttr/detail/impl/*.h
          rttr/detail/library/*.h
          rttr/detail/metadata/*.h
          rttr/detail/method/*.h
          rttr/detail/misc/*.h
          rttr/detail/parameter_info/*.h
          rttr/detail/policies/*.h
          rttr/detail/property/*.h
          rttr/detail/registration/*.h
          rttr/detail/type/*.h
          rttr/detail/variant/*.h
          rttr/detail/variant_associative_view/*.h
          rttr/detail/variant_sequential_view/*.h
          rttr/detail/visitor/*.h)

set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

set (INCLUDE_DIRS .)

# Setup target
setup_library ()

# Install headers for building the Urho3D library
install_header_files (DIRECTORY rttr/ DESTINATION ${DEST_INCLUDE_DIR}/ThirdParty/rttr FILES_MATCHING PATTERN *.h)  # Note: the trailing slash is significant
