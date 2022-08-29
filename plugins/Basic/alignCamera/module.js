import * as THREE from 'https://cdn.skypack.dev/three@v0.132';
import { Canvas } from '../../js/Canvas.js';
import { UI } from '../../js/UI.js';

const alignCamera = function (direction) {
    // directions[X][0]: camera position based from the origin
    // directions[X][1]: camera up direction
    const directions = [
        [new THREE.Vector3(0, 0, 1), new THREE.Vector3(0, 1, 0)],
        [new THREE.Vector3(-1, 0, 0), new THREE.Vector3(0, 1, 0)],
        [new THREE.Vector3(0, 1, 0), new THREE.Vector3(0, 0, -1)],
        [new THREE.Vector3(0, 0, -1), new THREE.Vector3(0, 1, 0)],
        [new THREE.Vector3(1, 0, 0), new THREE.Vector3(0, 1, 0)],
        [new THREE.Vector3(0, -1, 0), new THREE.Vector3(0, 0, 1)]
    ];

    // update Canvas.boundingSphere
    Canvas.calculateBoundingSphere();
    // change camera position, up, zoom
    Canvas.controls.target.copy(Canvas.boundingSphere.center);
    const targetToCamera = directions[direction][0].clone();
    targetToCamera.multiplyScalar(Canvas.boundingSphere.radius * 1.01);
    Canvas.camera.position.copy(targetToCamera);
    Canvas.camera.position.add(Canvas.controls.target);
    Canvas.camera.up.copy(directions[direction][1]);

    // for updating camera.clippingNear, clippingFar, making sure that whole of the mesh is visible
    Canvas.resetCamera();
}

export const init = async function () {
    Canvas.alignCamera = alignCamera;

    // keyboard event
    document.addEventListener("keyup", (function (e) {
        for (let modal of UI.modalDiv.children) {
            const instance = M.Modal.getInstance(modal);
            if(instance.isOpen){
                return;
            }
        }
        const keycode = e.code;
        if (keycode == 'Digit0' || keycode == 'NumPad0') {
            alignCamera(0 + ((e.altKey) ? 3 : 0));
        }
        else if (keycode == 'Digit1' || keycode == 'NumPad1') {
            alignCamera(1 + ((e.altKey) ? 3 : 0));
        }
        else if (keycode == 'Digit2' || keycode == 'NumPad2') {
            alignCamera(2 + ((e.altKey) ? 3 : 0));
        }
    }));
}

