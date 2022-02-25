import { Canvas } from '../../js/Canvas.js';
import { WS } from '../../js/WS.js';
import { WSTasks } from '../../js/WSTasks.js';
import { Core } from '../../js/Core.js';
import { MouseKey } from '../../js/MouseKey.js';
import { request } from '../../js/request.js';

////
// parameters = {
//     "timestamp": timestamp,
//     "controls": {
//         "target": {
//             "value": {
//                 "x": x-coordinate,
//                 "y": y-coordinate,
//                 "z": z-coordinate,
//             }
//         }
//     },
//     "camera": {
//         "position": {
//             "value": {
//                 "x": x-coordinate,
//                 "y": y-coordinate,
//                 "z": z-coordinate,
//             }
//         },
//         "up": {
//             "value": {
//                 "x": x-coordinate,
//                 "y": y-coordinate,
//                 "z": z-coordinate,
//             }
//         },
//         "zoom": {
//             "value": {
//                 "zoom": zoom parameter,
//             }
//         }
//     }
// }

const pullCurrentViewport = async function () {
    await request("syncViewport", {});
}

const updateViewportTimestamp = async function () {
    MouseKey.lastInteractionTimeStamp = Date.now();
}

const syncViewportSend = async function () {
    if (Core["UUID"]) {
        const targetnEq = (!Canvas.lastControlTarget["value"].equals(Canvas.controls.target));
        const posnEq = (!Canvas.lastCameraPosition["value"].equals(Canvas.camera.position));
        const upnEq = (!Canvas.lastCameraUp["value"].equals(Canvas.camera.up));
        const zoomnEq = (Canvas.lastCameraZoom["value"]["zoom"] != Canvas.camera.zoom);

        if (targetnEq || posnEq || upnEq || zoomnEq) {
            if (MouseKey.activeInteractionTimeStamp <= MouseKey.lastInteractionTimeStamp) {
                MouseKey.activeInteractionTimeStamp = MouseKey.lastInteractionTimeStamp;
                const json = {};
                // timestamp
                json["timestamp"] = MouseKey.lastInteractionTimeStamp;
                // controls
                json["controls"] = {};
                // controls/target
                Canvas.lastControlTarget["value"] = Canvas.controls.target.clone();
                json["controls"]["target"] = Canvas.lastControlTarget;
                // camera
                json["camera"] = {};
                // camera/position
                Canvas.lastCameraPosition["value"] = Canvas.camera.position.clone();
                json["camera"]["position"] = Canvas.lastCameraPosition;
                // camera/up
                Canvas.lastCameraUp["value"] = Canvas.camera.up.clone();
                json["camera"]["up"] = Canvas.lastCameraUp;
                // camera/zoom
                Canvas.lastCameraZoom["value"] = { "zoom": Canvas.camera.zoom };
                json["camera"]["zoom"] = Canvas.lastCameraZoom;

                WS.sendMsg("syncViewport", json);
            }
        }
    }
}

const syncViewportReceive = async function (parameters) {
    if (MouseKey.activeInteractionTimeStamp <= parameters["timestamp"]) {
        // timestamp
        MouseKey.activeInteractionTimeStamp = parameters["timestamp"];
        // controls/target
        Canvas.controls.target.set(
            parameters["controls"]["target"]["value"].x,
            parameters["controls"]["target"]["value"].y,
            parameters["controls"]["target"]["value"].z);
        Canvas.lastControlTarget["value"] = Canvas.controls.target.clone();
        // camera/position
        Canvas.camera.position.set(
            parameters["camera"]["position"]["value"].x,
            parameters["camera"]["position"]["value"].y,
            parameters["camera"]["position"]["value"].z);
        Canvas.lastCameraPosition["value"] = Canvas.camera.position.clone();
        // camera/up
        Canvas.camera.up.set(
            parameters["camera"]["up"]["value"].x,
            parameters["camera"]["up"]["value"].y,
            parameters["camera"]["up"]["value"].z);
        Canvas.lastCameraUp["value"] = Canvas.camera.up.clone();
        // camera/zoom
        Canvas.camera.zoom = parameters["camera"]["zoom"]["value"].zoom;
        Canvas.lastCameraZoom["value"]["zoom"] = Canvas.camera.zoom;

        // update
        Canvas.camera.updateProjectionMatrix();
        Canvas.camera.lookAt(Canvas.controls.target.clone());
        Canvas.camera.updateMatrixWorld(true);
    }
}

export const init = async function () {
    // update strokeTimeStamp on pointerDown
    document.addEventListener("pointerdown", function (e) { updateViewportTimestamp() });
    document.addEventListener("wheel", function (e) { updateViewportTimestamp() });

    Canvas.drawLoopTasks.push(syncViewportSend);

    WSTasks["syncViewport"] = syncViewportReceive;

    await pullCurrentViewport();
}
