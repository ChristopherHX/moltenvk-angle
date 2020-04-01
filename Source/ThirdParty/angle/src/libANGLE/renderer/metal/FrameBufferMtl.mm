//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// FramebufferMtl.mm:
//    Implements the class methods for FramebufferMtl.
//

#include "libANGLE/renderer/metal/ContextMtl.h"

#include <TargetConditionals.h>

#include "common/MemoryBuffer.h"
#include "common/angleutils.h"
#include "common/debug.h"
#include "libANGLE/renderer/metal/BufferMtl.h"
#include "libANGLE/renderer/metal/DisplayMtl.h"
#include "libANGLE/renderer/metal/FrameBufferMtl.h"
#include "libANGLE/renderer/metal/SurfaceMtl.h"
#include "libANGLE/renderer/metal/mtl_utils.h"
#include "libANGLE/renderer/renderer_utils.h"

namespace rx
{

namespace
{

const gl::InternalFormat &GetReadAttachmentInfo(const gl::Context *context,
                                                RenderTargetMtl *renderTarget)
{
    GLenum implFormat;

    if (renderTarget && renderTarget->getFormat())
    {
        implFormat = renderTarget->getFormat()->actualAngleFormat().fboImplementationInternalFormat;
    }
    else
    {
        implFormat = GL_NONE;
    }

    return gl::GetSizedInternalFormatInfo(implFormat);
}

}

// FramebufferMtl implementation
FramebufferMtl::FramebufferMtl(const gl::FramebufferState &state,
                               bool flipY,
                               SurfaceMtl *backbuffer)
    : FramebufferImpl(state), mBackbuffer(backbuffer), mFlipY(flipY)
{
    reset();
}

FramebufferMtl::~FramebufferMtl() {}

void FramebufferMtl::reset()
{
    for (auto &rt : mColorRenderTargets)
    {
        rt = nullptr;
    }
    mDepthRenderTarget = mStencilRenderTarget = nullptr;

    mRenderPassFirstColorAttachmentFormat = nullptr;
}

void FramebufferMtl::destroy(const gl::Context *context)
{
    reset();
}

angle::Result FramebufferMtl::discard(const gl::Context *context,
                                      size_t count,
                                      const GLenum *attachments)
{
    return invalidate(context, count, attachments);
}

angle::Result FramebufferMtl::invalidate(const gl::Context *context,
                                         size_t count,
                                         const GLenum *attachments)
{
    return invalidateImpl(mtl::GetImpl(context), count, attachments);
}

angle::Result FramebufferMtl::invalidateSub(const gl::Context *context,
                                            size_t count,
                                            const GLenum *attachments,
                                            const gl::Rectangle &area)
{
    if (area.encloses(getCompleteRenderArea()))
    {
        return invalidateImpl(mtl::GetImpl(context), count, attachments);
    }
    return angle::Result::Continue;
}

angle::Result FramebufferMtl::clear(const gl::Context *context, GLbitfield mask)
{
    ContextMtl *contextMtl = mtl::GetImpl(context);

    mtl::ClearRectParams clearOpts;

    bool clearColor   = IsMaskFlagSet(mask, static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT));
    bool clearDepth   = IsMaskFlagSet(mask, static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT));
    bool clearStencil = IsMaskFlagSet(mask, static_cast<GLbitfield>(GL_STENCIL_BUFFER_BIT));

    gl::DrawBufferMask clearColorBuffers;
    if (clearColor)
    {
        clearColorBuffers    = mState.getEnabledDrawBuffers();
        clearOpts.clearColor = contextMtl->getClearColorValue();
    }
    if (clearDepth)
    {
        clearOpts.clearDepth = contextMtl->getClearDepthValue();
    }
    if (clearStencil)
    {
        clearOpts.clearStencil = contextMtl->getClearStencilValue();
    }

    return clearImpl(context, clearColorBuffers, &clearOpts);
}

angle::Result FramebufferMtl::clearBufferfv(const gl::Context *context,
                                            GLenum buffer,
                                            GLint drawbuffer,
                                            const GLfloat *values)
{
    mtl::ClearRectParams clearOpts;

    gl::DrawBufferMask clearColorBuffers;
    if (buffer == GL_DEPTH)
    {
        clearOpts.clearDepth = values[0];
    }
    else
    {
        clearColorBuffers.set(drawbuffer);
        clearOpts.clearColor = {
            .type  = mtl::PixelType::Float,
            .red   = values[0],
            .green = values[1],
            .blue  = values[2],
            .alpha = values[3],
        };
    }

    return clearImpl(context, clearColorBuffers, &clearOpts);
}
angle::Result FramebufferMtl::clearBufferuiv(const gl::Context *context,
                                             GLenum buffer,
                                             GLint drawbuffer,
                                             const GLuint *values)
{
    gl::DrawBufferMask clearColorBuffers;
    clearColorBuffers.set(drawbuffer);

    mtl::ClearRectParams clearOpts;
    clearOpts.clearColor = {
        .type   = mtl::PixelType::UInt,
        .redU   = values[0],
        .greenU = values[1],
        .blueU  = values[2],
        .alphaU = values[3],
    };

    return clearImpl(context, clearColorBuffers, &clearOpts);
}
angle::Result FramebufferMtl::clearBufferiv(const gl::Context *context,
                                            GLenum buffer,
                                            GLint drawbuffer,
                                            const GLint *values)
{
    mtl::ClearRectParams clearOpts;

    gl::DrawBufferMask clearColorBuffers;
    if (buffer == GL_STENCIL)
    {
        clearOpts.clearStencil = gl::clamp(values[0], 0, static_cast<GLint>(mtl::kStencilMaskAll));
    }
    else
    {
        clearColorBuffers.set(drawbuffer);
        clearOpts.clearColor = {
            .type   = mtl::PixelType::Int,
            .redI   = values[0],
            .greenI = values[1],
            .blueI  = values[2],
            .alphaI = values[3],
        };
    }

    return clearImpl(context, clearColorBuffers, &clearOpts);
}
angle::Result FramebufferMtl::clearBufferfi(const gl::Context *context,
                                            GLenum buffer,
                                            GLint drawbuffer,
                                            GLfloat depth,
                                            GLint stencil)
{
    mtl::ClearRectParams clearOpts;
    clearOpts.clearDepth   = depth;
    clearOpts.clearStencil = gl::clamp(stencil, 0, static_cast<GLint>(mtl::kStencilMaskAll));

    return clearImpl(context, gl::DrawBufferMask(), &clearOpts);
}

