import * as THREE from 'https://cdn.skypack.dev/three@v0.132';
import { TrackballControls } from 'https://cdn.skypack.dev/three@v0.132/examples/jsm/controls/TrackballControls.js';
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
        Canvas.controls = new TrackballControls(Canvas.camera, Canvas.renderer.domElement);
        Canvas.controls.panSpeed = 0.3 * 5;
        Canvas.controls.target.set(0.0, 0.0, 0.0);
        Canvas.camera.lookAt(Canvas.controls.target.clone());
        Canvas.camera.updateMatrixWorld(true);
    }

    // bounding sphere (for tweaking camera parameters)
    Canvas.unifiedBSphere = new THREE.Sphere(new THREE.Vector3(0, 0, 0), 1.0);

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

Canvas.resetCamera = function (refreshBSphere) {
    let clippingNear = -1.01;
    let clippingFar = 1.01;
    if (UI.sliderDiv) {
        const sliderValue = UI.sliderDiv.noUiSlider.get();
        clippingNear = (parseFloat(sliderValue[0]) - 50.0) / 50.0;
        clippingFar = (parseFloat(sliderValue[1]) - 50.0) / 50.0;
    }

    MouseKey.lastInteractionTimeStamp = Date.now();

    const meshList = Canvas.meshGroup.children.filter(function (obj) { return (obj instanceof THREE.Mesh); });
    if (meshList.length > 0) {

        if (refreshBSphere) {
            const posAttrib = mergeBufferAttributes(meshList.map(function (obj) { return obj.geometry.getAttribute("position"); }));
            const geometry = new THREE.BufferGeometry();
            geometry.setAttribute("position", posAttrib);
            geometry.computeBoundingSphere();
            Canvas.unifiedBSphere = geometry.boundingSphere.clone();
            geometry.dispose();
        }

        // the procedure below also handles the case "controls.target != BSphere.center"
        const targetToCamera = Canvas.camera.position.clone();
        targetToCamera.sub(Canvas.controls.target);
        targetToCamera.normalize();

        const targetToBCenter = Canvas.unifiedBSphere.center.clone();
        targetToBCenter.sub(Canvas.controls.target);
        const shift = (targetToBCenter.length() + Canvas.unifiedBSphere.radius) * 1.01;
        const cameraPos = Canvas.controls.target.clone();
        targetToCamera.multiplyScalar(shift);
        cameraPos.add(targetToCamera);
        Canvas.camera.position.copy(cameraPos);

        // update pan speed
        Canvas.controls.panSpeed = 100.0 / targetToCamera.length();

        // update near/far clip
        const cameraToBCenter = Canvas.unifiedBSphere.center.clone();
        cameraToBCenter.sub(Canvas.camera.position);
        const cameraToTarget = Canvas.controls.target.clone();
        cameraToTarget.sub(Canvas.camera.position);
        cameraToTarget.normalize();
        Canvas.camera.near = cameraToBCenter.dot(cameraToTarget) + Canvas.unifiedBSphere.radius * clippingNear;
        Canvas.camera.far = cameraToBCenter.dot(cameraToTarget) + Canvas.unifiedBSphere.radius * clippingFar;
        Canvas.camera.updateProjectionMatrix();
    }
};
