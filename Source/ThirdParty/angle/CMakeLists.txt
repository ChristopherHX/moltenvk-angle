#
# Copyright (c) 2008-2019 the Urho3D project.
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
# 20190119 - Eli Aloni (A.K.A elix22) - initial creation

set (TARGET_NAME angle)

add_definitions(-DANGLE_ENABLE_VULKAN -DAPI_NAME=\"Vulkan\" -DVULKAN_NON_CMAKE_BUILD -DSDL_VIDEO_STATIC_ANGLE -DANGLE_EXPORT=  -DANGLE_VK_ICD_JSON="" -DANGLE_VK_DATA_DIR=""  -DANGLE_VK_MOCK_ICD_JSON="$angle_data_dir/VkICD_mock_icd.json"  -DANGLE_VK_SWIFTSHADER_ICD_JSON="${_sws_icd}" -DANGLE_VK_LAYERS_DIR="$angle_data_dir" -DANGLE_CAPTURE_ENABLED)
#add_definitions(-DGL_API=  -DGL_APIENTRY=  -DEGLAPI=  -DANGLE_PLATFORM_EXPORT=    -DGL_APICALL=  -DGL_APIENTRY= -DANGLE_EXPORT= -DEGLAPIENTRY= -DANGLE_APIENTRY=)



if(ANDROID)
	add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR  -DVK_USE_PLATFORM_ANDROID_KHX -DREDEF )
	add_definitions(-DSYSCONFDIR=\"/etc\"   -DFALLBACK_CONFIG_DIRS=\"/etc/xdg\" -DFALLBACK_DATA_DIRS=\"/usr/local/share:/usr/share\")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wincompatible-pointer-types")
endif()

if(IOS)
	add_definitions(-DVK_USE_PLATFORM_IOS_MVK )
endif()

if(TVOS)
	add_definitions(-DVK_USE_PLATFORM_IOS_MVK )
endif()

if(APPLE)
	add_definitions(-DVK_USE_PLATFORM_MACOS_MVK)
	add_definitions(-DSYSCONFDIR=\"/etc\"   -DFALLBACK_CONFIG_DIRS=\"/etc/xdg\" -DFALLBACK_DATA_DIRS=\"/usr/local/share:/usr/share\")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wincompatible-pointer-types")
endif()

if(WIN32)
	add_definitions(-DVK_USE_PLATFORM_WIN32_KHR  -DVK_USE_PLATFORM_WIN32_KHX -DANGLE_PLATFORM_WINDOWS -DSPIRV_WINDOWS -DWIN32_LEAN_AND_MEAN -DNOMINMAX /bigobj  /EHsc)
  add_definitions(-DGL_API=  -DGL_APIENTRY=  -DEGLAPI=  -DANGLE_PLATFORM_EXPORT=    -DGL_APICALL=  -DGL_APIENTRY= -DANGLE_EXPORT= -DEGLAPIENTRY= -DANGLE_APIENTRY=)
endif()

if(__linux__)
	add_definitions(-DVK_USE_PLATFORM_XCB_KHR  -DVK_USE_PLATFORM_XCB_KHX)
endif()



set (libangle_common_sources
  src/common/Color.inc
  src/common/Float16ToFloat32.cpp
  src/common/MemoryBuffer.cpp
  src/common/PackedEGLEnums_autogen.cpp
  src/common/PackedEnums.cpp
  src/common/PackedGLEnums_autogen.cpp
  src/common/android_util.cpp
  src/common/aligned_memory.cpp
  src/common/angleutils.cpp
  src/common/debug.cpp
  src/common/mathutil.cpp
  src/common/matrix_utils.cpp
  src/common/PoolAlloc.cpp
  src/common/string_utils.cpp
  src/common/system_utils.cpp
  src/common/third_party/base/anglebase/sha1.cc
  src/common/third_party/smhasher/src/PMurHash.cpp
  src/common/third_party/xxhash/xxhash.c
  src/common/tls.cpp
  src/common/uniform_type_info_autogen.cpp
  src/common/utilities.cpp)

source_group("angle_common"            FILES ${libangle_common_sources})

list (APPEND SOURCE_FILES ${libangle_common_sources})



set (libangle_image_util_sources
  src/image_util/copyimage.cpp
  src/image_util/copyimage.h
  src/image_util/copyimage.inc
  src/image_util/generatemip.h
  src/image_util/generatemip.inc
  src/image_util/imageformats.cpp
  src/image_util/imageformats.h
  src/image_util/loadimage.cpp
  src/image_util/loadimage.h
  src/image_util/loadimage.inc
  src/image_util/loadimage_etc.cpp
)

if (WIN32)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_win.cpp)
endif()

if(ANDROID)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_linux.cpp)
endif()

if(__linux__)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_linux.cpp)
endif()

if(APPLE)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_mac.cpp)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_posix.cpp)
endif()

if(IOS)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_mac.cpp)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_posix.cpp)
endif()

if(TVOS)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_mac.cpp)
	set (libangle_image_util_sources ${libangle_image_util_sources} src/common/system_utils_posix.cpp)
endif()



source_group("angle_image_util"            FILES ${libangle_image_util_sources})

