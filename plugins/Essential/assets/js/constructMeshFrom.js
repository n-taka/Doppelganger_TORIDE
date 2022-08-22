import * as THREE from 'https://cdn.skypack.dev/three@v0.132';
import { Canvas } from './Canvas.js';

////
// [IN]
// parameters = {
//     "meshes": {
//         "<UUID>": {
//             "UUID": UUID of this mesh,
//             "name": name of this mesh,
//             "visibility": visibility of this mesh,
//             "V": base64-encoded vertices (#V),
//             "F": base64-encoded facets (#F),
//             "VC": base64-encoded vertex colors (#V),
//             "TC": base64-encoded texture coordinates (#V),
//             "FC": base64-encoded vertices (#F, only for edit history),
//             "FTC": base64-encoded vertices (#F, only for edit history),
//             "textures": [
//                 {
//                     "name": original filename for this texture
//                     "width" = width of this texture
//                     "height" = height of this texture
//                     "texData" = base64-encoded texture data
//                 },
//                 ...
//             ],
//             "matrix": {
//                 "world": [
//                     [1, 0, 0, 0],
//                     [0, 1, 0, 0],
//                     [0, 0, 1, 0],
//                     [0, 0, 0, 1]
//                 ]
//             }
//         },
//         "<UUID>": null, // to be removed
//         ...
//     }
// }
// 
// [OUT]
// nothing
//
export const constructMeshFromParameters = async function (parameters) {
    if ("meshes" in parameters) {
        for (let meshUUID in parameters["meshes"]) {
            if (!(meshUUID in Canvas.UUIDToMesh)) {
                const geometry = new THREE.BufferGeometry();
                const material = new THREE.MeshPhongMaterial({ color: 0xFFFFFF, flatShading: true, vertexColors: THREE.NoColors });
                const mesh = new THREE.Mesh(geometry, material);

                Canvas.UUIDToMesh[meshUUID] = mesh;
                Canvas.meshGroup.add(mesh);
            }
            if (parameters["meshes"][meshUUID] == null) {
                // null (remove)
                const mesh = Canvas.UUIDToMesh[meshUUID];
                // remove from scene
                Canvas.meshGroup.remove(mesh);
                // remove from map
                delete Canvas.UUIDToMesh[meshUUID];
                // dispose geometry/material
                //   children[0]: backFaceMesh, geometry is shared
                mesh.children[0].material.dispose();
                mesh.geometry.dispose();
                mesh.material.dispose();
            } else {
                // non-null (update)
                await updateMeshFromJson(Canvas.UUIDToMesh[meshUUID], parameters["meshes"][meshUUID]);
            }
        }
        Canvas.resetCamera(true);
    }
    for (let handler of constructMeshFromParameters.handlers) {
        await handler();
    }
}
// handlers that need to be called when we call constructMeshFromParameters
// function (void) { ... }
constructMeshFromParameters.handlers = [];

