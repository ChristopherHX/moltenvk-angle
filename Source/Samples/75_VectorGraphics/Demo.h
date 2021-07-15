#pragma once

#include <Urho3D/VectorGraphics/VectorGraphics.h>

using namespace Urho3D;

struct DemoData
{
    int fontNormal, fontBold, fontIcons, fontEmoji;
    int images[12];
    int svgImage;
};
typedef struct DemoData DemoData;


void loadDemoData(VectorGraphics* vge, DemoData* data);

void renderVGElement(VGElement * vgElement, float mx, float my, float width, float height, float t, int blowup, DemoData* data);
void renderVGFrameBuffer(VGFrameBuffer* vge, float mx, float my, float width, float height, float t, int blowup,
                         DemoData* data);

void drawColorwheelOnVGComponent(VGComponent* vge, float x, float y, float w, float h, float t);
