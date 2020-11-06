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

set (CLING_HOME D:/temp/cling/tools/packaging/cling-build)
set (CLING_BUILD_DIR ${CLING_HOME}/builddir)


#if (CMAKE_BUILD_TYPE STREQUAL Release)
#    set (CLING_LIBPATH  ${CLING_BUILD_DIR}/Release/lib)
#else()
#set (CLING_LIBPATH  ${CLING_BUILD_DIR}/Debug/lib)
#endif()

set (CLING_LIBPATH  ${CLING_BUILD_DIR}/Release/lib)  
link_directories(${CLING_LIBPATH})

set (INCLUDE_DIRS ${INCLUDE_DIRS} ${CLING_HOME}/builddir/tools/cling/include ${CLING_HOME}/cling-src/tools/cling/include ${CLING_HOME}/cling-src/tools/cling/../clang/include ${CLING_HOME}/builddir/tools/cling/../clang/include  ${CLING_HOME}/builddir/include ${CLING_HOME}/cling-src/include)

set (LIBS 
clingInterpreter 
clingUtils.lib
clangCodeGen.lib
clangRewriteFrontend.lib
clangFrontend.lib
clangDriver.lib
clangParse.lib
clangRewrite.lib
clangSerialization.lib
clangSema.lib
clangAnalysis.lib
clangEdit.lib
clangAST.lib
clangLex.lib
clangBasic.lib
LLVMCoroutines.lib
LLVMCoverage.lib
LLVMLTO.lib
LLVMObjCARCOpts.lib
LLVMPasses.lib
LLVMOption.lib
LLVMOrcJIT.lib
LLVMExecutionEngine.lib
LLVMRuntimeDyld.lib
LLVMNVPTXCodeGen.lib
LLVMipo.lib
LLVMIRReader.lib
LLVMAsmParser.lib
LLVMInstrumentation.lib
LLVMLinker.lib
LLVMVectorize.lib
LLVMNVPTXDesc.lib
LLVMNVPTXAsmPrinter.lib
LLVMNVPTXInfo.lib
LLVMX86CodeGen.lib
LLVMAsmPrinter.lib
LLVMDebugInfoCodeView.lib
LLVMDebugInfoMSF.lib
LLVMSelectionDAG.lib
LLVMGlobalISel.lib
LLVMCodeGen.lib
LLVMScalarOpts.lib
LLVMInstCombine.lib
LLVMTarget.lib
LLVMTransformUtils.lib
LLVMBitWriter.lib
LLVMAnalysis.lib
LLVMProfileData.lib
LLVMX86AsmParser.lib
LLVMX86Desc.lib
LLVMObject.lib
LLVMBitReader.lib
LLVMMCParser.lib
LLVMX86AsmPrinter.lib
LLVMX86Disassembler.lib
LLVMX86Info.lib
LLVMMCDisassembler.lib
LLVMMC.lib
LLVMX86Utils.lib
LLVMCore.lib
LLVMBinaryFormat.lib
LLVMSupport.lib
LLVMDemangle.lib)

# Define source files
define_source_files (EXTRA_H_FILES ${COMMON_SAMPLE_H_FILES})



# Setup target with resource copying
setup_main_executable ()

set_target_properties(${TARGET_NAME}
  PROPERTIES ENABLE_EXPORTS 1)



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
