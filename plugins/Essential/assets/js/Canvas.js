import * as THREE from 'https://cdn.skypack.dev/three@v0.132';
import { mergeBufferAttributes } from 'https://cdn.skypack.dev/three@v0.132/examples/jsm/utils/BufferGeometryUtils.js';
import { RenderPass } from 'https://cdn.skypack.dev/three@v0.132/examples/jsm/postprocessing/RenderPass.js';
import { EffectComposer } from 'https://cdn.skypack.dev/three@v0.132/examples/jsm/postprocessing/EffectComposer.js';

import { UI } from './UI.js';
// import { Core } from './Core.js';
// import { WS } from './WS.js';
import { request } from './request.js';
import { MouseKey } from './MouseKey.js';

import { constructMeshFromParameters } from './constructMeshFrom.js';
import { constructMeshLiFromParameters } from './constructMeshLiFrom.js';

import { OrbitControls } from './ThreeOrbitControlsGizmo/OrbitControls.js';
import { OrbitControlsGizmo } from "./ThreeOrbitControlsGizmo/OrbitControlsGizmo.js";

////
// [note]
// The parameters are tweaked to work well
//   with the model whose bounding box is (100, 100, 100)
////

export const Canvas = {};

Canvas.drawLoopTasks = [];

Canvas.init = async function () {
    // default color
    Canvas.defaultColor = [208.0 / 255.0, 208.0 / 255.0, 208.0 / 255.0];

    // width / height
    {
        Canvas.width = UI.webGLDiv.offsetWidth;
        Canvas.height = UI.webGLDiv.offsetHeight;
    }

    // scene
    Canvas.scene = new THREE.Scene();

    // groups
    {
        Canvas.predefGroup = new THREE.Group();
        Canvas.meshGroup = new THREE.Group();
        Canvas.scene.add(Canvas.predefGroup);
        Canvas.scene.add(Canvas.meshGroup);
    }

    // orthographic camera
    {
        Canvas.camera = new THREE.OrthographicCamera(-Canvas.width / 2.0, Canvas.width / 2.0, Canvas.height / 2.0, -Canvas.height / 2.0, 0.0, 2000.0);
        Canvas.predefGroup.add(Canvas.camera);

        Canvas.camera.position.set(-30.0, 40.0, 30.0);
        Canvas.camera.up.set(0.0, 1.0, 0.0);
        Canvas.camera.zoom = 1.0;
        Canvas.camera.updateProjectionMatrix();
    }

    // light (need to tweak??)
    {
        Canvas.ambientLight = new THREE.AmbientLight(0x444444);
        Canvas.predefGroup.add(Canvas.ambientLight);
        Canvas.directionalLights = [];
        Canvas.directionalLights.push(new THREE.DirectionalLight(0xaaaaaa));
        Canvas.directionalLights[0].shadow.mapSize.width = 2048;
        Canvas.directionalLights[0].shadow.mapSize.height = 2048;
        Canvas.directionalLights[0].position.set(20, 20, 20);
        Canvas.directionalLights[0].castShadow = true;
        Canvas.predefGroup.add(Canvas.directionalLights[0]);
        Canvas.directionalLights.push(new THREE.DirectionalLight(0xaaaaaa));
        Canvas.directionalLights[1].shadow.mapSize.width = 2048;
        Canvas.directionalLights[1].shadow.mapSize.height = 2048;
        Canvas.directionalLights[1].position.set(0, -15, -20);
        Canvas.directionalLights[1].castShadow = true;
        Canvas.predefGroup.add(Canvas.directionalLights[1]);
        Canvas.directionalLights.push(new THREE.DirectionalLight(0xaaaaaa));
        Canvas.directionalLights[2].shadow.mapSize.width = 2048;
        Canvas.directionalLights[2].shadow.mapSize.height = 2048;
        Canvas.directionalLights[2].position.set(-25, 20, 0);
        Canvas.directionalLights[2].castShadow = true;
        Canvas.predefGroup.add(Canvas.directionalLights[2]);
    }

    // renderer
    {
        Canvas.renderer = new THREE.WebGLRenderer({ preserveDrawingBuffer: true, alpha: true });
        Canvas.renderer.setSize(Canvas.width, Canvas.height);
        Canvas.renderer.setClearColor(0xeeeeee, 1.0);
        Canvas.renderer.shadowMap.enabled = true;
        UI.webGLOutputDiv.appendChild(this.renderer.domElement);
    }

    // effectComposer
    {
        Canvas.effectComposer = new EffectComposer(Canvas.renderer);
        Canvas.renderPass = new RenderPass(Canvas.scene, Canvas.camera);
        Canvas.effectComposer.addPass(Canvas.renderPass);
    }

    // controls
    {
        Canvas.controls = new OrbitControls(Canvas.camera, Canvas.renderer.domElement);
        Canvas.controls.panSpeed = 0.3 * 5;
        Canvas.controls.target.set(0.0, 0.0, 0.0);

        Canvas.gizmoDiv = document.createElement('div');
        Canvas.gizmoDiv.setAttribute('style', 'position: absolute; right: 500px; top: 0; z-index: 2147483646;');
        UI.webGLDiv.appendChild(Canvas.gizmoDiv);
        Canvas.controlsGizmo = new OrbitControlsGizmo(Canvas.controls, { size: 100, padding: 8 });
        Canvas.gizmoDiv.appendChild(Canvas.controlsGizmo.domElement);

        Canvas.camera.lookAt(Canvas.controls.target.clone());
        Canvas.camera.updateMatrixWorld(true);
    }

    // bounding sphere (for tweaking camera parameters)
    Canvas.boundingSphere = undefined;

    // parameters for UI
    Canvas.lastControlTarget = {};
    Canvas.lastCameraPosition = {};
    Canvas.lastCameraUp = {};
    Canvas.lastCameraZoom = {};
    // for preventing syncing default value on connect
    Canvas.lastControlTarget["value"] = Canvas.controls.target.clone();
    Canvas.lastCameraPosition["value"] = Canvas.camera.position.clone();
    Canvas.lastCameraUp["value"] = Canvas.camera.up.clone();
    Canvas.lastCameraZoom["value"] = { "zoom": Canvas.camera.zoom };

    // parameter for mesh
    Canvas.UUIDToMesh = {};

    // event listener
    window.addEventListener('resize', function () {
        Canvas.width = UI.webGLDiv.offsetWidth;
        Canvas.height = UI.webGLDiv.offsetHeight;

        Canvas.camera.left = -Canvas.width / 2.0;
        Canvas.camera.right = Canvas.width / 2.0;
        Canvas.camera.top = Canvas.height / 2.0;
        Canvas.camera.bottom = -Canvas.height / 2.0;
        Canvas.camera.updateProjectionMatrix();

        Canvas.renderer.setPixelRatio(window.devicePixelRatio);
        Canvas.renderer.setSize(Canvas.width, Canvas.height);
        Canvas.effectComposer.setSize(Canvas.width, Canvas.height);
    });


    Canvas.controls.domElement.addEventListener("pointerup", function (e) {
        Canvas.calculateBoundingSphere();
        Canvas.resetCamera();
    });

    Canvas.drawLoop();
    return;
};

