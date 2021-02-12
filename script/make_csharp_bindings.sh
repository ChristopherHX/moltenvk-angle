LOCAL_CLANG=tools/clang+llvm-3.7.0-x86_64-apple-darwin/bin/clang
CUSTOM_CLANG=$(pwd)/${LOCAL_CLANG}


./script/cmake_xcode.sh build-xcode -DURHO3D_PCH=0  -DURHO3D_WEBP=0 -DURHO3D_LUA=0 -DURHO3D_ANGELSCRIPT=0 -DURHO3D_TOOLS=1  -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15


${CUSTOM_CLANG} -cc1 -stdlib=libc++ -std=c++0x -emit-pch -DURHO3D_OPENGL -o DotNet/Bindings/Urho.pch DotNet/Bindings/Native/all-urho.cpp  -Ibuild-xcode/include -Ibuild-xcode/include/Urho3D/ThirdParty -Ibuild-xcode/include/Urho3D/ThirdParty/Bullet