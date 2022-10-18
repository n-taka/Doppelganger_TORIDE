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

    // update Canvas.boundingSphereVisible
    Canvas.calculateBoundingSphere();
    // change camera position, up, zoom
    if (Canvas.boundingSphereVisible) {
        Canvas.controls.target.copy(Canvas.boundingSphereVisible.center);
        const targetToCamera = directions[direction][0].clone();
        targetToCamera.multiplyScalar(Canvas.boundingSphereVisible.radius * 1.01);
        Canvas.camera.position.copy(targetToCamera);
        Canvas.camera.position.add(Canvas.controls.target);
        Canvas.camera.up.copy(directions[direction][1]);
    } else {
        // we use Canvas.controls.target (because this variable will be set to Zero)
        Canvas.controls.target.sub(Canvas.camera.position);
        const distanceFromCameraToTarget =  Canvas.controls.target.length();
        // set to origin
        Canvas.controls.target.multiplyScalar(0.0);

        const targetToCamera = directions[direction][0].clone();
        targetToCamera.multiplyScalar(distanceFromCameraToTarget);

        Canvas.camera.position.copy(targetToCamera);
        Canvas.camera.position.add(Canvas.controls.target);
        Canvas.camera.up.copy(directions[direction][1]);
    }

    // for updating camera.clippingNear, clippingFar, making sure that whole of the mesh is visible
    Canvas.resetCamera();
}

export const init = async function () {
    Canvas.alignCamera = alignCamera;

    // keyboard event
    document.addEventListener("keyup", (function (e) {
        for (let modal of UI.modalDiv.children) {
            const instance = M.Modal.getInstance(modal);
            if (instance.isOpen) {
                return;
            }
        }
        const keycode = e.code;
        if (keycode == 'Digit0' || keycode == 'NumPad0') {
            Canvas.alignCamera(0 + ((e.altKey) ? 3 : 0));
        }
        else if (keycode == 'Digit1' || keycode == 'NumPad1') {
            Canvas.alignCamera(1 + ((e.altKey) ? 3 : 0));
        }
        // else if (keycode == 'Digit2' || keycode == 'NumPad2') {
        //     Canvas.alignCamera(2 + ((e.altKey) ? 3 : 0));
        // }
    }));

    // FAB (6 elements)
    let addFAB = function (text, dirIndex) {
        const FABLi = document.createElement('li');
        {
            const FABA = document.createElement('a');
            FABA.setAttribute('class', 'btn-floating teal lighten-2')
            FABA.addEventListener('click', () => {
                Canvas.alignCamera(dirIndex);
            });
            FABA.innerText = text;
            FABLi.appendChild(FABA);
        }
        UI.FABUl.appendChild(FABLi);
    }
    addFAB('+X', 4);
    addFAB('-X', 1);
    // addFAB('+Y', 2);
    // addFAB('-Y', 5);
    addFAB('+Z', 0);
    addFAB('-Z', 3);

    // re-initialize FAB
    const elems = document.querySelectorAll('.fixed-action-btn');
    M.FloatingActionButton.init(elems, {
        hoverEnabled: false
    });
}