GLenum FramebufferMtl::getImplementationColorReadFormat(const gl::Context *context) const
{
    return GetReadAttachmentInfo(context, getColorReadRenderTarget(context)).format;
}

GLenum FramebufferMtl::getImplementationColorReadType(const gl::Context *context) const
{
    GLenum readType = GetReadAttachmentInfo(context, getColorReadRenderTarget(context)).type;
    if (context->getClientMajorVersion() < 3 && readType == GL_HALF_FLOAT)
    {
        // GL_HALF_FLOAT was not introduced until GLES 3.0, and has a different value from
        // GL_HALF_FLOAT_OES
        readType = GL_HALF_FLOAT_OES;
    }
    return readType;
}

angle::Result FramebufferMtl::readPixels(const gl::Context *context,
                                         const gl::Rectangle &area,
                                         GLenum format,
                                         GLenum type,
                                         void *pixels)
{
    // Clip read area to framebuffer.
    const gl::Extents &fbSize = getState().getReadAttachment()->getSize();
    const gl::Rectangle fbRect(0, 0, fbSize.width, fbSize.height);

    gl::Rectangle clippedArea;
    if (!ClipRectangle(area, fbRect, &clippedArea))
    {
        // nothing to read
        return angle::Result::Continue;
    }
    gl::Rectangle flippedArea = getReadArea(context, clippedArea);

    ContextMtl *contextMtl              = mtl::GetImpl(context);
    const gl::State &glState            = context->getState();
    const gl::PixelPackState &packState = glState.getPackState();

    const gl::InternalFormat &sizedFormatInfo = gl::GetInternalFormatInfo(format, type);

    GLuint outputPitch = 0;
    ANGLE_CHECK_GL_MATH(contextMtl,
                        sizedFormatInfo.computeRowPitch(type, area.width, packState.alignment,
                                                        packState.rowLength, &outputPitch));
    GLuint outputSkipBytes = 0;
    ANGLE_CHECK_GL_MATH(contextMtl, sizedFormatInfo.computeSkipBytes(
                                        type, outputPitch, 0, packState, false, &outputSkipBytes));

    outputSkipBytes += (clippedArea.x - area.x) * sizedFormatInfo.pixelBytes +
                       (clippedArea.y - area.y) * outputPitch;

    const angle::Format &angleFormat = GetFormatFromFormatType(format, type);

    PackPixelsParams params(flippedArea, angleFormat, outputPitch, packState.reverseRowOrder,
                            glState.getTargetBuffer(gl::BufferBinding::PixelPack), 0);
    if (mFlipY)
    {
        params.reverseRowOrder = !params.reverseRowOrder;
    }

    ANGLE_TRY(readPixelsImpl(context, flippedArea, params, getColorReadRenderTarget(context),
                             static_cast<uint8_t *>(pixels) + outputSkipBytes));

    return angle::Result::Continue;
}

