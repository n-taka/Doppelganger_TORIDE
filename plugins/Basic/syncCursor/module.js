import * as THREE from 'https://cdn.skypack.dev/three@v0.132';
import { WS } from '../../js/WS.js';
import { WSTasks } from '../../js/WSTasks.js';
import { Core } from '../../js/Core.js';
import { Canvas } from '../../js/Canvas.js';
import { MouseKey } from '../../js/MouseKey.js';
import { request, beacon } from '../../js/request.js';

const pullCurrentCursors = async function () {
    await request("syncCursor", {});
}

const updateCursor = async function (e) {
    if (Core["UUID"]) {
        const mouse = new THREE.Vector2();
        const bounding = Canvas.controls.domElement.getBoundingClientRect();
        mouse.x = e.clientX - bounding.left - Canvas.width / 2.0;
        mouse.y = e.clientY - bounding.top - Canvas.height / 2.0;
        const cursorInfo = {
            "icon": MouseKey.iconIdx,
            "dir": mouse
        };
        MouseKey["cursors"][Core["UUID"]] = cursorInfo;
    }
}

const unloadCursor = function () {
    const json = {};
    json[Core["UUID"]] = null;

    beacon("syncCursor", json);
}

const syncCursorSend = async function () {
    const cursorNEq = (Core["UUID"]
        && MouseKey["cursors"][Core["UUID"]]
        && !MouseKey["prevCursor"].equals(MouseKey["cursors"][Core["UUID"]]["dir"]));

    if (cursorNEq) {
        const json = {};
        json[Core["UUID"]] = {
            "dir": {
                "x": MouseKey["cursors"][Core["UUID"]]["dir"].x,
                "y": MouseKey["cursors"][Core["UUID"]]["dir"].y
            },
            "icon": MouseKey["cursors"][Core["UUID"]]["icon"]
        };
        MouseKey["prevCursor"] = MouseKey["cursors"][Core["UUID"]]["dir"].clone();
        WS.sendMsg("syncCursor", json);
    }
}

const syncCursorReceive = async function (parameters) {
    ////
    // [IN]
    // parameters = {
    //     "<sessionUUID>": {
    //         "dir": {
    //             "x": x corrdinate of this cursor,
    //             "y": y corrdinate of this cursor
    //         },
    //         "icon": idx for cursor icon
    //     }
    // }

    for (let uuid of Object.keys(parameters)) {
        let cursorInfo = parameters[uuid];
        if (cursorInfo == null) {
            // remove
            document.body.removeChild(MouseKey["cursors"][uuid].img);
            delete MouseKey["cursors"][uuid];
        } else {
            const x = cursorInfo["dir"]["x"];
            const y = cursorInfo["dir"]["y"];
            const icon = cursorInfo["icon"];

            if (!MouseKey["cursors"][uuid]) {
                // new entry
                MouseKey["cursors"][uuid] = { "dir": new THREE.Vector2(x, y), "icon": icon, "img": new Image() };
                const style = MouseKey["cursors"][uuid].img.style;
                style.position = "fixed";
                style["z-index"] = "1000"; // material css sidenav has 999
                style["pointer-events"] = "none";
                MouseKey["cursors"][uuid].img.src = "../icon/cursorIcon" + (icon % 10) + ".png";

                document.body.appendChild(MouseKey["cursors"][uuid].img);
            }
            MouseKey["cursors"][uuid]["dir"].set(x, y);

            const bounding = Canvas.controls.domElement.getBoundingClientRect();
            const clientX = x + bounding.left + Canvas.width / 2.0;
            const clientY = y + bounding.top + Canvas.height / 2.0;
            MouseKey["cursors"][uuid].img.style.left = (clientX - 16) + "px";
            MouseKey["cursors"][uuid].img.style.top = (clientY - 16) + "px";
        }
    }
}

export const init = async function () {
    MouseKey["cursors"] = {};
    MouseKey["prevCursor"] = new THREE.Vector2(-1.0, -1.0);
    // we first updateCursor, then syncCursor
    Canvas.controls.domElement.addEventListener("pointermove", function (e) { updateCursor(e) });
    Canvas.controls.domElement.addEventListener("pointermove", function (e) { syncCursorSend() });
    // remove cursor
    Canvas.controls.domElement.addEventListener("pointerleave", function (e) { unloadCursor() });
    //     we want to fire this function in the early stage. useCapture
    window.addEventListener("unload", function (e) { unloadCursor() });

    WSTasks["syncCursor"] = syncCursorReceive;

    await pullCurrentCursors();
}

