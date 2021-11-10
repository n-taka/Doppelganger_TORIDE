#!/bin/bash

# MeshFix
cmake -B build_MeshFix -S ./submodule/MeshFix-V2.1
cmake --build build_MeshFix --config "Release"
if [ "$(uname)" == "Darwin" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairMesh/
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairAllMeshes/
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairMesh/
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairAllMeshes/
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairMesh/
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairAllMeshes/
fi
