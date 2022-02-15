import { Canvas } from '../../js/Canvas.js';
import { WS } from '../../js/WS.js';
import { WSTasks } from '../../js/WSTasks.js';
import { Core } from '../../js/Core.js';
import { MouseKey } from '../../js/MouseKey.js';
import { request } from '../../js/request.js';

const pullCurrentViewport = async function () {
    await request("syncViewport", {});
}

const updateViewportTimestamp = async function () {
    MouseKey.strokeTimeStamp = Date.now();
}

const syncViewportSend = async function () {
    if (Core["UUID"]) {
        const targetnEq = (!Canvas.lastControlTarget["value"].equals(Canvas.controls.target));
        const posnEq = (!Canvas.lastCameraPosition["value"].equals(Canvas.camera.position));
        const upnEq = (!Canvas.lastCameraUp["value"].equals(Canvas.camera.up));
        const zoomnEq = (Canvas.lastCameraZoom["value"] != Canvas.camera.zoom);

        const json = {};
        if (targetnEq) {
            json["controls"] = {};
            Canvas.lastControlTarget["value"] = Canvas.controls.target.clone();
            Canvas.lastControlTarget["timestamp"] = MouseKey.strokeTimeStamp;
            json["controls"]["target"] = Canvas.lastControlTarget;
        }
        if (posnEq) {
            json["camera"] = {};
            Canvas.lastCameraPosition["value"] = Canvas.camera.position.clone();
            Canvas.lastCameraPosition["timestamp"] = MouseKey.strokeTimeStamp;
            json["camera"]["position"] = Canvas.lastCameraPosition;
        }
        if (upnEq) {
            if (!json["camera"]) {
                json["camera"] = {};
            }
            Canvas.lastCameraUp["value"] = Canvas.camera.up.clone();
            Canvas.lastCameraUp["timestamp"] = MouseKey.strokeTimeStamp;
            json["camera"]["up"] = Canvas.lastCameraUp;
        }
        if (zoomnEq) {
            if (!json["camera"]) {
                json["camera"] = {};
            }
            Canvas.lastCameraZoom["value"] = { "zoom": Canvas.camera.zoom };
            Canvas.lastCameraZoom["timestamp"] = MouseKey.strokeTimeStamp;
            json["camera"]["zoom"] = Canvas.lastCameraZoom;
        }

        if (targetnEq || posnEq || upnEq || zoomnEq) {
            WS.sendMsg("syncViewport", json);
        }
    }
}

const syncViewportReceive = async function (parameters) {
    ////
    // [IN]
    // parameters = {
    //     "controls": {
    //         "target": {
    //             "value": {
    //                 "x": x-coordinate,
    //                 "y": y-coordinate,
    //                 "z": z-coordinate,
    //             },
    //             "timestamp": timestamp
    //         },
    //     },
    //     "camera": {
    //         "position": {
    //             "value": {
    //                 "x": x-coordinate,
    //                 "y": y-coordinate,
    //                 "z": z-coordinate,
    //             },
    //             "timestamp": timestamp
    //         },
    //         "up": {
    //             "value": {
    //                 "x": x-coordinate,
    //                 "y": y-coordinate,
    //                 "z": z-coordinate,
    //             },
    //             "timestamp": timestamp
    //         },
    //         "zoom": {
    //             "value": {
    //                 "zoom": zoom parameter,
    //             },
    //             "timestamp": timestamp
    //         }
    //     }
    // }

    let needUpdate = false;
    if ("controls" in parameters) {
        if ("target" in parameters["controls"]) {
            if (Canvas.lastControlTarget["timestamp"] <= parameters["controls"]["target"]["timestamp"]) {
                Canvas.controls.target.set(
                    parameters["controls"]["target"]["value"].x,
                    parameters["controls"]["target"]["value"].y,
                    parameters["controls"]["target"]["value"].z);
                Canvas.lastControlTarget["value"] = Canvas.controls.target.clone();
                Canvas.lastControlTarget["timestamp"] = parameters["controls"]["target"]["timestamp"];
                needUpdate = true;
            }
        }
    }
    if ("camera" in parameters) {
        if ("position" in parameters["camera"]) {
            if (Canvas.lastCameraPosition["timestamp"] <= parameters["camera"]["position"]["timestamp"]) {
                Canvas.camera.position.set(
                    parameters["camera"]["position"]["value"].x,
                    parameters["camera"]["position"]["value"].y,
                    parameters["camera"]["position"]["value"].z);
                Canvas.lastCameraPosition["value"] = Canvas.camera.position.clone();
                Canvas.lastCameraPosition["timestamp"] = parameters["camera"]["position"]["timestamp"];
                needUpdate = true;
            }
        }
        if ("up" in parameters["camera"]) {
            if (Canvas.lastCameraUp["timestamp"] <= parameters["camera"]["up"]["timestamp"]) {
                Canvas.camera.up.set(
                    parameters["camera"]["up"]["value"].x,
                    parameters["camera"]["up"]["value"].y,
                    parameters["camera"]["up"]["value"].z);
                Canvas.lastCameraUp["value"] = Canvas.camera.up.clone();
                Canvas.lastCameraUp["timestamp"] = parameters["camera"]["up"]["timestamp"];
                needUpdate = true;
            }
        }
        if ("zoom" in parameters["camera"]) {
            if (Canvas.lastCameraZoom["timestamp"] <= parameters["camera"]["zoom"]["timestamp"]) {
                Canvas.camera.zoom = parameters["camera"]["zoom"]["value"].zoom;
                Canvas.lastCameraZoom["value"] = Canvas.camera.zoom;
                Canvas.lastCameraZoom["timestamp"] = parameters["camera"]["zoom"]["timestamp"];
                needUpdate = true;
            }
        }
    }

    if (needUpdate) {
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