list (APPEND SOURCE_FILES ${libangle_image_util_sources})



set (libangle_sources
  src/common/event_tracer.cpp
  src/libANGLE/AttributeMap.cpp
  src/libANGLE/BlobCache.cpp
  src/libANGLE/Buffer.cpp
  src/libANGLE/Caps.cpp
  src/libANGLE/Compiler.cpp
  src/libANGLE/capture_gles_1_0_autogen.cpp
  src/libANGLE/capture_gles_1_0_params.cpp
  src/libANGLE/capture_gles_2_0_autogen.cpp
  src/libANGLE/capture_gles_2_0_params.cpp
  src/libANGLE/capture_gles_3_0_autogen.cpp
  src/libANGLE/capture_gles_3_0_params.cpp
  src/libANGLE/capture_gles_3_1_autogen.cpp
  src/libANGLE/capture_gles_3_1_params.cpp
  src/libANGLE/capture_gles_3_2_autogen.cpp
  src/libANGLE/capture_gles_3_2_params.cpp
  src/libANGLE/capture_gles_ext_autogen.cpp
  src/libANGLE/capture_gles_ext_params.cpp
  src/libANGLE/Config.cpp
  src/libANGLE/Context.cpp
  src/libANGLE/Context_gl.cpp
  src/libANGLE/Context_gles_1_0.cpp
  src/libANGLE/Debug.cpp
  src/libANGLE/Device.cpp
  src/libANGLE/Display.cpp
  src/libANGLE/EGLSync.cpp
  src/libANGLE/entry_points_enum_autogen.cpp
  src/libANGLE/Error.cpp
  src/libANGLE/Fence.cpp
  src/libANGLE/format_map_desktop.cpp
  src/libANGLE/Framebuffer.cpp
  src/libANGLE/FramebufferAttachment.cpp
  src/libANGLE/FrameCapture.cpp
  src/libANGLE/formatutils.cpp
  src/libANGLE/frame_capture_replay_autogen.cpp
  src/libANGLE/frame_capture_utils_autogen.cpp
  src/libANGLE/gl_enum_utils.cpp
  src/libANGLE/gl_enum_utils_autogen.cpp
  src/libANGLE/GLES1Renderer.cpp
  src/libANGLE/GLES1State.cpp
  src/libANGLE/HandleAllocator.cpp
  src/libANGLE/HandleRangeAllocator.cpp
  src/libANGLE/Image.cpp
  src/libANGLE/ImageIndex.cpp
  src/libANGLE/IndexRangeCache.cpp
  src/libANGLE/LoggingAnnotator.cpp
  src/libANGLE/MemoryObject.cpp
  src/libANGLE/MemoryProgramCache.cpp
  src/libANGLE/Observer.cpp
  src/libANGLE/Overlay.cpp
  src/libANGLE/Overlay_autogen.cpp
  src/libANGLE/Overlay_font_autogen.cpp
  src/libANGLE/OverlayWidgets.cpp
  src/libANGLE/Path.cpp
  src/libANGLE/Platform.cpp
  src/libANGLE/Program.cpp
  src/libANGLE/ProgramLinkedResources.cpp
  src/libANGLE/ProgramPipeline.cpp
  src/libANGLE/Query.cpp
  src/libANGLE/Renderbuffer.cpp
  src/libANGLE/ResourceManager.cpp
  src/libANGLE/Sampler.cpp
  src/libANGLE/Semaphore.cpp
  src/libANGLE/Shader.cpp
  src/libANGLE/State.cpp
  src/libANGLE/Stream.cpp
  src/libANGLE/Surface.cpp
  src/libANGLE/Texture.cpp
  src/libANGLE/Thread.cpp
  src/libANGLE/TransformFeedback.cpp
  src/libANGLE/Uniform.cpp
  src/libANGLE/validationEGL.cpp
  src/libANGLE/validationES.cpp
  src/libANGLE/validationES1.cpp
  src/libANGLE/validationES2.cpp
  src/libANGLE/validationES3.cpp
  src/libANGLE/validationES31.cpp
  src/libANGLE/validationESEXT.cpp
  src/libANGLE/validationES32.cpp
  src/libANGLE/VaryingPacking.cpp
  src/libANGLE/VertexArray.cpp
  src/libANGLE/VertexAttribute.cpp
  src/libANGLE/VertexAttribute.inc
  src/libANGLE/WorkerThread.cpp
  src/libANGLE/angletypes.cpp
  src/libANGLE/es3_copy_conversion_table_autogen.cpp
  src/libANGLE/format_map_autogen.cpp
  src/libANGLE/formatutils.cpp
  src/libANGLE/queryconversions.cpp
  src/libANGLE/queryutils.cpp
  src/libANGLE/renderer/ContextImpl.cpp
  src/libANGLE/renderer/driver_utils.cpp
  src/libANGLE/renderer/DeviceImpl.cpp
  src/libANGLE/renderer/DisplayImpl.cpp
  src/libANGLE/renderer/Format_table_autogen.cpp
  src/libANGLE/renderer/glslang_wrapper_utils.cpp
  src/libANGLE/renderer/QueryImpl.cpp
  src/libANGLE/renderer/SurfaceImpl.cpp
  src/libANGLE/renderer/TextureImpl.cpp
  src/libANGLE/renderer/load_functions_table_autogen.cpp
  src/libANGLE/renderer/renderer_utils.cpp
  src/libANGLE/renderer/ShaderImpl.cpp
)