angle::Result FramebufferMtl::blit(const gl::Context *context,
                                   const gl::Rectangle &sourceAreaIn,
                                   const gl::Rectangle &destAreaIn,
                                   GLbitfield mask,
                                   GLenum filter)
{
    // NOTE(hqle): Support MSAA feature.
    ContextMtl *contextMtl = mtl::GetImpl(context);

    bool blitColorBuffer   = (mask & GL_COLOR_BUFFER_BIT) != 0;
    bool blitDepthBuffer   = (mask & GL_DEPTH_BUFFER_BIT) != 0;
    bool blitStencilBuffer = (mask & GL_STENCIL_BUFFER_BIT) != 0;

    const gl::State &glState              = context->getState();
    const gl::Framebuffer *srcFramebuffer = glState.getReadFramebuffer();

    FramebufferMtl *srcFrameBuffer = mtl::GetImpl(srcFramebuffer);

    blitColorBuffer =
        blitColorBuffer && srcFrameBuffer->getColorReadRenderTarget(context) != nullptr;
    blitDepthBuffer   = blitDepthBuffer && srcFrameBuffer->getDepthRenderTarget() != nullptr;
    blitStencilBuffer = blitStencilBuffer && srcFrameBuffer->getStencilRenderTarget() != nullptr;

    if (!blitColorBuffer && !blitDepthBuffer && !blitStencilBuffer)
    {
        // No-op
        return angle::Result::Continue;
    }

    gl::Rectangle sourceArea = sourceAreaIn;
    gl::Rectangle destArea   = destAreaIn;

    const gl::Rectangle srcFramebufferDimensions = srcFrameBuffer->getCompleteRenderArea();

    // If the destination is flipped in either direction, we will flip the source instead so that
    // the destination area is always unflipped.
    sourceArea = sourceArea.flip(destArea.isReversedX(), destArea.isReversedY());
    destArea   = destArea.removeReversal();

    // Calculate the stretch factor prior to any clipping, as it needs to remain constant.
    const float stretch[2] = {
        std::abs(sourceArea.width / static_cast<float>(destArea.width)),
        std::abs(sourceArea.height / static_cast<float>(destArea.height)),
    };

    // First, clip the source area to framebuffer.  That requires transforming the dest area to
    // match the clipped source.
    gl::Rectangle absSourceArea = sourceArea.removeReversal();
    gl::Rectangle clippedSourceArea;
    if (!gl::ClipRectangle(srcFramebufferDimensions, absSourceArea, &clippedSourceArea))
    {
        return angle::Result::Continue;
    }

    // Resize the destination area based on the new size of source.  Note again that stretch is
    // calculated as SrcDimension/DestDimension.
    gl::Rectangle srcClippedDestArea;
    if (clippedSourceArea == absSourceArea)
    {
        // If there was no clipping, keep dest area as is.
        srcClippedDestArea = destArea;
    }
    else
    {
        // Shift dest area's x0,y0,x1,y1 by as much as the source area's got shifted (taking
        // stretching into account)
        float x0Shift = std::round((clippedSourceArea.x - absSourceArea.x) / stretch[0]);
        float y0Shift = std::round((clippedSourceArea.y - absSourceArea.y) / stretch[1]);
        float x1Shift = std::round((absSourceArea.x1() - clippedSourceArea.x1()) / stretch[0]);
        float y1Shift = std::round((absSourceArea.y1() - clippedSourceArea.y1()) / stretch[1]);

        // If the source area was reversed in any direction, the shift should be applied in the
        // opposite direction as well.
        if (sourceArea.isReversedX())
        {
            std::swap(x0Shift, x1Shift);
        }

        if (sourceArea.isReversedY())
        {
            std::swap(y0Shift, y1Shift);
        }

        srcClippedDestArea.x = destArea.x0() + static_cast<int>(x0Shift);
        srcClippedDestArea.y = destArea.y0() + static_cast<int>(y0Shift);
        int x1               = destArea.x1() - static_cast<int>(x1Shift);
        int y1               = destArea.y1() - static_cast<int>(y1Shift);

        srcClippedDestArea.width  = x1 - srcClippedDestArea.x;
        srcClippedDestArea.height = y1 - srcClippedDestArea.y;
    }

    const bool unpackFlipX = sourceArea.isReversedX();
    const bool unpackFlipY = sourceArea.isReversedY();

    ASSERT(!destArea.isReversedX() && !destArea.isReversedY());

    // Clip the destination area to the framebuffer size and scissor.
    gl::Rectangle scissoredDestArea;
    if (!gl::ClipRectangle(ClipRectToScissor(glState, this->getCompleteRenderArea(), false),
                           srcClippedDestArea, &scissoredDestArea))
    {
        return angle::Result::Continue;
    }

    // NOTE(hqle): Consider splitting this function.
    // Use blit with draw
    mtl::RenderCommandEncoder *renderEncoder = nullptr;

    mtl::BlitParams baseParams;
    baseParams.dstTextureSize = mState.getExtents();
    baseParams.dstRect        = srcClippedDestArea;
    baseParams.dstScissorRect = scissoredDestArea;
    baseParams.dstFlipY       = this->flipY();
    baseParams.srcYFlipped    = srcFrameBuffer->flipY();
    baseParams.unpackFlipX    = unpackFlipX;
    baseParams.unpackFlipY    = unpackFlipY;

    // Depth & stencil are special cases. Need to copy to intermediate texture that is readable
    // in shader. The copy must be done before render pass starts.
    if (blitDepthBuffer || blitStencilBuffer)
    {
        mtl::DepthStencilBlitParams dsBlitParams;
        memcpy(&dsBlitParams, &baseParams, sizeof(baseParams));
        RenderTargetMtl *depthRt   = srcFrameBuffer->getDepthRenderTarget();
        RenderTargetMtl *stencilRt = srcFrameBuffer->getStencilRenderTarget();

        bool sameTexture = depthRt == stencilRt;
        if (blitDepthBuffer)
        {
            ANGLE_TRY(getReadableViewForRenderTarget(context, *depthRt, clippedSourceArea, false,
                                                     /** readableView */ &dsBlitParams.src,
                                                     &dsBlitParams.srcLevel, &dsBlitParams.srcLayer,
                                                     &dsBlitParams.srcRect));
            if (sameTexture && blitStencilBuffer)
            {
                // If texture is packed depth stencil, we can skip the stencil view copying step.
                dsBlitParams.srcStencil      = dsBlitParams.src->getStencilView();
                dsBlitParams.srcStencilLevel = dsBlitParams.srcLevel;
                dsBlitParams.srcStencilLayer = dsBlitParams.srcLayer;
            }
        }

        if (blitStencilBuffer && !dsBlitParams.srcStencil)
        {
            ANGLE_TRY(getReadableViewForRenderTarget(
                context, *stencilRt, clippedSourceArea, true,
                /** readableView */ &dsBlitParams.srcStencil, &dsBlitParams.srcStencilLevel,
                &dsBlitParams.srcStencilLayer, &dsBlitParams.srcRect));
        }

        renderEncoder = ensureRenderPassStarted(context);
        ANGLE_TRY(contextMtl->getDisplay()->getUtils().blitDepthStencilWithDraw(
            context, renderEncoder, dsBlitParams));
    }
    else
    {
        renderEncoder = ensureRenderPassStarted(context);
    }

    // Blit color
    if (blitColorBuffer)
    {
        mtl::ColorBlitParams colorBlitParams;
        memcpy(&colorBlitParams, &baseParams, sizeof(baseParams));

        RenderTargetMtl *srcColorRt = srcFrameBuffer->getColorReadRenderTarget(context);
        ASSERT(srcColorRt);

        colorBlitParams.src      = srcColorRt->getTexture();
        colorBlitParams.srcLevel = srcColorRt->getLevelIndex();
        colorBlitParams.srcLayer = srcColorRt->getLayerIndex();
        colorBlitParams.srcRect  = clippedSourceArea;

        colorBlitParams.blitColorMask  = contextMtl->getColorMask();
        colorBlitParams.enabledBuffers = getState().getEnabledDrawBuffers();
        colorBlitParams.filter         = filter;
        colorBlitParams.dstLuminance   = srcColorRt->getFormat()->actualAngleFormat().isLUMA();

        ANGLE_TRY(contextMtl->getDisplay()->getUtils().blitColorWithDraw(
            context, renderEncoder, srcColorRt->getFormat()->actualAngleFormat(), colorBlitParams));
    }

    return angle::Result::Continue;
}

bool FramebufferMtl::checkStatus(const gl::Context *context) const
{
    if (!mState.attachmentsHaveSameDimensions())
    {
        return false;
    }

    ContextMtl *contextMtl = mtl::GetImpl(context);
    if (!contextMtl->getDisplay()->getFeatures().allowSeparatedDepthStencilBuffers.enabled &&
        mState.hasSeparateDepthAndStencilAttachments())
    {
        return false;
    }

    return true;
}

