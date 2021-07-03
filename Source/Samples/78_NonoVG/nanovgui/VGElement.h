#pragma once
#include "GLHeaders.h"
#include "nanovg.h"
#include <Urho3D/UI/BorderImage.h>

struct NVGcontext;
struct NVGLUframebuffer;

namespace Urho3D
{

class Texture2D;

class URHO3D_API VGElement : public BorderImage
{
    URHO3D_OBJECT(VGElement, BorderImage);

    /// Construct.
    explicit VGElement(Context* context);
    /// Destruct.
    ~VGElement() override;
    /// Register object factory.
    /// @nobind
    static void RegisterObject(Context* context);

    /// React to resize.
    void OnResize(const IntVector2& newSize, const IntVector2& delta) override;

    void BeginRender();
    void EndRender();

    IntVector2 GetSize() { return textureSize_; }

    // Begin drawing a new frame
    // Calls to nanovg drawing API should be wrapped in nvgBeginFrame() & nvgEndFrame()
    // nvgBeginFrame() defines the size of the window to render to in relation currently
    // set viewport (i.e. glViewport on GL backends). Device pixel ration allows to
    // control the rendering on Hi-DPI devices.
    // For example, GLFW returns two dimension for an opened window: window size and
    // frame buffer size. In that case you would set windowWidth/Height to the window size
    // devicePixelRatio to: frameBufferWidth / windowWidth.
    void BeginFrame(float windowWidth, float windowHeight, float devicePixelRatio);

    // Cancels drawing the current frame.
    void CancelFrame();

    // Ends drawing flushing remaining render state.
    void EndFrame();

    //
    // Composite operation
    //
    // The composite operations in NanoVG are modeled after HTML Canvas API, and
    // the blend func is based on OpenGL (see corresponding manuals for more info).
    // The colors in the blending state have premultiplied alpha.

    // Sets the composite operation. The op parameter should be one of NVGcompositeOperation.
    void GlobalCompositeOperation(int op);

    // Sets the composite operation with custom pixel arithmetic. The parameters should be one of NVGblendFactor.
    void GlobalCompositeBlendFunc(int sfactor, int dfactor);

    // Sets the composite operation with custom pixel arithmetic for RGB and alpha components separately. The parameters
    // should be one of NVGblendFactor.
    void GlobalCompositeBlendFuncSeparate(int srcRGB, int dstRGB, int srcAlpha, int dstAlpha);

    //
    // Color utils
    //
    // Colors in NanoVG are stored as unsigned ints in ABGR format.

    // Returns a color value from red, green, blue values. Alpha will be set to 255 (1.0f).
    NVGcolor RGB(unsigned char r, unsigned char g, unsigned char b);

    // Returns a color value from red, green, blue values. Alpha will be set to 1.0f.
    NVGcolor RGBf(float r, float g, float b);

    // Returns a color value from red, green, blue and alpha values.
    NVGcolor RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    // Returns a color value from red, green, blue and alpha values.
    NVGcolor RGBAf(float r, float g, float b, float a);

    // Linearly interpolates from color c0 to c1, and returns resulting color value.
    NVGcolor LerpRGBA(NVGcolor c0, NVGcolor c1, float u);

    // Sets transparency of a color value.
    NVGcolor TransRGBA(NVGcolor c0, unsigned char a);

    // Sets transparency of a color value.
    NVGcolor TransRGBAf(NVGcolor c0, float a);

    // Returns color value specified by hue, saturation and lightness.
    // HSL values are all in range [0..1], alpha will be set to 255.
    NVGcolor HSL(float h, float s, float l);

    // Returns color value specified by hue, saturation and lightness and alpha.
    // HSL values are all in range [0..1], alpha in range [0..255]
    NVGcolor HSLA(float h, float s, float l, unsigned char a);

    //
    // State Handling
    //
    // NanoVG contains state which represents how paths will be rendered.
    // The state contains transform, fill and stroke styles, text and font styles,
    // and scissor clipping.

    // Pushes and saves the current render state into a state stack.
    // A matching nvgRestore() must be used to restore the state.
    void SaveState();