////
// [IN]
// meshPrev = {
//     previous mesh
// }
// json = {
//     "UUID": UUID of this mesh,
//     "name": name of this mesh,
//     "visibility": visibility of this mesh,
//     "V": base64-encoded vertices (#V),
//     "F": base64-encoded facets (#F),
//     "VC": base64-encoded vertex colors (#V),
//     "TC": base64-encoded texture coordinates (#V),
//     "FC": base64-encoded vertices (#F, only for edit history),
//     "FTC": base64-encoded vertices (#F, only for edit history),
//     "textures": [
//         {
//             "name": original filename for this texture
//             "width" = width of this texture
//             "height" = height of this texture
//             "texData" = base64-encoded texture data
//         }
//     ],
//     "matrix": {
//         "world": [
//             [1, 0, 0, 0],
//             [0, 1, 0, 0],
//             [0, 0, 1, 0],
//             [0, 0, 0, 1]
//         ]
//     }
// }
// 
// [OUT]
// THREE.Mesh
//
export const updateMeshFromJson = async function (mesh, json) {
    // IMPORTANT
    // If the mesh has face color, server automatically convert
    // face color to vertex color (by duplicating the vertices)
    function b64ToUint6(nChr) {
        return nChr > 64 && nChr < 91 ?
            nChr - 65
            : nChr > 96 && nChr < 123 ?
                nChr - 71
                : nChr > 47 && nChr < 58 ?
                    nChr + 4
                    : nChr === 43 ?
                        62
                        : nChr === 47 ?
                            63
                            :
                            0;
    }
    function base64DecToArr(sBase64, nBlockSize) {
        const sB64Enc = sBase64.replace(/[^A-Za-z0-9\+\/]/g, "");
        const nInLen = sB64Enc.length;
        const nOutLen = nBlockSize ? Math.ceil((nInLen * 3 + 1 >>> 2) / nBlockSize) * nBlockSize : nInLen * 3 + 1 >>> 2;
        const aBytes = new Uint8Array(nOutLen);

        for (let nMod3, nMod4, nUint24 = 0, nOutIdx = 0, nInIdx = 0; nInIdx < nInLen; nInIdx++) {
            nMod4 = nInIdx & 3;
            nUint24 |= b64ToUint6(sB64Enc.charCodeAt(nInIdx)) << 18 - 6 * nMod4;
            if (nMod4 === 3 || nInLen - nInIdx === 1) {
                for (nMod3 = 0; nMod3 < 3 && nOutIdx < nOutLen; nMod3++, nOutIdx++) {
                    aBytes[nOutIdx] = nUint24 >>> (16 >>> nMod3 & 24) & 255;
                }
                nUint24 = 0;
            }
        }
        return aBytes;
    }

    // V, 32 bit (4 byte) float in the client
    if ("V" in json && json["V"].length > 0) {
        const array = base64DecToArr(json["V"], 4);
        mesh.geometry.setAttribute('position', new THREE.Float32BufferAttribute(new Float32Array(array.buffer), 3));
        mesh.geometry.deleteAttribute('normal');
        mesh.geometry.getAttribute('position').needsUpdate = true;
    }
    // F, 32 bit (4 byte) int in the client
    if ("F" in json && json["F"].length > 0) {
        const array = base64DecToArr(json["F"], 4);
        mesh.geometry.setIndex(new THREE.Uint32BufferAttribute(new Int32Array(array.buffer), 1));
    }
    // VC, 32 bit (4 byte) float in the client
    if ("VC" in json) {
        // we accept VC even if VC.rows() != V.rows() to support erasing VC with undo/redo
        const array = base64DecToArr(json["VC"], 4);
        mesh.geometry.setAttribute('color', new THREE.Float32BufferAttribute(new Float32Array(array.buffer), 3));
        mesh.geometry.getAttribute('color').needsUpdate = true;
    }
    // TC, 32 bit (4 byte) float in the client
    if ("TC" in json && json["TC"].length > 0) {
        const array = base64DecToArr(json["TC"], 4);
        mesh.geometry.setAttribute('uv', new THREE.Float32BufferAttribute(new Float32Array(array.buffer), 2));
        mesh.geometry.getAttribute('uv').needsUpdate = true;
    }
    // Tex, 8 bit (1 byte) uint in the client
    //   Currently, we only support single texture
    if ("textures" in json && json["textures"].length > 0) {
        const array = base64DecToArr(json["textures"][0]["texData"], 1);
        mesh.material.map = new THREE.DataTexture(new Uint8Array(array.buffer), json["textures"][0]["width"], json["textures"][0]["height"], THREE.RGBAFormat);
        mesh.material.map.needsUpdate = true;
    }

    // matrixWorld
    if ("matrix" in json && "world" in json["matrix"]) {
        mesh.matrixWorld.fromArray(json["matrix"]["world"]);
    }

    if (mesh.material.map && mesh.geometry.hasAttribute('position') && mesh.geometry.hasAttribute('uv') && mesh.geometry.getAttribute('position').count == mesh.geometry.getAttribute('uv').count) {
        // texture
        mesh.material.vertexColors = THREE.NoColors;
    } else if (mesh.geometry.hasAttribute('position') &&
        mesh.geometry.hasAttribute('color') &&
        mesh.geometry.getAttribute('position').count == mesh.geometry.getAttribute('color').count) {
        // vertex color (user-defined)
        mesh.material.vertexColors = THREE.VertexColors;
    } else {
        // vertex color (default, grey)
        const buffer = new ArrayBuffer(mesh.geometry.getAttribute('position').count * 3 * 4);
        const view = new Float32Array(buffer);
        for (let i = 0; i < mesh.geometry.getAttribute('position').count; ++i) {
            for (let rgb = 0; rgb < 3; ++rgb) {
                view[i * 3 + rgb] = Canvas.defaultColor[rgb];
            }
        }
        mesh.geometry.setAttribute('color', new THREE.Float32BufferAttribute(view, 3));
        mesh.geometry.getAttribute('color').needsUpdate = true;
        mesh.material.vertexColors = THREE.VertexColors;
    }
    mesh.geometry.computeVertexNormals();
    mesh.material.needsUpdate = true;


    // UUID
    if ("UUID" in json) {
        mesh.DoppelgangerUUID = json["UUID"];
    }

    // name
    if ("name" in json) {
        mesh.name = json["name"];
    }
    // visibility
    if ("visibility" in json) {
        mesh.visible = json["visibility"];
    }

    // create mesh for backface
    if (mesh.children.length == 0) {
        // const backFaceMaterial = new THREE.MeshPhongMaterial({ color: 0xf56c0a, flatShading: true, vertexColors: THREE.NoColors, side: THREE.BackSide });
        const backFaceMaterial = new THREE.MeshBasicMaterial({ color: 0xf56c0a, vertexColors: THREE.NoColors, side: THREE.BackSide });
        const backFaceMesh = new THREE.Mesh(mesh.geometry, backFaceMaterial);
        mesh.add(backFaceMesh);
    }

    for (let handler of updateMeshFromJson.handlers) {
        await handler(json, mesh);
    }
}

// handlers that need to be called when we call updateMeshFromJson
// function (json, mesh) { ... }
updateMeshFromJson.handlers = [];