angle::Result FramebufferMtl::syncState(const gl::Context *context,
                                        const gl::Framebuffer::DirtyBits &dirtyBits)
{
    ContextMtl *contextMtl = mtl::GetImpl(context);
    ASSERT(dirtyBits.any());
    bool mustNotifyContext = false;
    for (size_t dirtyBit : dirtyBits)
    {
        switch (dirtyBit)
        {
            case gl::Framebuffer::DIRTY_BIT_DEPTH_ATTACHMENT:
                ANGLE_TRY(updateDepthRenderTarget(context));
                break;
            case gl::Framebuffer::DIRTY_BIT_STENCIL_ATTACHMENT:
                ANGLE_TRY(updateStencilRenderTarget(context));
                break;
            case gl::Framebuffer::DIRTY_BIT_DEPTH_BUFFER_CONTENTS:
            case gl::Framebuffer::DIRTY_BIT_STENCIL_BUFFER_CONTENTS:
                // NOTE(hqle): What are we supposed to do?
                break;
            case gl::Framebuffer::DIRTY_BIT_DRAW_BUFFERS:
                mustNotifyContext = true;
                break;
            case gl::Framebuffer::DIRTY_BIT_READ_BUFFER:
            case gl::Framebuffer::DIRTY_BIT_DEFAULT_WIDTH:
            case gl::Framebuffer::DIRTY_BIT_DEFAULT_HEIGHT:
            case gl::Framebuffer::DIRTY_BIT_DEFAULT_SAMPLES:
            case gl::Framebuffer::DIRTY_BIT_DEFAULT_FIXED_SAMPLE_LOCATIONS:
                break;
            default:
            {
                static_assert(gl::Framebuffer::DIRTY_BIT_COLOR_ATTACHMENT_0 == 0, "FB dirty bits");
                if (dirtyBit < gl::Framebuffer::DIRTY_BIT_COLOR_ATTACHMENT_MAX)
                {
                    size_t colorIndexGL = static_cast<size_t>(
                        dirtyBit - gl::Framebuffer::DIRTY_BIT_COLOR_ATTACHMENT_0);
                    ANGLE_TRY(updateColorRenderTarget(context, colorIndexGL));
                }
                else
                {
                    ASSERT(dirtyBit >= gl::Framebuffer::DIRTY_BIT_COLOR_BUFFER_CONTENTS_0 &&
                           dirtyBit < gl::Framebuffer::DIRTY_BIT_COLOR_BUFFER_CONTENTS_MAX);
                    // NOTE: might need to notify context.
                }
                break;
            }
        }
    }

    auto oldRenderPassDesc = mRenderPassDesc;

    ANGLE_TRY(prepareRenderPass(context, &mRenderPassDesc));
    bool renderPassChanged = !oldRenderPassDesc.equalIgnoreLoadStoreOptions(mRenderPassDesc);

    if (mustNotifyContext || renderPassChanged)
    {
        FramebufferMtl *currentDrawFramebuffer =
            mtl::GetImpl(context->getState().getDrawFramebuffer());
        if (currentDrawFramebuffer == this)
        {
            contextMtl->onDrawFrameBufferChangedState(context, this, renderPassChanged);
        }
    }

    return angle::Result::Continue;
}

angle::Result FramebufferMtl::getSamplePosition(const gl::Context *context,
                                                size_t index,
                                                GLfloat *xy) const
{
    UNIMPLEMENTED();
    return angle::Result::Stop;
}

RenderTargetMtl *FramebufferMtl::getColorReadRenderTarget(const gl::Context *context) const
{
    if (mState.getReadIndex() >= mColorRenderTargets.size())
    {
        return nullptr;
    }

    if (mBackbuffer)
    {
        if (IsError(mBackbuffer->ensureCurrentDrawableObtained(context)))
        {
            return nullptr;
        }
    }

    return mColorRenderTargets[mState.getReadIndex()];
}

int FramebufferMtl::getSamples() const
{
    return mRenderPassDesc.sampleCount;
}

gl::Rectangle FramebufferMtl::getCompleteRenderArea() const
{
    return gl::Rectangle(0, 0, mState.getDimensions().width, mState.getDimensions().height);
}

bool FramebufferMtl::renderPassHasStarted(ContextMtl *contextMtl) const
{
    return contextMtl->hasStartedRenderPass(mRenderPassDesc);
}

mtl::RenderCommandEncoder *FramebufferMtl::ensureRenderPassStarted(const gl::Context *context)
{
    return ensureRenderPassStarted(context, mRenderPassDesc);
}

mtl::RenderCommandEncoder *FramebufferMtl::ensureRenderPassStarted(const gl::Context *context,
                                                                   const mtl::RenderPassDesc &desc)
{
    ContextMtl *contextMtl = mtl::GetImpl(context);

    if (renderPassHasStarted(contextMtl))
    {
        return contextMtl->getRenderCommandEncoder();
    }

    if (mBackbuffer)
    {
        // Backbuffer might obtain new drawable, which means it might change the
        // the native texture used as the target of the render pass.
        // We need to call this before creating render encoder.
        if (IsError(mBackbuffer->ensureCurrentDrawableObtained(context)))
        {
            return nullptr;
        }
    }

    // Only support ensureRenderPassStarted() with different load & store options.
    // The texture, level, slice must be the same.
    ASSERT(desc.equalIgnoreLoadStoreOptions(mRenderPassDesc));

    mtl::RenderCommandEncoder *encoder = contextMtl->getRenderCommandEncoder(desc);

    if (mRenderPassCleanStart)
    {
        // After a clean start we should reset the loadOp to MTLLoadActionLoad.
        mRenderPassCleanStart = false;
        for (mtl::RenderPassColorAttachmentDesc &colorAttachment : mRenderPassDesc.colorAttachments)
        {
            colorAttachment.loadAction = MTLLoadActionLoad;
        }
        mRenderPassDesc.depthAttachment.loadAction   = MTLLoadActionLoad;
        mRenderPassDesc.stencilAttachment.loadAction = MTLLoadActionLoad;
    }

    return encoder;
}

void FramebufferMtl::initLoadStoreActionOnRenderPassFirstStart(
    mtl::RenderPassAttachmentDesc *attachmentOut)
{
    ASSERT(mRenderPassCleanStart);

    mtl::RenderPassAttachmentDesc &attachment = *attachmentOut;

    if (attachment.storeAction == MTLStoreActionDontCare ||
        attachment.storeAction == MTLStoreActionMultisampleResolve)
    {
        // If we previously discarded attachment's content, then don't need to load it.
        attachment.loadAction = MTLLoadActionDontCare;
    }
    else
    {
        attachment.loadAction = MTLLoadActionLoad;
    }

    if (attachment.hasImplicitMSTexture())
    {
        if (mBackbuffer)
        {
            // Default action for default framebuffer is resolve and keep MS texture's content.
            // We only discard MS texture's content at the end of the frame. See onFrameEnd().
            attachment.storeAction = MTLStoreActionStoreAndMultisampleResolve;
        }
        else
        {
            // Default action is resolve but don't keep MS texture's content.
            attachment.storeAction = MTLStoreActionMultisampleResolve;
        }
    }
    else
    {
        attachment.storeAction = MTLStoreActionStore;  // Default action is store
    }
}