    // Pops and restores current render state.
    void RestoreState();

    // Resets current render state to default values. Does not affect the render state stack.
    void ResetState();

    //
    // Render styles
    //
    // Fill and stroke render style can be either a solid color or a paint which is a gradient or a pattern.
    // Solid color is simply defined as a color value, different kinds of paints can be created
    // using nvgLinearGradient(), nvgBoxGradient(), nvgRadialGradient() and nvgImagePattern().
    //
    // Current render style can be saved and restored using nvgSave() and nvgRestore().

    // Sets whether to draw antialias for nvgStroke() and nvgFill(). It's enabled by default.
    void ShapeAntiAlias(int enabled);

    // Sets current stroke style to a solid color.
    void StrokeColor(NVGcolor color);

    // Sets current stroke style to a paint, which can be a one of the gradients or a pattern.
    void StrokePaint(NVGpaint paint);

    // Sets current fill style to a solid color.
    void FillColor(NVGcolor color);

    // Sets current fill style to a paint, which can be a one of the gradients or a pattern.
    void FillPaint(NVGpaint paint);

    // Sets the miter limit of the stroke style.
    // Miter limit controls when a sharp corner is beveled.
    void MiterLimit(float limit);

    // Sets the stroke width of the stroke style.
    void StrokeWidth(float size);

    // Sets how the end of the line (cap) is drawn,
    // Can be one of: NVG_BUTT (default), NVG_ROUND, NVG_SQUARE.
    void LineCap(int cap);

    // Sets how sharp path corners are drawn.
    // Can be one of NVG_MITER (default), NVG_ROUND, NVG_BEVEL.
    void LineJoin(int join);

    // Sets the transparency applied to all rendered shapes.
    // Already transparent paths will get proportionally more transparent as well.
    void GlobalAlpha(float alpha);

    //
    // Transforms
    //
    // The paths, gradients, patterns and scissor region are transformed by an transformation
    // matrix at the time when they are passed to the API.
    // The current transformation matrix is a affine matrix:
    //   [sx kx tx]
    //   [ky sy ty]
    //   [ 0  0  1]
    // Where: sx,sy define scaling, kx,ky skewing, and tx,ty translation.
    // The last row is assumed to be 0,0,1 and is not stored.
    //
    // Apart from nvgResetTransform(), each transformation function first creates
    // specific transformation matrix and pre-multiplies the current transformation by it.
    //
    // Current coordinate system (transformation) can be saved and restored using nvgSave() and nvgRestore().

    // Resets current transform to a identity matrix.
    void ResetTransform();

    // Premultiplies current coordinate system by specified matrix.
    // The parameters are interpreted as matrix as follows:
    //   [a c e]
    //   [b d f]
    //   [0 0 1]
    void Transform(float a, float b, float c, float d, float e, float f);

    // Translates current coordinate system.
    void Translate(float x, float y);

    // Rotates current coordinate system. Angle is specified in radians.
    void Rotate(float angle);

    // Skews the current coordinate system along X axis. Angle is specified in radians.
    void SkewX(float angle);

    // Skews the current coordinate system along Y axis. Angle is specified in radians.
    void SkewY(float angle);

    // Scales the current coordinate system.
    void Scale(float x, float y);

    // Stores the top part (a-f) of the current transformation matrix in to the specified buffer.
    //   [a c e]
    //   [b d f]
    //   [0 0 1]
    // There should be space for 6 floats in the return buffer for the values a-f.
    void CurrentTransform(float* xform);

    // The following functions can be used to make calculations on 2x3 transformation matrices.
    // A 2x3 matrix is represented as float[6].

    // Sets the transform to identity matrix.
    void TransformIdentity(float* dst);

    // Sets the transform to translation matrix matrix.
    void TransformTranslate(float* dst, float tx, float ty);

    // Sets the transform to scale matrix.
    void TransformScale(float* dst, float sx, float sy);

    // Sets the transform to rotate matrix. Angle is specified in radians.
    void TransformRotate(float* dst, float a);

    // Sets the transform to skew-x matrix. Angle is specified in radians.
    void TransformSkewX(float* dst, float a);

