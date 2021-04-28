URHONET_HOME_ROOT=$(cat ~/.urhonet_config/urhonethome)
URHO3D_HOME=$(pwd)

if [ ! -d "$URHONET_HOME_ROOT" ]; then
    echo  "Urho.Net is not configured , please  run configure.sh (configure.bat on Windows) from the Urho.Net installation folder  "
    exit -1
else
    echo "URHONET_HOME_ROOT=${URHONET_HOME_ROOT}"
fi

mkdir -p ${URHONET_HOME_ROOT}/template/libs/dotnet/urho/desktop
cp -f ${URHO3D_HOME}/DotNet/UrhoDotNet/desktop/UrhoDotNet.dll ${URHONET_HOME_ROOT}/template/libs/dotnet/urho/desktop
cp -f ${URHO3D_HOME}/DotNet/UrhoDotNet/desktop/UrhoDotNet.xml ${URHONET_HOME_ROOT}/template/libs/dotnet/urho/desktop
mkdir -p ${URHONET_HOME_ROOT}/template/libs/dotnet/urho/mobile/ios
cp -f ${URHO3D_HOME}/DotNet/UrhoDotNet/mobile/ios/UrhoDotNet.dll ${URHONET_HOME_ROOT}/template/libs/dotnet/urho/mobile/ios
mkdir -p ${URHONET_HOME_ROOT}/template/libs/dotnet/urho/mobile/android
cp -f ${URHO3D_HOME}/DotNet/UrhoDotNet/mobile/android/UrhoDotNet.dll  ${URHONET_HOME_ROOT}/template/libs/dotnet/urho/mobile/android

mkdir -p ${URHONET_HOME_ROOT}/template/libs/ios/urho3d/gles
cp -f ${URHO3D_HOME}/DotNet/libs/iphone/Release-iphoneos/libUrho3D-GLES.a  ${URHONET_HOME_ROOT}/template/libs/ios/urho3d/gles/libUrho3D.a
mkdir -p ${URHONET_HOME_ROOT}/template/libs/ios/urho3d/metal
cp -f ${URHO3D_HOME}/DotNet/libs/iphone/Release-iphoneos/libUrho3D-Metal.a ${URHONET_HOME_ROOT}/template/libs/ios/urho3d/metal/libUrho3D.a

mkdir -p ${URHONET_HOME_ROOT}/template/libs/macos
cp -f ${URHO3D_HOME}/DotNet/libs/macos/Release/libUrho3D.dylib  ${URHONET_HOME_ROOT}/template/libs/macos

mkdir -p ${URHONET_HOME_ROOT}/template/libs/windows
cp -f ${URHO3D_HOME}/DotNet/libs/windows/release/Urho3D.dll  ${URHONET_HOME_ROOT}/template/libs/windows

mkdir -p ${URHONET_HOME_ROOT}/template/libs/linux
cp -f ${URHO3D_HOME}/DotNet/libs/linux/release/libUrho3D.so  ${URHONET_HOME_ROOT}/template/libs/linux

cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/arm64-v8a/libUrho3D.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/arm64-v8a
cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/armeabi-v7a/libUrho3D.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/armeabi-v7a
cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/x86/libUrho3D.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/x86
cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/x86_64/libUrho3D.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/x86_64

cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/arm64-v8a/libMonoEmbedded.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/arm64-v8a
cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/armeabi-v7a/libMonoEmbedded.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/armeabi-v7a
cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/x86/libMonoEmbedded.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/x86
cp -f ${URHO3D_HOME}/DotNet/libs/android/release/lib/x86_64/libMonoEmbedded.so     ${URHONET_HOME_ROOT}/template/Android/app/src/main/jniLibs/x86_64