void FramebufferMtl::onStartedDrawingToFrameBuffer(const gl::Context *context)
{
    mRenderPassCleanStart = true;

    // Compute loadOp based on previous storeOp and reset storeOp flags:
    for (mtl::RenderPassColorAttachmentDesc &colorAttachment : mRenderPassDesc.colorAttachments)
    {
        initLoadStoreActionOnRenderPassFirstStart(&colorAttachment);
    }
    // Depth load/store
    initLoadStoreActionOnRenderPassFirstStart(&mRenderPassDesc.depthAttachment);

    // Stencil load/store
    initLoadStoreActionOnRenderPassFirstStart(&mRenderPassDesc.stencilAttachment);
}

void FramebufferMtl::onFrameEnd(const gl::Context *context)
{
    if (!mBackbuffer)
    {
        return;
    }

    ContextMtl *contextMtl = mtl::GetImpl(context);
    // Always discard default FBO's depth stencil & multisample buffers at the end of the frame:
    if (this->renderPassHasStarted(contextMtl))
    {
        mtl::RenderCommandEncoder *encoder = contextMtl->getRenderCommandEncoder();

        constexpr GLenum dsAttachments[] = {GL_DEPTH, GL_STENCIL};
        (void)invalidateImpl(contextMtl, 2, dsAttachments);
        if (mBackbuffer->getSamples() > 1)
        {
            encoder->setColorStoreAction(MTLStoreActionMultisampleResolve, 0);
        }

        contextMtl->endEncoding(false);

        // Reset discard flag.
        onStartedDrawingToFrameBuffer(context);
    }
}

angle::Result FramebufferMtl::updateColorRenderTarget(const gl::Context *context,
                                                      size_t colorIndexGL)
{
    ASSERT(colorIndexGL < mtl::kMaxRenderTargets);
    // Reset load store action
    mRenderPassDesc.colorAttachments[colorIndexGL].reset();
    return updateCachedRenderTarget(context, mState.getColorAttachment(colorIndexGL),
                                    &mColorRenderTargets[colorIndexGL]);
}

angle::Result FramebufferMtl::updateDepthRenderTarget(const gl::Context *context)
{
    // Reset load store action
    mRenderPassDesc.depthAttachment.reset();
    return updateCachedRenderTarget(context, mState.getDepthAttachment(), &mDepthRenderTarget);
}

angle::Result FramebufferMtl::updateStencilRenderTarget(const gl::Context *context)
{
    // Reset load store action
    mRenderPassDesc.stencilAttachment.reset();
    return updateCachedRenderTarget(context, mState.getStencilAttachment(), &mStencilRenderTarget);
}

angle::Result FramebufferMtl::updateCachedRenderTarget(const gl::Context *context,
                                                       const gl::FramebufferAttachment *attachment,
                                                       RenderTargetMtl **cachedRenderTarget)
{
    RenderTargetMtl *newRenderTarget = nullptr;
    if (attachment)
    {
        ASSERT(attachment->isAttached());
        ANGLE_TRY(attachment->getRenderTarget(context, attachment->getRenderToTextureSamples(),
                                              &newRenderTarget));
    }
    *cachedRenderTarget = newRenderTarget;
    return angle::Result::Continue;
}

angle::Result FramebufferMtl::getReadableViewForRenderTarget(const gl::Context *context,
                                                             const RenderTargetMtl &rtt,
                                                             const gl::Rectangle &readArea,
                                                             bool readStencil,
                                                             mtl::TextureRef *readableView,
                                                             uint32_t *readableViewLevel,
                                                             uint32_t *readableViewLayer,
                                                             gl::Rectangle *readableViewArea)
{
    ContextMtl *contextMtl     = mtl::GetImpl(context);
    mtl::TextureRef srcTexture = rtt.getTexture();
    uint32_t level             = rtt.getLevelIndex();
    uint32_t slice             = rtt.getLayerIndex();

    // NOTE(hqle): slice is not used atm.
    ASSERT(slice == 0);

    if (!srcTexture)
    {
        *readableView     = nullptr;
        *readableViewArea = readArea;
        return angle::Result::Continue;
    }

    bool skipCopy = srcTexture->isShaderReadable();
    if (rtt.getFormat()->hasDepthAndStencilBits() && readStencil)
    {
        // If the texture is packed depth stencil, and we need stencil view,
        // then it must support creating different format view.
        skipCopy = skipCopy && srcTexture->supportFormatView();
    }

    if (skipCopy)
    {
        // Texture is shader readable, just use it directly
        *readableView      = srcTexture;
        *readableViewLevel = level;
        *readableViewLayer = slice;
        *readableViewArea  = readArea;
    }
    else
    {
        ASSERT(srcTexture->textureType() != MTLTextureType3D);

        // Texture is not shader readable, copy to a interminate texture that is readable
        *readableView = srcTexture->getReadableCopy(
            contextMtl, contextMtl->getBlitCommandEncoder(), level, slice,
            MTLRegionMake2D(readArea.x, readArea.y, readArea.width, readArea.height));

        ANGLE_CHECK_GL_ALLOC(contextMtl, *readableView);

        if (readStencil)
        {
            *readableView = (*readableView)->getStencilView();
        }

        *readableViewLevel = 0;
        *readableViewLayer = 0;
        *readableViewArea  = gl::Rectangle(0, 0, readArea.width, readArea.height);
    }

    return angle::Result::Continue;
}

