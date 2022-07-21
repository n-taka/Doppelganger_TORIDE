#!/bin/bash

# MeshFix
cmake -B build_MeshFix -S ./submodule/MeshFix-V2.1 -DCMAKE_BUILD_TYPE="Release"
cmake --build build_MeshFix --config "Release"
if [ "$(uname)" = "Darwin" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/Print/repairMesh/MeshFix
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/Print/repairAllMeshes/MeshFix
    rm ./submodule/MeshFix-V2.1/bin64/MeshFix
elif [ "$(expr substr $(uname -s) 1 10)" = "MINGW64_NT" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/Print/repairMesh/MeshFix.exe
    cp ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe ./plugins/Print/repairAllMeshes/MeshFix.exe
    rm ./submodule/MeshFix-V2.1/bin64/Release/MeshFix.exe
elif [ "$(expr substr $(uname -s) 1 5)" = "Linux" ]; then
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/Print/repairMesh/MeshFix
    cp ./submodule/MeshFix-V2.1/bin64/MeshFix ./plugins/Print/repairAllMeshes/MeshFix
    rm ./submodule/MeshFix-V2.1/bin64/MeshFix
fi
