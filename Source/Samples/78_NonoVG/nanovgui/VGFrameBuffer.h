//
// Copyright (c) 2008-2021 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/Texture2D.h>


#include "GLHeaders.h"

struct NVGcontext;
struct NVGLUframebuffer;

namespace Urho3D
{
class Graphics;
class VertexBuffer;
class Cursor;
class ResourceCache;
class Timer;
class UIBatch;
class UIElement;
class XMLElement;
class XMLFile;
class NanoVG;

class VGFrameBuffer : public Object
{
    URHO3D_OBJECT(VGFrameBuffer, Object);

public:
    VGFrameBuffer(Context* context, int Width, int Height);
    ~VGFrameBuffer();

    void Bind();
    void UnBind();

    Texture2D* GetRenderTarget();
    IntVector2 GetSize() { return textureSize_; }
    void SetClearColor(Color color);
    Color GetClearColor();

   
protected:
    WeakPtr<Graphics> graphics_;
    Urho3D::SharedPtr<Texture2D> drawTexture_;
    IntVector2 textureSize_;
    NVGLUframebuffer* nvgFrameBuffer_;
    WeakPtr<NanoVG> nanoVG_;
    NVGcontext* vg_;
    GLint previousVBO;
    GLint previousFBO;
    ShaderVariation* previousVS;
    ShaderVariation* previousPS;
    Color clearColor_;

private:
    bool CreateFrameBuffer(int Width, int Height);
};

} // namespace Urho3D