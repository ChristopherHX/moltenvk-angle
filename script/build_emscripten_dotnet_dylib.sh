
CWD=$(pwd)
unamestr=$(uname)
# Switch-on alias expansion within the script 
shopt -s expand_aliases

#Alias the sed in-place command for OSX and Linux - incompatibilities between BSD and Linux sed args
if [[ "$unamestr" == "Darwin" ]]; then
	alias aliassedinplace='sed -i ""'
else
	#For Linux, notice no space after the '-i' 
	alias aliassedinplace='sed -i""'
fi


./script/cmake_emscripten_dotnet_lib.sh build-emscripten-dotnet-dylib
cd build-emscripten-dotnet-dylib
aliassedinplace "s*,-Bstatic**g" "Source/Samples/71_MonoEmbed/CMakeFiles/MonoEmbedded.dir/link.txt"
aliassedinplace "s*,-Bdynamic**g" "Source/Samples/71_MonoEmbed/CMakeFiles/MonoEmbedded.dir/link.txt"
make -j4
../tools/python/macos/bin/python3 ../tools/ems_tools/tools/file_packager.py  bin/UrhoNetFileSystem.data  --preload "../bin@/"   --js-output=bin/UrhoNetFileSystemPreloader.js --use-preload-cache --lz4
cd ..