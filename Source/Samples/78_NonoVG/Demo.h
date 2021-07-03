#ifndef DEMO_H
#define DEMO_H

#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/IO/File.h>
#include "nanovg.h"

using namespace Urho3D;

#ifdef __cplusplus
extern "C" {
#endif

struct DemoData {
	int fontNormal, fontBold, fontIcons, fontEmoji;
	int images[12];
};
typedef struct DemoData DemoData;

int loadDemoData(Urho3D::ResourceCache* resourceCache, NVGcontext* vg, DemoData* data);
void freeDemoData(NVGcontext* vg, DemoData* data);
void renderDemo(NVGcontext* vg, float mx, float my, float width, float height, float t, int blowup, DemoData* data);

void saveScreenShot(int w, int h, int premult, const char* name);

#ifdef __cplusplus
}
#endif

#endif // DEMO_H
