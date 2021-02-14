
./script/cmake_ios_dotnet_lib.sh build-ios-dotnet-lib
xcodebuild -project  build-ios-dotnet-lib/Urho3D.xcodeproj -target Urho3D
cp -Lf build-ios-dotnet-lib/lib/libUrho3D.a  DotNet/libs/iphone/Release-iphoneos/libUrho3D-GLES.a 

./script/cmake_ios_dotnet_metal_lib.sh build-ios-dotnet-metal-lib
xcodebuild -project  build-ios-dotnet-metal-lib/Urho3D.xcodeproj -target Urho3D
cp -Lf build-ios-dotnet-metal-lib/lib/libUrho3D.a  DotNet/libs/iphone/Release-iphoneos/libUrho3D-Metal.a 