source_group("libANGLE"            FILES ${libangle_sources})

list (APPEND SOURCE_FILES ${libangle_sources})


set(libangle_vulkan_sources
  src/libANGLE/renderer/vulkan/BufferVk.cpp
  src/libANGLE/renderer/vulkan/CommandGraph.cpp
  src/libANGLE/renderer/vulkan/CompilerVk.cpp
  src/libANGLE/renderer/vulkan/ContextVk.cpp
  src/libANGLE/renderer/vulkan/DeviceVk.cpp
  src/libANGLE/renderer/vulkan/DisplayVk.cpp
  src/libANGLE/renderer/vulkan/FenceNVVk.cpp
  src/libANGLE/renderer/vulkan/FramebufferVk.cpp
  src/libANGLE/renderer/vulkan/GlslangWrapperVk.cpp
  src/libANGLE/renderer/vulkan/ImageVk.cpp
  src/libANGLE/renderer/vulkan/MemoryObjectVk.cpp
  src/libANGLE/renderer/vulkan/OverlayVk.cpp
  src/libANGLE/renderer/vulkan/PersistentCommandPool.cpp
  src/libANGLE/renderer/vulkan/ProgramVk.cpp
  src/libANGLE/renderer/vulkan/ProgramPipelineVk.cpp
  src/libANGLE/renderer/vulkan/QueryVk.cpp
  src/libANGLE/renderer/vulkan/RenderbufferVk.cpp
  src/libANGLE/renderer/vulkan/RendererVk.cpp
  src/libANGLE/renderer/vulkan/RenderTargetVk.cpp
  src/libANGLE/renderer/vulkan/SamplerVk.cpp
  src/libANGLE/renderer/vulkan/SecondaryCommandBuffer.cpp
  src/libANGLE/renderer/vulkan/SemaphoreVk.cpp
  src/libANGLE/renderer/vulkan/ShaderVk.cpp
  src/libANGLE/renderer/vulkan/SurfaceVk.cpp
  src/libANGLE/renderer/vulkan/SyncVk.cpp
  src/libANGLE/renderer/vulkan/TextureVk.cpp
  src/libANGLE/renderer/vulkan/TransformFeedbackVk.cpp
  src/libANGLE/renderer/vulkan/UtilsVk.cpp
  src/libANGLE/renderer/vulkan/VertexArrayVk.cpp
  src/libANGLE/renderer/vulkan/vk_cache_utils.cpp
  src/libANGLE/renderer/vulkan/vk_caps_utils.cpp
  src/libANGLE/renderer/vulkan/vk_format_table_autogen.cpp
  src/libANGLE/renderer/vulkan/vk_format_utils.cpp
  src/libANGLE/renderer/vulkan/vk_helpers.cpp
  src/libANGLE/renderer/vulkan/vk_internal_shaders_autogen.cpp
  src/libANGLE/renderer/vulkan/vk_mandatory_format_support_table_autogen.cpp
  src/libANGLE/renderer/vulkan/vk_utils.cpp
)

if (WIN32)
	set (libangle_vulkan_win32_sources
	  src/libANGLE/renderer/vulkan/win32/DisplayVkWin32.cpp
	  src/libANGLE/renderer/vulkan/win32/WindowSurfaceVkWin32.cpp
	)
	set (libangle_vulkan_sources ${libangle_vulkan_sources} ${libangle_vulkan_win32_sources})
endif()

if(ANDROID)
	set (libangle_vulkan_android_sources
	  src/libANGLE/renderer/vulkan/android/DisplayVkAndroid.cpp
	  src/libANGLE/renderer/vulkan/android/WindowSurfaceVkAndroid.cpp
	  )
	set (libangle_vulkan_sources ${libangle_vulkan_sources} ${libangle_vulkan_android_sources}
)
endif()

if(APPLE)
  if (ARM)
  set (libangle_vulkan_ios_sources
  src/libANGLE/renderer/vulkan/iOS/DisplayVkiOS.mm
  src/libANGLE/renderer/vulkan/iOS/WindowSurfaceVkiOS.mm)
  set (libangle_vulkan_sources ${libangle_vulkan_sources} ${libangle_vulkan_ios_sources})
  else ()
    set (libangle_vulkan_macos_sources
    src/libANGLE/renderer/vulkan/macos/DisplayVkMacOS.mm
    src/libANGLE/renderer/vulkan/macos/WindowSurfaceVkMacOS.mm)
    set (libangle_vulkan_sources ${libangle_vulkan_sources} ${libangle_vulkan_macos_sources})
  endif()
endif()

source_group("libANGLE_renderer_vulkan"            FILES ${libangle_vulkan_sources})

list (APPEND SOURCE_FILES ${libangle_vulkan_sources})