angle::Result FramebufferMtl::prepareRenderPass(const gl::Context *context,
                                                mtl::RenderPassDesc *pDescOut)
{
    auto &desc = *pDescOut;

    mRenderPassFirstColorAttachmentFormat = nullptr;
    mRenderPassAttachmentsSameColorType   = true;
    uint32_t maxColorAttachments = static_cast<uint32_t>(mState.getColorAttachments().size());
    desc.numColorAttachments     = 0;
    for (uint32_t colorIndexGL = 0; colorIndexGL < maxColorAttachments; ++colorIndexGL)
    {
        ASSERT(colorIndexGL < mtl::kMaxRenderTargets);

        mtl::RenderPassColorAttachmentDesc &colorAttachment = desc.colorAttachments[colorIndexGL];
        const RenderTargetMtl *colorRenderTarget            = mColorRenderTargets[colorIndexGL];
        if (colorRenderTarget)
        {
            colorRenderTarget->toRenderPassAttachmentDesc(&colorAttachment);
            desc.sampleCount = std::max(desc.sampleCount, colorRenderTarget->getRenderSamples());

            desc.numColorAttachments = std::max(desc.numColorAttachments, colorIndexGL + 1);

            if (!mRenderPassFirstColorAttachmentFormat)
            {
                mRenderPassFirstColorAttachmentFormat = colorRenderTarget->getFormat();
            }
            else if (colorRenderTarget->getFormat())
            {
                if (mRenderPassFirstColorAttachmentFormat->actualAngleFormat().isSint() !=
                        colorRenderTarget->getFormat()->actualAngleFormat().isSint() ||
                    mRenderPassFirstColorAttachmentFormat->actualAngleFormat().isUint() !=
                        colorRenderTarget->getFormat()->actualAngleFormat().isUint())
                {
                    mRenderPassAttachmentsSameColorType = false;
                }
            }
        }
        else
        {
            colorAttachment.reset();
        }
    }

    if (mDepthRenderTarget)
    {
        mDepthRenderTarget->toRenderPassAttachmentDesc(&desc.depthAttachment);
        desc.sampleCount = std::max(desc.sampleCount, mDepthRenderTarget->getRenderSamples());
    }

    if (mStencilRenderTarget)
    {
        mStencilRenderTarget->toRenderPassAttachmentDesc(&desc.stencilAttachment);
        desc.sampleCount = std::max(desc.sampleCount, mStencilRenderTarget->getRenderSamples());
    }

    return angle::Result::Continue;
}

// Override clear color based on texture's write mask
void FramebufferMtl::overrideClearColor(const mtl::TextureRef &texture,
                                        MTLClearColor clearColor,
                                        MTLClearColor *colorOut)
{
    *colorOut = mtl::EmulatedAlphaClearColor(clearColor, texture->getColorWritableMask());
}

angle::Result FramebufferMtl::clearWithLoadOp(const gl::Context *context,
                                              gl::DrawBufferMask clearColorBuffers,
                                              const mtl::ClearRectParams &clearOpts)
{
    ContextMtl *contextMtl             = mtl::GetImpl(context);
    bool startedRenderPass             = renderPassHasStarted(contextMtl);
    mtl::RenderCommandEncoder *encoder = nullptr;

    if (startedRenderPass)
    {
        encoder = ensureRenderPassStarted(context);
        if (encoder->hasDrawCalls())
        {
            // Render pass already has draw calls recorded, it is better to use clear with draw
            // operation.
            return clearWithDraw(context, clearColorBuffers, clearOpts);
        }
        else
        {
            return clearWithLoadOpRenderPassStarted(context, clearColorBuffers, clearOpts, encoder);
        }
    }
    else
    {
        return clearWithLoadOpRenderPassNotStarted(context, clearColorBuffers, clearOpts);
    }
}

angle::Result FramebufferMtl::clearWithLoadOpRenderPassNotStarted(
    const gl::Context *context,
    gl::DrawBufferMask clearColorBuffers,
    const mtl::ClearRectParams &clearOpts)
{
    mtl::RenderPassDesc tempDesc = mRenderPassDesc;

    for (uint32_t colorIndexGL = 0; colorIndexGL < tempDesc.numColorAttachments; ++colorIndexGL)
    {
        ASSERT(colorIndexGL < mtl::kMaxRenderTargets);

        mtl::RenderPassColorAttachmentDesc &colorAttachment =
            tempDesc.colorAttachments[colorIndexGL];
        const mtl::TextureRef &texture = colorAttachment.texture();

        if (clearColorBuffers.test(colorIndexGL))
        {
            colorAttachment.loadAction = MTLLoadActionClear;
            overrideClearColor(texture, clearOpts.clearColor.value(), &colorAttachment.clearColor);
        }
    }

    if (clearOpts.clearDepth.valid())
    {
        tempDesc.depthAttachment.loadAction = MTLLoadActionClear;
        tempDesc.depthAttachment.clearDepth = clearOpts.clearDepth.value();
    }

    if (clearOpts.clearStencil.valid())
    {
        tempDesc.stencilAttachment.loadAction   = MTLLoadActionClear;
        tempDesc.stencilAttachment.clearStencil = clearOpts.clearStencil.value();
    }

    // Start new render encoder with loadOp=Clear
    ensureRenderPassStarted(context, tempDesc);

    return angle::Result::Continue;
}

angle::Result FramebufferMtl::clearWithLoadOpRenderPassStarted(
    const gl::Context *context,
    gl::DrawBufferMask clearColorBuffers,
    const mtl::ClearRectParams &clearOpts,
    mtl::RenderCommandEncoder *encoder)
{
    ASSERT(!encoder->hasDrawCalls());

    for (uint32_t colorIndexGL = 0; colorIndexGL < mRenderPassDesc.numColorAttachments;
         ++colorIndexGL)
    {
        ASSERT(colorIndexGL < mtl::kMaxRenderTargets);

        mtl::RenderPassColorAttachmentDesc &colorAttachment =
            mRenderPassDesc.colorAttachments[colorIndexGL];
        const mtl::TextureRef &texture = colorAttachment.texture();

        if (clearColorBuffers.test(colorIndexGL))
        {
            MTLClearColor clearVal;
            overrideClearColor(texture, clearOpts.clearColor.value(), &clearVal);

            encoder->setColorLoadAction(MTLLoadActionClear, clearVal, colorIndexGL);
        }
    }

    if (clearOpts.clearDepth.valid())
    {
        encoder->setDepthLoadAction(MTLLoadActionClear, clearOpts.clearDepth.value());
    }

    if (clearOpts.clearStencil.valid())
    {
        encoder->setStencilLoadAction(MTLLoadActionClear, clearOpts.clearStencil.value());
    }

    return angle::Result::Continue;
}

