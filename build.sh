#!/bin/bash

############
# OS detection
############
triplet="x"
if [ "$(uname)" == "Darwin" ]; then
    # we use custom triplet (x64-osx-mojave)
    triplet="${triplet}64-osx-mojave"
    # copy custom triplet file (for supporting Mojave)
    cp "${triplet}.cmake" "submodule/vcpkg/triplets/${triplet}.cmake"
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
    triplet="${triplet}64-windows-static"
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    triplet="${triplet}64-linux"
else
    echo "This OS is not supported..."
    exit 1
fi

############
# build project
############
if [ "$(uname)" == "Darwin" ]; then
    cmake -B build -S . -DVCPKG_TARGET_TRIPLET="${triplet}" -DCMAKE_BUILD_TYPE="Release"
    cmake --build build --config "Release"
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
    subst X: .
    cd X:
    cmake -B build -S . -DVCPKG_TARGET_TRIPLET="${triplet}" -DCMAKE_BUILD_TYPE="Release"
    cmake --build build --config "Release"
    # revert subst command
    # "/" symbol was comprehended as separator for path in MINGW. Thus, we need to explicitly use "//"
    # echo "unbind ./submodule as X:"
    subst X: //D
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    cmake -B build -S . -DVCPKG_TARGET_TRIPLET="${triplet}" -DCMAKE_BUILD_TYPE="Release"
    cmake --build build --config "Release"
else
    echo "This OS is not supported..."
    exit 1
fi
