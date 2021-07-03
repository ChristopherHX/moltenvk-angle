/*!
 * \file NanoVG.h
 *
 * \author vitali
 * \date Februar 2015
 *
 * 
 */

#pragma once


#include <Urho3D/Core/Object.h>
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

	
class NanoVG : public Object
{
    URHO3D_OBJECT(NanoVG, Object);

public:
	NanoVG(Context* context);
	virtual ~NanoVG();
	/// Initialize when screen mode initially set.
	void Initialize();
	void Clear();
	NVGcontext * GetNVGContext() { return vg_; }

protected:

	/// Initialized flag.
	bool initialized_;
	/// Graphics subsystem.
	WeakPtr<Graphics> graphics_;

	/// nanovg context
	NVGcontext* vg_;

private:
};
}