set (libglesv2_sources
  src/libGLESv2/entry_points_egl.cpp
  src/libGLESv2/entry_points_egl_ext.cpp
  src/libGLESv2/entry_points_gles_1_0_autogen.cpp
  src/libGLESv2/entry_points_gles_2_0_autogen.cpp
  src/libGLESv2/entry_points_gles_3_0_autogen.cpp
  src/libGLESv2/entry_points_gles_3_1_autogen.cpp
  src/libGLESv2/entry_points_gles_3_2_autogen.cpp
  src/libGLESv2/entry_points_gles_ext_autogen.cpp
  src/libGLESv2/global_state.cpp
  src/libGLESv2/libGLESv2_autogen.cpp
  src/libGLESv2/libGLESv2.rc
  src/libGLESv2/proc_table_autogen.cpp
)
source_group("libGLESv2"            FILES ${libglesv2_sources})
list (APPEND SOURCE_FILES ${libglesv2_sources})




set (libegl_sources
  src/libEGL/libEGL.cpp
  src/libEGL/resource.h
)
source_group("libEGL"            FILES ${libegl_sources})
list (APPEND SOURCE_FILES ${libegl_sources})



set (angle_translator_sources
  src/compiler/translator/BuiltInFunctionEmulator.cpp
  src/compiler/translator/BuiltinsWorkaroundGLSL.cpp
  src/compiler/translator/CallDAG.cpp
  src/compiler/translator/CodeGen.cpp
  src/compiler/translator/CollectVariables.cpp
  src/compiler/translator/Compiler.cpp
  src/compiler/translator/ConstantUnion.cpp
  src/compiler/translator/Declarator.cpp
  src/compiler/translator/Diagnostics.cpp
  src/compiler/translator/DirectiveHandler.cpp
  src/compiler/translator/ExtensionBehavior.cpp
  src/compiler/translator/FlagStd140Structs.cpp
  src/compiler/translator/FunctionLookup.cpp
  src/compiler/translator/HashNames.cpp
  src/compiler/translator/ImmutableString_autogen.cpp
  src/compiler/translator/ImmutableString_ESSL_autogen.cpp
  src/compiler/translator/ImmutableStringBuilder.cpp
  src/compiler/translator/InfoSink.cpp
  src/compiler/translator/Initialize.cpp
  src/compiler/translator/InitializeDll.cpp
  src/compiler/translator/IntermNode.cpp
  src/compiler/translator/IsASTDepthBelowLimit.cpp
  src/compiler/translator/Operator.cpp
  src/compiler/translator/OutputTree.cpp
  src/compiler/translator/ParseContext.cpp
  src/compiler/translator/PoolAlloc.cpp
  src/compiler/translator/QualifierTypes.cpp
  src/compiler/translator/ShaderLang.cpp
  src/compiler/translator/ShaderVars.cpp
  src/compiler/translator/Symbol.cpp
  src/compiler/translator/SymbolTable.cpp
  src/compiler/translator/SymbolTable_autogen.cpp
  src/compiler/translator/SymbolUniqueId.cpp
  src/compiler/translator/Types.cpp
  src/compiler/translator/ValidateGlobalInitializer.cpp
  src/compiler/translator/ValidateLimitations.cpp
  src/compiler/translator/ValidateMaxParameters.cpp
  src/compiler/translator/ValidateOutputs.cpp
  src/compiler/translator/ValidateSwitch.cpp
  src/compiler/translator/ValidateVaryingLocations.cpp
  src/compiler/translator/VariablePacker.cpp
  src/compiler/translator/blocklayout.cpp
  src/compiler/translator/glslang_lex.cpp
  src/compiler/translator/glslang_tab.cpp
  src/compiler/translator/util.cpp
  src/compiler/translator/tree_ops/AddAndTrueToLoopCondition.cpp
  src/compiler/translator/tree_ops/BreakVariableAliasingInInnerLoops.cpp
  src/compiler/translator/tree_ops/ClampFragDepth.cpp
  src/compiler/translator/tree_ops/ClampPointSize.cpp
  src/compiler/translator/tree_ops/DeclareAndInitBuiltinsForInstancedMultiview.cpp
  src/compiler/translator/tree_ops/DeferGlobalInitializers.cpp
  src/compiler/translator/tree_ops/EmulateMultiDrawShaderBuiltins.cpp
  src/compiler/translator/tree_ops/EmulateGLFragColorBroadcast.cpp
  src/compiler/translator/tree_ops/EmulatePrecision.cpp
  src/compiler/translator/tree_ops/ExpandIntegerPowExpressions.cpp
  src/compiler/translator/tree_ops/FoldExpressions.cpp
  src/compiler/translator/tree_ops/InitializeVariables.cpp
  src/compiler/translator/tree_ops/NameEmbeddedUniformStructs.cpp
  src/compiler/translator/tree_ops/NameNamelessUniformBuffers.cpp
  src/compiler/translator/tree_ops/PruneEmptyCases.cpp
  src/compiler/translator/tree_ops/PruneNoOps.cpp
  src/compiler/translator/tree_ops/RecordConstantPrecision.cpp
  src/compiler/translator/tree_ops/RegenerateStructNames.cpp
  src/compiler/translator/tree_ops/RemoveArrayLengthMethod.cpp
  src/compiler/translator/tree_ops/RemoveInvariantDeclaration.cpp
  src/compiler/translator/tree_ops/RemovePow.cpp
  src/compiler/translator/tree_ops/RemoveUnreferencedVariables.cpp
  src/compiler/translator/tree_ops/RewriteAtomicCounters.cpp
  src/compiler/translator/tree_ops/RewriteAtomicFunctionExpressions.cpp
  src/compiler/translator/tree_ops/RewriteCubeMapSamplersAs2DArray.cpp
  src/compiler/translator/tree_ops/RewriteDfdy.cpp
  src/compiler/translator/tree_ops/RewriteDoWhile.cpp
  src/compiler/translator/tree_ops/RewriteRowMajorMatrices.cpp
  src/compiler/translator/tree_ops/RewriteStructSamplers.cpp
  src/compiler/translator/tree_ops/RewriteStructSamplersOld.cpp
  src/compiler/translator/tree_ops/RewriteRepeatedAssignToSwizzled.cpp
  src/compiler/translator/tree_ops/RewriteTexelFetchOffset.cpp
  src/compiler/translator/tree_ops/RewriteUnaryMinusOperatorFloat.cpp
  src/compiler/translator/tree_ops/RewriteUnaryMinusOperatorInt.cpp
  src/compiler/translator/tree_ops/ScalarizeVecAndMatConstructorArgs.cpp
  src/compiler/translator/tree_ops/SeparateDeclarations.cpp
  src/compiler/translator/tree_ops/SimplifyLoopConditions.cpp
  src/compiler/translator/tree_ops/SplitSequenceOperator.cpp
  src/compiler/translator/tree_ops/UnfoldShortCircuitAST.cpp
  src/compiler/translator/tree_ops/UseInterfaceBlockFields.cpp
  src/compiler/translator/tree_ops/VectorizeVectorScalarArithmetic.cpp
  src/compiler/translator/tree_util/FindFunction.cpp
  src/compiler/translator/tree_util/FindMain.cpp
  src/compiler/translator/tree_util/FindSymbolNode.cpp
  src/compiler/translator/tree_util/IntermNodePatternMatcher.cpp
  src/compiler/translator/tree_util/IntermNode_util.cpp
  src/compiler/translator/tree_util/IntermTraverse.cpp
  src/compiler/translator/tree_util/ReplaceVariable.cpp
  src/compiler/translator/tree_util/ReplaceShadowingVariables.cpp
  src/compiler/translator/tree_util/RunAtTheEndOfShader.cpp
  src/third_party/compiler/ArrayBoundsClamper.cpp
  src/compiler/translator/OutputESSL.cpp
  src/compiler/translator/TranslatorESSL.cpp
  src/compiler/translator/BuiltInFunctionEmulatorGLSL.cpp
  src/compiler/translator/ExtensionGLSL.cpp
  src/compiler/translator/OutputGLSL.cpp
  src/compiler/translator/OutputGLSLBase.cpp
  src/compiler/translator/TranslatorGLSL.cpp
  src/compiler/translator/VersionGLSL.cpp
  src/compiler/translator/tree_ops/AddDefaultReturnStatements.cpp
  src/compiler/translator/tree_ops/ArrayReturnValueToOutParameter.cpp
  src/compiler/translator/tree_ops/RemoveDynamicIndexing.cpp
  src/compiler/translator/tree_ops/RemoveSwitchFallThrough.cpp
  src/compiler/translator/tree_ops/RewriteElseBlocks.cpp
  src/compiler/translator/tree_ops/SeparateArrayConstructorStatements.cpp
  src/compiler/translator/tree_ops/SeparateArrayInitialization.cpp
  src/compiler/translator/tree_ops/SeparateExpressionsReturningArrays.cpp
  src/compiler/translator/tree_ops/UnfoldShortCircuitToIf.cpp
  src/compiler/translator/tree_ops/WrapSwitchStatementsInBlocks.cpp
  src/compiler/translator/OutputVulkanGLSL.cpp
  src/compiler/translator/TranslatorVulkan.cpp
  src/compiler/translator/ValidateAST.cpp
  src/compiler/fuzz/translator_fuzzer.cpp
)
source_group("translator"            FILES ${angle_translator_sources})
list (APPEND SOURCE_FILES ${angle_translator_sources})


