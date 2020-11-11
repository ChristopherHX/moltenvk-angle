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

# Keep symbols for JIT resolution
set(LLVM_NO_DEAD_STRIP 1)

# Cling needs at least C++11; so does this demo.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define target name
set (TARGET_NAME 71_ClingEmbedded)

# CLING_HOME must be set on CLI
#MAC
#-DCLING_HOME="/Users/elialoni/Development/cling_2020-10-27/cling/tools/packaging/cling-build"
#-DURHO3D_DLL_CLING_PATH="/Users/elialoni/Urho3D-Dev/cling-poc/Urho3D/build-cling-dylib/lib"
#WIN
#-DCLING_HOME="D:/temp/cling/tools/packaging/cling-build"
#-DURHO3D_DLL_CLING_PATH="D:/Urho3D-Dev/master/Urho3D/build_cling_lib/bin"
# i.e ./script/cmake_xcode_cling.sh  build-xcode-cling -DCLING_HOME="/Users/elialoni/Development/cling_2020-10-27/cling/tools/packaging/cling-build" -DURHO3D_DLL_CLING_PATH="/Users/elialoni/Urho3D-Dev/cling-poc/Urho3D/build-cling-dylib/lib"



set (CLING_BUILD_DIR ${CLING_HOME}/builddir)

if(WIN32)
set (CLING_LIBPATH  ${CLING_BUILD_DIR}/Release/lib)  
else()
set (CLING_LIBPATH  ${CLING_BUILD_DIR}/lib)  
endif()


link_directories(${CLING_LIBPATH})

set (INCLUDE_DIRS ${INCLUDE_DIRS} ${CLING_HOME}/builddir/tools/cling/include ${CLING_HOME}/cling-src/tools/cling/include ${CLING_HOME}/cling-src/tools/cling/../clang/include ${CLING_HOME}/builddir/tools/cling/../clang/include  ${CLING_HOME}/builddir/include ${CLING_HOME}/cling-src/include)


set (LIBS 
clingInterpreter 
clingUtils
clangCodeGen
clangRewriteFrontend
clangFrontend
clangDriver
clangParse
clangRewrite
clangSerialization
clangSema
clangAnalysis
clangEdit
clangAST
clangLex
clangBasic
LLVMCoroutines
LLVMCoverage
LLVMLTO
LLVMObjCARCOpts
LLVMPasses
LLVMOption
LLVMOrcJIT
LLVMExecutionEngine
LLVMRuntimeDyld
LLVMNVPTXCodeGen
LLVMipo
LLVMIRReader
LLVMAsmParser
LLVMInstrumentation
LLVMLinker
LLVMVectorize
LLVMNVPTXDesc
LLVMNVPTXAsmPrinter
LLVMNVPTXInfo
LLVMX86CodeGen
LLVMAsmPrinter
LLVMDebugInfoCodeView
LLVMDebugInfoMSF
LLVMSelectionDAG
LLVMGlobalISel
LLVMCodeGen
LLVMScalarOpts
LLVMInstCombine
LLVMTarget
LLVMTransformUtils
LLVMBitWriter
LLVMAnalysis
LLVMProfileData
LLVMX86AsmParser
LLVMX86Desc
LLVMObject
LLVMBitReader
LLVMMCParser
LLVMX86AsmPrinter
LLVMX86Disassembler
LLVMX86Info
LLVMMCDisassembler
LLVMMC
LLVMX86Utils
LLVMCore
LLVMBinaryFormat
LLVMSupport
LLVMDemangle)

if(APPLE)
set (LIBS  ${LIBS}  z ncurses)
endif()

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES})



# Setup target with resource copying
setup_main_executable ()

set_target_properties(${TARGET_NAME}
  PROPERTIES ENABLE_EXPORTS 1)

add_definitions(-DCLING_HOME="${CLING_HOME}")
add_definitions(-DCLING_BUILD_DIR="${CLING_BUILD_DIR}")
add_definitions(-DURHO3D_DLL_CLING_PATH="${URHO3D_DLL_CLING_PATH}")


if(MSVC)
  set_target_properties(${TARGET_NAME} PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS 1)
  set_property(TARGET ${TARGET_NAME} APPEND_STRING PROPERTY LINK_FLAGS
              "/EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00D_K@Z
               /EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00DM@Z
               /EXPORT:cling_runtime_internal_throwIfInvalidPointer
               /EXPORT:?setValueNoAlloc@internal@runtime@cling@@YAXPEAX00DPEBX@Z
               /EXPORT:??_7type_info@@6B@
               /EXPORT:?RegisterString@StringHashRegister@Urho3D@@QEAA?AVStringHash@2@PEBD@Z
               /EXPORT:?GetEventNameRegister@Urho3D@@YAAEAVStringHashRegister@1@XZ
               /EXPORT:?Resize@String@Urho3D@@QEAAXI@Z
               /EXPORT:?endZero@String@Urho3D@@0DA
               /EXPORT:??0StringHash@Urho3D@@QEAA@PEBD@Z
               /EXPORT:??3@YAXPEAXHPEBDH@Z")
endif()


# Setup test cases
setup_test ()
