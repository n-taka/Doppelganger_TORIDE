#!/bin/bash

# MeshFix
cmake -B build_MeshFix -S ./submodule/MeshFix-V2.1
cmake --build build_MeshFix --config "Release"
if [ "$(uname)" == "Darwin" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/TORIDE/repairMesh/MeshFix_macOS
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/TORIDE/repairAllMeshes/MeshFix_macOS
    rm ./submodule/MeshFix-V2.1/bin64/MeshFix
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairMesh/
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/TORIDE/repairAllMeshes/
    rm ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/TORIDE/repairMesh/MeshFix_Linux
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/TORIDE/repairAllMeshes/MeshFix_Linux
    rm ./submodule/MeshFix-V2.1/bin64/MeshFix
fi
