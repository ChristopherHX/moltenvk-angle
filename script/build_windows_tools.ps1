cmd /c rmdir build-vs2019-tools /s /q 
script/cmake_vs2019_tools.bat build-vs2019-tools
cmake --build  build-vs2019-tools/Source/ThirdParty/Assimp/.  --config Release
cmake --build  build-vs2019-tools/. --config Release
Copy-Item   build-vs2019-tools/bin/Assimp.dll  -Destination build-vs2019-tools/bin/tool
Copy-Item   build-vs2019-tools/bin/draco.dll  -Destination build-vs2019-tools/bin/tool