    // Sets the transform to skew-y matrix. Angle is specified in radians.
    void TransformSkewY(float* dst, float a);

    // Sets the transform to the result of multiplication of two transforms, of A = A*B.
    void TransformMultiply(float* dst, const float* src);

    // Sets the transform to the result of multiplication of two transforms, of A = B*A.
    void TransformPremultiply(float* dst, const float* src);

    // Sets the destination to inverse of specified transform.
    // Returns 1 if the inverse could be calculated, else 0.
    int TransformInverse(float* dst, const float* src);

    // Transform a point by given transform.
    void TransformPoint(float* dstx, float* dsty, const float* xform, float srcx, float srcy);

    // Converts degrees to radians and vice versa.
    float DegToRad(float deg);
    float RadToDeg(float rad);
    
    // Paths
    //
    // Drawing a new shape starts with nvgBeginPath(), it clears all the currently defined paths.
    // Then you define one or more paths and sub-paths which describe the shape. The are functions
    // to draw common shapes like rectangles and circles, and lower level step-by-step functions,
    // which allow to define a path curve by curve.
    //
    // NanoVG uses even-odd fill rule to draw the shapes. Solid shapes should have counter clockwise
    // winding and holes should have counter clockwise order. To specify winding of a path you can
    // call nvgPathWinding(). This is useful especially for the common shapes, which are drawn CCW.
    //
    // Finally you can fill the path using current fill style by calling nvgFill(), and stroke it
    // with current stroke style by calling nvgStroke().
    //
    // The curve segments and sub-paths are transformed by the current transform.

    // Clears the current path and sub-paths.
    void BeginPath();

    // Starts new sub-path with specified point as first point.
    void MoveTo( float x, float y);

    // Adds line segment from the last point in the path to the specified point.
    void LineTo( float x, float y);

    // Adds cubic bezier segment from last point in the path via two control points to the specified point.
    void BezierTo( float c1x, float c1y, float c2x, float c2y, float x, float y);

    // Adds quadratic bezier segment from last point in the path via a control point to the specified point.
    void QuadTo( float cx, float cy, float x, float y);

    // Adds an arc segment at the corner defined by the last path point, and two specified points.
    void ArcTo( float x1, float y1, float x2, float y2, float radius);

    // Closes current sub-path with a line segment.
    void ClosePath();

    // Sets the current sub-path winding, see NVGwinding and NVGsolidity.
    void PathWinding( int dir);

    // Creates new circle arc shaped sub-path. The arc center is at cx,cy, the arc radius is r,
    // and the arc is drawn from angle a0 to a1, and swept in direction dir (NVG_CCW, or NVG_CW).
    // Angles are specified in radians.
    void Arc( float cx, float cy, float r, float a0, float a1, int dir);

    // Creates new rectangle shaped sub-path.
    void Rect( float x, float y, float w, float h);

    // Creates new rounded rectangle shaped sub-path.
    void RoundedRect( float x, float y, float w, float h, float r);

    // Creates new rounded rectangle shaped sub-path with varying radii for each corner.
    void RoundedRectVarying( float x, float y, float w, float h, float radTopLeft, float radTopRight,
                               float radBottomRight, float radBottomLeft);

    // Creates new ellipse shaped sub-path.
    void Ellipse( float cx, float cy, float rx, float ry);

    // Creates new circle shaped sub-path.
    void Circle( float cx, float cy, float r);

    // Fills the current path with current fill style.
    void Fill();

    // Fills the current path with current stroke style.
    void Stroke();

protected:
    void CreateFrameBuffer(int mWidth, int mHeight);

protected:
    WeakPtr<Graphics> graphics_;
    Urho3D::SharedPtr<Texture2D> drawTexture_;
    IntVector2 textureSize_;
    NVGLUframebuffer* nvgFrameBuffer_;
    NVGcontext* vg_;
    GLint previousVBO;
    GLint previousFBO;
    ShaderVariation* previousVS;
    ShaderVariation* previousPS;

private:
    // Handle render event.
    void HandleRender(StringHash eventType, VariantMap& eventData);
};

} // namespace Urho3D
