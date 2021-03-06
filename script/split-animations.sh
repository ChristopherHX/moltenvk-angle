#!/bin/bash

while getopts a:e: option
do
case "${option}"
in
a) INPUT=${OPTARG};;
e) ECHO=${OPTARG};;
esac
done

if [[ "$INPUT" != "" ]]; then
    while IFS= read -r line
    do
    arrIN=(${line//-/ })
    name="$(echo -e " ${arrIN[2]}" | tr -d '[:space:]')"
    start="$(echo -e " ${arrIN[0]}" | tr -d '[:space:]')"
    end="$(echo -e " ${arrIN[1]}" | tr -d '[:space:]')"
    ./AssetImporter.exe anim drago-anim_DirectX.X ${name} -split ${start}  ${end}
    #echo \"${name}\",
    done < "$INPUT"
fi

if [[ "$ECHO" != "" ]]; then
    while IFS= read -r line
    do
    arrIN=(${line//-/ })
    name="$(echo -e " ${arrIN[2]}" | tr -d '[:space:]')"
    start="$(echo -e " ${arrIN[0]}" | tr -d '[:space:]')"
    end="$(echo -e " ${arrIN[1]}" | tr -d '[:space:]')"
    echo \"${name}\",
    done < "$ECHO"
fi