angle::Result FramebufferMtl::clearWithDraw(const gl::Context *context,
                                            gl::DrawBufferMask clearColorBuffers,
                                            const mtl::ClearRectParams &clearOpts)
{
    ContextMtl *contextMtl = mtl::GetImpl(context);
    DisplayMtl *display    = contextMtl->getDisplay();

    if (mRenderPassAttachmentsSameColorType || !clearColorBuffers.any() ||
        !clearOpts.clearColor.valid())
    {
        // Start new render encoder if not already.
        mtl::RenderCommandEncoder *encoder = ensureRenderPassStarted(context, mRenderPassDesc);

        return display->getUtils().clearWithDraw(context, encoder, clearOpts);
    }
    else
    {
        // Not all attachments have the same color type.
        // Clear the attachment one by one.
        mtl::ClearRectParams overrideClearOps = clearOpts;
        overrideClearOps.enabledBuffers.reset();
        overrideClearOps.enabledBuffers.set(0);
        for (size_t drawbuffer : clearColorBuffers)
        {
            if (drawbuffer >= mRenderPassDesc.numColorAttachments)
            {
                continue;
            }
            RenderTargetMtl *renderTarget = mColorRenderTargets[drawbuffer];
            if (!renderTarget || !renderTarget->getTexture())
            {
                continue;
            }
            const mtl::Format &format     = *renderTarget->getFormat();
            mtl::PixelType clearColorType = overrideClearOps.clearColor.value().type;
            if ((clearColorType == mtl::PixelType::Int && !format.actualAngleFormat().isSint()) ||
                (clearColorType == mtl::PixelType::UInt && !format.actualAngleFormat().isUint()) ||
                (clearColorType == mtl::PixelType::Float && format.actualAngleFormat().isInt()))
            {
                continue;
            }

            mtl::RenderCommandEncoder *encoder = contextMtl->getRenderCommandEncoder(*renderTarget);
            ANGLE_TRY(display->getUtils().clearWithDraw(context, encoder, overrideClearOps));
        }

        return angle::Result::Continue;
    }
}

angle::Result FramebufferMtl::clearImpl(const gl::Context *context,
                                        gl::DrawBufferMask clearColorBuffers,
                                        mtl::ClearRectParams *pClearOpts)
{
    auto &clearOpts = *pClearOpts;

    if (!clearOpts.clearColor.valid() && !clearOpts.clearDepth.valid() &&
        !clearOpts.clearStencil.valid())
    {
        // No Op.
        return angle::Result::Continue;
    }

    ContextMtl *contextMtl = mtl::GetImpl(context);
    const gl::Rectangle renderArea(0, 0, mState.getDimensions().width,
                                   mState.getDimensions().height);

    clearOpts.colorFormat    = mRenderPassFirstColorAttachmentFormat;
    clearOpts.dstTextureSize = mState.getExtents();
    clearOpts.clearArea      = ClipRectToScissor(contextMtl->getState(), renderArea, false);
    clearOpts.flipY          = mFlipY;

    // Discard clear altogether if scissor has 0 width or height.
    if (clearOpts.clearArea.width == 0 || clearOpts.clearArea.height == 0)
    {
        return angle::Result::Continue;
    }

    MTLColorWriteMask colorMask = contextMtl->getColorMask();
    uint32_t stencilMask        = contextMtl->getStencilMask();
    if (!contextMtl->getDepthMask())
    {
        // Disable depth clearing, since depth write is disable
        clearOpts.clearDepth.reset();
    }

    // Only clear enabled buffers
    clearOpts.enabledBuffers = clearColorBuffers;

    if (clearOpts.clearArea == renderArea &&
        (!clearOpts.clearColor.valid() || colorMask == MTLColorWriteMaskAll) &&
        (!clearOpts.clearStencil.valid() ||
         (stencilMask & mtl::kStencilMaskAll) == mtl::kStencilMaskAll))
    {
        return clearWithLoadOp(context, clearColorBuffers, clearOpts);
    }

    return clearWithDraw(context, clearColorBuffers, clearOpts);
}

angle::Result FramebufferMtl::invalidateImpl(ContextMtl *contextMtl,
                                             size_t count,
                                             const GLenum *attachments)
{
    gl::DrawBufferMask invalidateColorBuffers;
    bool invalidateDepthBuffer   = false;
    bool invalidateStencilBuffer = false;

    for (size_t i = 0; i < count; ++i)
    {
        const GLenum attachment = attachments[i];

        switch (attachment)
        {
            case GL_DEPTH:
            case GL_DEPTH_ATTACHMENT:
                invalidateDepthBuffer = true;
                break;
            case GL_STENCIL:
            case GL_STENCIL_ATTACHMENT:
                invalidateStencilBuffer = true;
                break;
            case GL_DEPTH_STENCIL_ATTACHMENT:
                invalidateDepthBuffer   = true;
                invalidateStencilBuffer = true;
                break;
            default:
                ASSERT(
                    (attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT15) ||
                    (attachment == GL_COLOR));

                invalidateColorBuffers.set(
                    attachment == GL_COLOR ? 0u : (attachment - GL_COLOR_ATTACHMENT0));
        }
    }

    // Set the appropriate storeOp for attachments.
    // If we already start the render pass, then need to set the store action now.
    bool renderPassStarted = contextMtl->hasStartedRenderPass(mRenderPassDesc);
    mtl::RenderCommandEncoder *encoder =
        renderPassStarted ? contextMtl->getRenderCommandEncoder() : nullptr;

    for (uint32_t i = 0; i < mRenderPassDesc.numColorAttachments; ++i)
    {
        if (invalidateColorBuffers.test(i))
        {
            mtl::RenderPassColorAttachmentDesc &colorAttachment =
                mRenderPassDesc.colorAttachments[i];
            colorAttachment.storeAction = MTLStoreActionDontCare;
            if (renderPassStarted)
            {
                encoder->setColorStoreAction(MTLStoreActionDontCare, i);
            }
        }
    }

    if (invalidateDepthBuffer && mDepthRenderTarget)
    {
        mRenderPassDesc.depthAttachment.storeAction = MTLStoreActionDontCare;
        if (renderPassStarted)
        {
            encoder->setDepthStoreAction(MTLStoreActionDontCare);
        }
    }

    if (invalidateStencilBuffer && mStencilRenderTarget)
    {
        mRenderPassDesc.stencilAttachment.storeAction = MTLStoreActionDontCare;
        if (renderPassStarted)
        {
            encoder->setStencilStoreAction(MTLStoreActionDontCare);
        }
    }

    return angle::Result::Continue;
}

gl::Rectangle FramebufferMtl::getReadArea(const gl::Context *context, const gl::Rectangle &glArea)
{
    RenderTargetMtl *readRT = getColorReadRenderTarget(context);
    if (!readRT)
    {
        readRT = mDepthRenderTarget;
    }
    if (!readRT)
    {
        readRT = mStencilRenderTarget;
    }
    ASSERT(readRT);
    gl::Rectangle flippedArea = glArea;
    if (mFlipY)
    {
        flippedArea.y =
            readRT->getTexture()->height(static_cast<uint32_t>(readRT->getLevelIndex())) -
            flippedArea.y - flippedArea.height;
    }

    return flippedArea;
}