set (angle_preprocessor_sources
  src/compiler/preprocessor/DiagnosticsBase.cpp
  src/compiler/preprocessor/DirectiveHandlerBase.cpp
  src/compiler/preprocessor/DirectiveParser.cpp
  src/compiler/preprocessor/ExpressionParser.cpp
  src/compiler/preprocessor/Input.cpp
  src/compiler/preprocessor/Lexer.cpp
  src/compiler/preprocessor/Macro.cpp
  src/compiler/preprocessor/MacroExpander.cpp
  src/compiler/preprocessor/Preprocessor.cpp
  src/compiler/preprocessor/Token.cpp
  src/compiler/preprocessor/Tokenizer.cpp
)
source_group("preprocessor"            FILES ${angle_preprocessor_sources})
list (APPEND SOURCE_FILES ${angle_preprocessor_sources})



set(glslang
    third_party/glslang/src/OGLCompilersDLL/InitializeDll.cpp
    third_party/glslang/src/SPIRV/GlslangToSpv.cpp
    third_party/glslang/src/SPIRV/InReadableOrder.cpp
    third_party/glslang/src/SPIRV/Logger.cpp
    third_party/glslang/src/SPIRV/SpvBuilder.cpp
    third_party/glslang/src/SPIRV/SpvPostProcess.cpp
    third_party/glslang/src/SPIRV/disassemble.cpp
    third_party/glslang/src/SPIRV/doc.cpp
    third_party/glslang/src/StandAlone/ResourceLimits.cpp
    third_party/glslang/src/glslang/GenericCodeGen/CodeGen.cpp
    third_party/glslang/src/glslang/GenericCodeGen/Link.cpp
    third_party/glslang/src/glslang/MachineIndependent/Constant.cpp
    third_party/glslang/src/glslang/MachineIndependent/InfoSink.cpp
    third_party/glslang/src/glslang/MachineIndependent/Initialize.cpp
    third_party/glslang/src/glslang/MachineIndependent/IntermTraverse.cpp
    third_party/glslang/src/glslang/MachineIndependent/Intermediate.cpp
    third_party/glslang/src/glslang/MachineIndependent/ParseContextBase.cpp
    third_party/glslang/src/glslang/MachineIndependent/ParseHelper.cpp
    third_party/glslang/src/glslang/MachineIndependent/PoolAlloc.cpp
    third_party/glslang/src/glslang/MachineIndependent/RemoveTree.cpp
    third_party/glslang/src/glslang/MachineIndependent/Scan.cpp
    third_party/glslang/src/glslang/MachineIndependent/ShaderLang.cpp
    third_party/glslang/src/glslang/MachineIndependent/SymbolTable.cpp
    third_party/glslang/src/glslang/MachineIndependent/Versions.cpp
    third_party/glslang/src/glslang/MachineIndependent/attribute.cpp
    third_party/glslang/src/glslang/MachineIndependent/glslang_tab.cpp
    third_party/glslang/src/glslang/MachineIndependent/intermOut.cpp
    third_party/glslang/src/glslang/MachineIndependent/iomapper.cpp
    third_party/glslang/src/glslang/MachineIndependent/limits.cpp
    third_party/glslang/src/glslang/MachineIndependent/linkValidate.cpp
    third_party/glslang/src/glslang/MachineIndependent/parseConst.cpp
    third_party/glslang/src/glslang/MachineIndependent/preprocessor/Pp.cpp
    third_party/glslang/src/glslang/MachineIndependent/preprocessor/PpAtom.cpp
    third_party/glslang/src/glslang/MachineIndependent/preprocessor/PpContext.cpp
    third_party/glslang/src/glslang/MachineIndependent/preprocessor/PpScanner.cpp
    third_party/glslang/src/glslang/MachineIndependent/preprocessor/PpTokens.cpp
    third_party/glslang/src/glslang/MachineIndependent/propagateNoContraction.cpp
    third_party/glslang/src/glslang/MachineIndependent/reflection.cpp
	)

