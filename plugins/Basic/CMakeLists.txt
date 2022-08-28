cmake_minimum_required(VERSION 3.15)

### top menu (left)
add_subdirectory(loadPolygonMesh)

### top menu (right)
add_subdirectory(undo)
add_subdirectory(redo)

### per-mesh
add_subdirectory(loadTexture)
add_subdirectory(saveMesh)
add_subdirectory(saveScreenshot)
add_subdirectory(removeMesh)
add_subdirectory(toggleMeshVisibility)
add_subdirectory(meshInfo)
add_subdirectory(meshErrorInfo)

### summary
add_subdirectory(allMeshesInfo)

### bottom menu (left)
add_subdirectory(saveAllMeshes)
add_subdirectory(saveScreenshotAll)
add_subdirectory(removeAllMeshes)
add_subdirectory(toggleAxesVisibility)

### bottom menu (right)
# no entry

### others
add_subdirectory(alignCamera)
add_subdirectory(crossSectionalView)
add_subdirectory(fitToFrame)
add_subdirectory(outlineOnClick)
add_subdirectory(syncViewport)
add_subdirectory(syncCursor)
add_subdirectory(transformGizmo)