angle::Result FramebufferMtl::readPixelsImpl(const gl::Context *context,
                                             const gl::Rectangle &area,
                                             const PackPixelsParams &packPixelsParams,
                                             RenderTargetMtl *renderTarget,
                                             uint8_t *pixels)
{
    ContextMtl *contextMtl = mtl::GetImpl(context);
    if (!renderTarget)
    {
        return angle::Result::Continue;
    }
    mtl::TextureRef texture = renderTarget->getTexture();

    if (!texture)
    {
        return angle::Result::Continue;
    }

    if (packPixelsParams.packBuffer)
    {
        return readPixelsToPBO(context, area, packPixelsParams, renderTarget);
    }

    const mtl::Format &readFormat        = *renderTarget->getFormat();
    const angle::Format &readAngleFormat = readFormat.actualAngleFormat();

    int srcRowPitch = area.width * readAngleFormat.pixelBytes;
    angle::MemoryBuffer readPixelRowBuffer;
    ANGLE_CHECK_GL_ALLOC(contextMtl, readPixelRowBuffer.resize(srcRowPitch));

    auto packPixelsRowParams = packPixelsParams;
    gl::Rectangle srcRowRegion(area.x, area.y, area.width, 1);

    int rowOffset = packPixelsParams.reverseRowOrder ? -1 : 1;
    int startRow  = packPixelsParams.reverseRowOrder ? (area.y1() - 1) : area.y;

    // Copy pixels row by row
    packPixelsRowParams.area.height     = 1;
    packPixelsRowParams.reverseRowOrder = false;
    for (int r = startRow, i = 0; i < area.height;
         ++i, r += rowOffset, pixels += packPixelsRowParams.outputPitch)
    {
        srcRowRegion.y             = r;
        packPixelsRowParams.area.y = packPixelsParams.area.y + i;

        // Read the pixels data to the row buffer
        ANGLE_TRY(mtl::ReadTexturePerSliceBytes(
            context, texture, srcRowPitch, srcRowRegion, renderTarget->getLevelIndex(),
            renderTarget->getLayerIndex(), readPixelRowBuffer.data()));

        // Convert to destination format
        PackPixels(packPixelsRowParams, readAngleFormat, srcRowPitch, readPixelRowBuffer.data(),
                   pixels);
    }

    return angle::Result::Continue;
}

angle::Result FramebufferMtl::readPixelsToPBO(const gl::Context *context,
                                              const gl::Rectangle &area,
                                              const PackPixelsParams &packPixelsParams,
                                              RenderTargetMtl *renderTarget)
{
    ASSERT(packPixelsParams.packBuffer);
    ASSERT(renderTarget);

    ContextMtl *contextMtl = mtl::GetImpl(context);

    const mtl::Format &readFormat        = *renderTarget->getFormat();
    const angle::Format &readAngleFormat = readFormat.actualAngleFormat();

    ANGLE_MTL_CHECK(contextMtl, packPixelsParams.offset <= std::numeric_limits<uint32_t>::max(),
                    GL_INVALID_OPERATION);
    uint32_t offset = static_cast<uint32_t>(packPixelsParams.offset);

    mtl::TextureRef texture = renderTarget->getTexture();

    BufferMtl *packBufferMtl = mtl::GetImpl(packPixelsParams.packBuffer);
    mtl::BufferRef dstBuffer = packBufferMtl->getCurrentBuffer();

    if (packPixelsParams.destFormat->id != readAngleFormat.id ||
        (offset % packPixelsParams.destFormat->pixelBytes))
    {
        const angle::Format *actualDstAngleFormat;

        // SRGB is special case: We need to write sRGB values to buffer, not linear values.
        switch (readAngleFormat.id)
        {
            case angle::FormatID::B8G8R8A8_UNORM_SRGB:
            case angle::FormatID::R8G8B8_UNORM_SRGB:
            case angle::FormatID::R8G8B8A8_UNORM_SRGB:
                if (packPixelsParams.destFormat->id != readAngleFormat.id)
                {
                    switch (packPixelsParams.destFormat->id)
                    {
                        case angle::FormatID::B8G8R8A8_UNORM:
                            actualDstAngleFormat =
                                &angle::Format::Get(angle::FormatID::B8G8R8A8_UNORM_SRGB);
                            break;
                        case angle::FormatID::R8G8B8A8_UNORM:
                            actualDstAngleFormat =
                                &angle::Format::Get(angle::FormatID::R8G8B8A8_UNORM_SRGB);
                            break;
                        default:
                            // Unsupported format.
                            ANGLE_MTL_CHECK(contextMtl, false, GL_INVALID_ENUM);
                    }
                    break;
                }
                OS_FALLTHROUGH;
            default:
                actualDstAngleFormat = packPixelsParams.destFormat;
        }

        // Use compute shader
        mtl::CopyPixelsToBufferParams params;
        params.buffer            = dstBuffer;
        params.bufferStartOffset = offset;
        params.bufferRowPitch    = packPixelsParams.outputPitch;

        params.texture                = texture;
        params.textureArea            = area;
        params.textureLevel           = renderTarget->getLevelIndex();
        params.textureSliceOrDeph     = renderTarget->getLayerIndex();
        params.reverseTextureRowOrder = packPixelsParams.reverseRowOrder;

        ANGLE_TRY(contextMtl->getDisplay()->getUtils().packPixelsFromTextureToBuffer(
            contextMtl, *actualDstAngleFormat, params));
    }
    else
    {
        // Use blit command encoder
        int srcRowPitch = area.width * readAngleFormat.pixelBytes;

        gl::Rectangle srcRowRegion(area.x, area.y, area.width, 1);

        int rowOffset = packPixelsParams.reverseRowOrder ? -1 : 1;
        int startRow  = packPixelsParams.reverseRowOrder ? (area.y1() - 1) : area.y;

        uint32_t bufferRowOffset = offset;
        // Copy pixels row by row
        for (int r = startRow, i = 0; i < area.height;
             ++i, r += rowOffset, bufferRowOffset += packPixelsParams.outputPitch)
        {
            srcRowRegion.y = r;

            // Read the pixels data to the buffer's row
            ANGLE_TRY(mtl::ReadTexturePerSliceBytesToBuffer(
                context, texture, srcRowPitch, srcRowRegion, renderTarget->getLevelIndex(),
                renderTarget->getLayerIndex(), bufferRowOffset, dstBuffer));
        }
    }

    return angle::Result::Continue;
}

}