if (WIN32)
	set (glslang ${glslang} third_party/glslang/src/glslang/OSDependent/Windows/ossource.cpp)
else()
	set (glslang ${glslang} third_party/glslang/src/glslang/OSDependent/Unix/ossource.cpp)
endif()
	
source_group("glslang"            FILES ${glslang})
list (APPEND SOURCE_FILES ${glslang})



 set(spvtools
    third_party/spirv-tools/src/source/assembly_grammar.cpp
    third_party/spirv-tools/src/source/binary.cpp
    third_party/spirv-tools/src/source/diagnostic.cpp
    third_party/spirv-tools/src/source/disassemble.cpp
    third_party/spirv-tools/src/source/enum_string_mapping.cpp
    third_party/spirv-tools/src/source/ext_inst.cpp
    third_party/spirv-tools/src/source/extensions.cpp
    third_party/spirv-tools/src/source/libspirv.cpp
    third_party/spirv-tools/src/source/name_mapper.cpp
    third_party/spirv-tools/src/source/opcode.cpp
    third_party/spirv-tools/src/source/operand.cpp
    third_party/spirv-tools/src/source/parsed_operand.cpp
    third_party/spirv-tools/src/source/print.cpp
    third_party/spirv-tools/src/source/spirv_endian.cpp
    third_party/spirv-tools/src/source/spirv_optimizer_options.cpp
    third_party/spirv-tools/src/source/spirv_target_env.cpp
    third_party/spirv-tools/src/source/spirv_validator_options.cpp
    third_party/spirv-tools/src/source/table.cpp
    third_party/spirv-tools/src/source/text.cpp
    third_party/spirv-tools/src/source/text_handler.cpp
    third_party/spirv-tools/src/source/util/bit_vector.cpp
    third_party/spirv-tools/src/source/util/parse_number.cpp
    third_party/spirv-tools/src/source/util/string_utils.cpp
    third_party/spirv-tools/src/source/util/timer.cpp
  )
source_group("spvtools"            FILES ${spvtools})
list (APPEND SOURCE_FILES ${spvtools})