Canvas.drawLoop = async function () {
    Canvas.controls.update();
    // Canvas.pushCanvasParameters();
    for (let handler of Canvas.drawLoopTasks) {
        await handler();
    }
    requestAnimationFrame(Canvas.drawLoop);
    Canvas.effectComposer.render();
};

Canvas.pullCurrentMeshes = async function () {
    const parameters = JSON.parse(await request("pullCurrentMeshes"));
    await constructMeshFromParameters(parameters);
    await constructMeshLiFromParameters(parameters);
}

// calculates bounding sphere for all meshes
Canvas.calculateBoundingSphere = function () {
    Canvas.boundingSphere = undefined;

    const meshList = Canvas.meshGroup.children.filter(function (obj) { return (obj instanceof THREE.Mesh); });
    if (meshList.length > 0) {
        const posAttrib = mergeBufferAttributes(meshList.map(function (obj) {
            const tmpGeometry = obj.geometry.clone();
            tmpGeometry.applyMatrix4(obj.matrixWorld);
            return tmpGeometry.getAttribute("position");
        }));
        const geometry = new THREE.BufferGeometry();
        geometry.setAttribute("position", posAttrib);
        geometry.computeBoundingSphere();
        Canvas.boundingSphere = geometry.boundingSphere.clone();
        geometry.dispose();
    }
};

// update camera/controls parameters
//   + camera.position
//   + camera.near
//   + camera.far
//   + controls.target
//   + controls.panSpeed
Canvas.resetCamera = function () {
    // default values for three js
    let clippingNear = 0.0;
    let clippingFar = 2000.0;
    let panSpeed = 1.0;

    if (Canvas.boundingSphere) {
        MouseKey.lastInteractionTimeStamp = Date.now();

        // re-locate camera so that whole meshes are visible during the rotation
        //   the procedure below also handles the case "controls.target != BSphere.center"
        const targetToCameraUnit = Canvas.camera.position.clone();
        targetToCameraUnit.sub(Canvas.controls.target);
        targetToCameraUnit.normalize();
        const targetToBCenter = Canvas.boundingSphere.center.clone();
        targetToBCenter.sub(Canvas.controls.target);
        const shift = (targetToBCenter.length() + Canvas.boundingSphere.radius) * 1.01;
        const cameraPos = Canvas.controls.target.clone();
        const targetToCamera = targetToCameraUnit.clone();
        targetToCamera.multiplyScalar(shift);
        cameraPos.add(targetToCamera);
        Canvas.camera.position.copy(cameraPos);

        // update pan speed
        panSpeed = 100.0 / targetToCamera.length();

        if (UI.sliderDiv) {
            // plugin crossSectionalView is installed
            const sliderValue = UI.sliderDiv.noUiSlider.get();
            const sliderValueNear = parseFloat(sliderValue[0]);
            const sliderValueFar = parseFloat(sliderValue[1]);

            if (sliderValueNear > 0.0 || sliderValueFar < 100.0) {
                // ranged rendering
                const sliderValueNearRatio = (sliderValueNear - 50.0) / 50.0;
                const sliderValueFarRatio = (sliderValueFar - 50.0) / 50.0;
                const clippingCenter = shift;
                // update near/far clip
                clippingNear = clippingCenter + sliderValueNearRatio * clippingCenter;
                clippingFar = clippingCenter + sliderValueFarRatio * clippingCenter;
            }
        }

    }
    Canvas.camera.near = clippingNear;
    Canvas.camera.far = clippingFar;
    Canvas.controls.panSpeed = panSpeed;
    Canvas.camera.updateProjectionMatrix();
};
