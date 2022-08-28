import * as THREE from 'https://cdn.skypack.dev/three@v0.132';
import { TransformControls } from 'https://cdn.skypack.dev/three@v0.132/examples/jsm/controls/TransformControls.js';
import { Canvas } from '../../js/Canvas.js';
import { updateMeshFromJson } from '../../js/constructMeshFrom.js';

let activeMeshUUID = "";

const findClosestMesh = function (e) {
    // [IN]
    //  e: mouse event
    // [OUT]
    //  UUID for intersected mesh
    const mouse = new THREE.Vector2();
    const clientX = e.clientX;
    const clientY = e.clientY;
    mouse.x = (clientX / Canvas.width) * 2 - 1;
    mouse.y = -(clientY / Canvas.height) * 2 + 1;

    const raycaster = new THREE.Raycaster();
    raycaster.setFromCamera(mouse, Canvas.camera);

    const intersects = raycaster.intersectObjects(Canvas.meshGroup.children);
    if (intersects.length > 0) {
        for (let h of intersects) {
            // we only check intersection for visible parts
            if (h.object.visible) {
                return h.object.DoppelgangerUUID;
            }
        }
    }
    return "";
}

export const init = async function () {
    Canvas.transformGizmo = new TransformControls(Canvas.camera, Canvas.renderer.domElement);
    Canvas.predefGroup.add(Canvas.transformGizmo);

    Canvas.transformGizmo.addEventListener('dragging-changed', function (event) {
        Canvas.controls.enabled = !event.value;
    });
    Canvas.transformGizmo.addEventListener('objectChange', function (event) {
        // todo
        //   post the update to the server
    });
    Canvas.transformGizmo.detach();


    Canvas.controls.domElement.addEventListener("pointerdown", function (e) {
        if (activeMeshUUID in Canvas.UUIDToMesh) {
            if (Canvas.transformGizmo.dragging) {
                // do nothing (TransformControls would take care)
            } else {
                // invalidate current gizmo
                Canvas.transformGizmo.detach();
                // then activate another one (if any)
                const closestMeshUUID = findClosestMesh(e);
                if (closestMeshUUID in Canvas.UUIDToMesh) {
                    Canvas.transformGizmo.attach(Canvas.UUIDToMesh[closestMeshUUID]);
                    activeMeshUUID = closestMeshUUID;
                } else {
                    activeMeshUUID = "";
                }
            }
        } else {
            // no gizmo is active
            const closestMeshUUID = findClosestMesh(e);
            if (closestMeshUUID in Canvas.UUIDToMesh) {
                Canvas.transformGizmo.attach(Canvas.UUIDToMesh[closestMeshUUID]);
                activeMeshUUID = closestMeshUUID;
            } else {
                activeMeshUUID = "";
            }
        }
    });

    window.addEventListener('keydown', function (event) {
        if (activeMeshUUID in Canvas.UUIDToMesh) {
            switch (event.code) {
                case 'KeyQ':
                    Canvas.transformGizmo.setSpace(Canvas.transformGizmo.space === 'local' ? 'world' : 'local');
                    break;
                case 'Shift':
                    Canvas.transformGizmo.setTranslationSnap(100);
                    Canvas.transformGizmo.setRotationSnap(THREE.MathUtils.degToRad(15));
                    Canvas.transformGizmo.setScaleSnap(0.25);
                    break;
                case 'KeyW':
                    Canvas.transformGizmo.setMode('translate');
                    break;
                case 'KeyE':
                    Canvas.transformGizmo.setMode('rotate');
                    break;
                case 'KeyR':
                    Canvas.transformGizmo.setMode('scale');
                    break;
                case 'Equal':
                    Canvas.transformGizmo.setSize(Canvas.transformGizmo.size + 0.1);
                    break;
                case 'Minus':
                    Canvas.transformGizmo.setSize(Math.max(Canvas.transformGizmo.size - 0.1, 0.1));
                    break;
                case 'KeyX':
                    Canvas.transformGizmo.showX = !Canvas.transformGizmo.showX;
                    break;
                case 'KeyY':
                    Canvas.transformGizmo.showY = !Canvas.transformGizmo.showY;
                    break;
                case 'KeyZ':
                    Canvas.transformGizmo.showZ = !Canvas.transformGizmo.showZ;
                    break;
                case 'Escape':
                    Canvas.transformGizmo.reset();
                    break;
            }
        }
    });
}