set(spvtools_val
    third_party/spirv-tools/src/source/val/basic_block.cpp
    third_party/spirv-tools/src/source/val/construct.cpp
    third_party/spirv-tools/src/source/val/function.cpp
    third_party/spirv-tools/src/source/val/instruction.cpp
    third_party/spirv-tools/src/source/val/validate.cpp
    third_party/spirv-tools/src/source/val/validate_adjacency.cpp
    third_party/spirv-tools/src/source/val/validate_annotation.cpp
    third_party/spirv-tools/src/source/val/validate_arithmetics.cpp
    third_party/spirv-tools/src/source/val/validate_atomics.cpp
    third_party/spirv-tools/src/source/val/validate_barriers.cpp
    third_party/spirv-tools/src/source/val/validate_bitwise.cpp
    third_party/spirv-tools/src/source/val/validate_builtins.cpp
    third_party/spirv-tools/src/source/val/validate_capability.cpp
    third_party/spirv-tools/src/source/val/validate_cfg.cpp
    third_party/spirv-tools/src/source/val/validate_composites.cpp
    third_party/spirv-tools/src/source/val/validate_constants.cpp
    third_party/spirv-tools/src/source/val/validate_conversion.cpp
    third_party/spirv-tools/src/source/val/validate_debug.cpp
    third_party/spirv-tools/src/source/val/validate_decorations.cpp
    third_party/spirv-tools/src/source/val/validate_derivatives.cpp
    third_party/spirv-tools/src/source/val/validate_execution_limitations.cpp
    third_party/spirv-tools/src/source/val/validate_function.cpp
    third_party/spirv-tools/src/source/val/validate_id.cpp
    third_party/spirv-tools/src/source/val/validate_image.cpp
    third_party/spirv-tools/src/source/val/validate_instruction.cpp
    third_party/spirv-tools/src/source/val/validate_interfaces.cpp
    third_party/spirv-tools/src/source/val/validate_layout.cpp
    third_party/spirv-tools/src/source/val/validate_literals.cpp
    third_party/spirv-tools/src/source/val/validate_logicals.cpp
    third_party/spirv-tools/src/source/val/validate_memory.cpp
    third_party/spirv-tools/src/source/val/validate_mode_setting.cpp
    third_party/spirv-tools/src/source/val/validate_non_uniform.cpp
    third_party/spirv-tools/src/source/val/validate_primitives.cpp
    third_party/spirv-tools/src/source/val/validate_type.cpp
    third_party/spirv-tools/src/source/val/validation_state.cpp
  )
source_group("spvtools_val"            FILES ${spvtools_val})
list (APPEND SOURCE_FILES ${spvtools_val})



