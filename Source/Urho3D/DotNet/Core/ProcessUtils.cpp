#include "../../Core/ProcessUtils.h"
#include "../../DotNet/Defines.h"

using namespace Urho3D;

extern "C"
{

// TODO: should return
URHO3D_API void ProcessUtils_ParseArguments(const char* cmdLine)
{
    ParseArguments(cmdLine);
}

}
