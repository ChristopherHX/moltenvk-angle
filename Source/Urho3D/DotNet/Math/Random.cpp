#include "../../Math/Random.h"
#include "../../DotNet/Defines.h"
using namespace Urho3D;
extern "C"
{

URHO3D_API int Rand()
{
    return Urho3D::Rand();
}

}