set(spvtools_opt
    third_party/spirv-tools/src/source/opt/aggressive_dead_code_elim_pass.cpp
    third_party/spirv-tools/src/source/opt/basic_block.cpp
    third_party/spirv-tools/src/source/opt/block_merge_pass.cpp
    third_party/spirv-tools/src/source/opt/build_module.cpp
    third_party/spirv-tools/src/source/opt/ccp_pass.cpp
    third_party/spirv-tools/src/source/opt/cfg.cpp
    third_party/spirv-tools/src/source/opt/cfg_cleanup_pass.cpp
    third_party/spirv-tools/src/source/opt/combine_access_chains.cpp
    third_party/spirv-tools/src/source/opt/compact_ids_pass.cpp
    third_party/spirv-tools/src/source/opt/composite.cpp
    third_party/spirv-tools/src/source/opt/const_folding_rules.cpp
    third_party/spirv-tools/src/source/opt/constants.cpp
    third_party/spirv-tools/src/source/opt/copy_prop_arrays.cpp
    third_party/spirv-tools/src/source/opt/dead_branch_elim_pass.cpp
    third_party/spirv-tools/src/source/opt/dead_insert_elim_pass.cpp
    third_party/spirv-tools/src/source/opt/dead_variable_elimination.cpp
    third_party/spirv-tools/src/source/opt/decoration_manager.cpp
    third_party/spirv-tools/src/source/opt/def_use_manager.cpp
    third_party/spirv-tools/src/source/opt/dominator_analysis.cpp
    third_party/spirv-tools/src/source/opt/dominator_tree.cpp
    third_party/spirv-tools/src/source/opt/eliminate_dead_constant_pass.cpp
    third_party/spirv-tools/src/source/opt/eliminate_dead_functions_pass.cpp
    third_party/spirv-tools/src/source/opt/feature_manager.cpp
    third_party/spirv-tools/src/source/opt/flatten_decoration_pass.cpp
    third_party/spirv-tools/src/source/opt/fold.cpp
    third_party/spirv-tools/src/source/opt/fold_spec_constant_op_and_composite_pass.cpp
    third_party/spirv-tools/src/source/opt/folding_rules.cpp
    third_party/spirv-tools/src/source/opt/freeze_spec_constant_value_pass.cpp
    third_party/spirv-tools/src/source/opt/function.cpp
    third_party/spirv-tools/src/source/opt/if_conversion.cpp
    third_party/spirv-tools/src/source/opt/inline_exhaustive_pass.cpp
    third_party/spirv-tools/src/source/opt/inline_opaque_pass.cpp
    third_party/spirv-tools/src/source/opt/inline_pass.cpp
    third_party/spirv-tools/src/source/opt/instruction.cpp
    third_party/spirv-tools/src/source/opt/instruction_list.cpp
    third_party/spirv-tools/src/source/opt/ir_context.cpp
    third_party/spirv-tools/src/source/opt/ir_loader.cpp
    third_party/spirv-tools/src/source/opt/licm_pass.cpp
    third_party/spirv-tools/src/source/opt/local_access_chain_convert_pass.cpp
    third_party/spirv-tools/src/source/opt/local_redundancy_elimination.cpp
    third_party/spirv-tools/src/source/opt/local_single_block_elim_pass.cpp
    third_party/spirv-tools/src/source/opt/local_single_store_elim_pass.cpp
    third_party/spirv-tools/src/source/opt/loop_dependence.cpp
    third_party/spirv-tools/src/source/opt/loop_dependence_helpers.cpp
    third_party/spirv-tools/src/source/opt/loop_descriptor.cpp
    third_party/spirv-tools/src/source/opt/loop_fission.cpp
    third_party/spirv-tools/src/source/opt/loop_fusion.cpp
    third_party/spirv-tools/src/source/opt/loop_fusion_pass.cpp
    third_party/spirv-tools/src/source/opt/loop_peeling.cpp
    third_party/spirv-tools/src/source/opt/loop_unroller.cpp
    third_party/spirv-tools/src/source/opt/loop_unswitch_pass.cpp
    third_party/spirv-tools/src/source/opt/loop_utils.cpp
    third_party/spirv-tools/src/source/opt/mem_pass.cpp
    third_party/spirv-tools/src/source/opt/merge_return_pass.cpp
    third_party/spirv-tools/src/source/opt/module.cpp
    third_party/spirv-tools/src/source/opt/optimizer.cpp
    third_party/spirv-tools/src/source/opt/pass.cpp
    third_party/spirv-tools/src/source/opt/pass_manager.cpp
    third_party/spirv-tools/src/source/opt/private_to_local_pass.cpp
    third_party/spirv-tools/src/source/opt/propagator.cpp
    third_party/spirv-tools/src/source/opt/reduce_load_size.cpp
    third_party/spirv-tools/src/source/opt/redundancy_elimination.cpp
    third_party/spirv-tools/src/source/opt/register_pressure.cpp
    third_party/spirv-tools/src/source/opt/remove_duplicates_pass.cpp
    third_party/spirv-tools/src/source/opt/replace_invalid_opc.cpp
    third_party/spirv-tools/src/source/opt/scalar_analysis.cpp
    third_party/spirv-tools/src/source/opt/scalar_analysis_simplification.cpp
    third_party/spirv-tools/src/source/opt/scalar_replacement_pass.cpp
    third_party/spirv-tools/src/source/opt/set_spec_constant_default_value_pass.cpp
    third_party/spirv-tools/src/source/opt/simplification_pass.cpp
    third_party/spirv-tools/src/source/opt/ssa_rewrite_pass.cpp
    third_party/spirv-tools/src/source/opt/strength_reduction_pass.cpp
    third_party/spirv-tools/src/source/opt/strip_debug_info_pass.cpp
    third_party/spirv-tools/src/source/opt/strip_reflect_info_pass.cpp
    third_party/spirv-tools/src/source/opt/struct_cfg_analysis.cpp
    third_party/spirv-tools/src/source/opt/type_manager.cpp
    third_party/spirv-tools/src/source/opt/types.cpp
    third_party/spirv-tools/src/source/opt/unify_const_pass.cpp
    third_party/spirv-tools/src/source/opt/value_number_table.cpp
    third_party/spirv-tools/src/source/opt/vector_dce.cpp
    third_party/spirv-tools/src/source/opt/workaround1209.cpp
  )

source_group("spvtools_opt"            FILES ${spvtools_opt})
list (APPEND SOURCE_FILES ${spvtools_opt})



  
  #D:\GAME-ENGINES\Urho3D\20181103\Source\ThirdParty\angle\out\Debug
set (libangle_includes
  include/EGL
  include/GLES/gl.h
  include/GLES
  include/GLES2
  include/GLES3
  include/GLSLANG
  include/KHR
  include/platform
  include
  src
  third_party/vulkan-headers/src/include
  src/common/third_party/base
  third_party/glslang/src
  third_party/spirv-tools/src
  third_party/spirv-tools/src/include
  third_party/spirv-headers/src/include
  third_party/vulkan-validation-layers/src/layers
  third_party/vulkan-loader/src/loader
  out/gen/angle
  out/gen/third_party
  out/gen/third_party/spirv-tools/src
)

# Define dependency libs
set (INCLUDE_DIRS ${libangle_includes})

# Setup target
setup_library ()

# Install headers for building and using the Urho3D library (install dependency for Urho3D/Script/APITemplates.h)
install_header_files (DIRECTORY include/ DESTINATION ${DEST_INCLUDE_DIR}/ThirdParty/angle FILES_MATCHING PATTERN *.h)     # Note: the trailing slash is significant
