import * as THREE from 'https://cdn.skypack.dev/three@v0.132';

export const MouseKey = {};

MouseKey.init = async function () {
    ////
    // icon
    MouseKey.iconIdx = Math.floor(Math.random() * 10);
    document.body.style.cursor = "url(../icon/cursorIcon" + MouseKey.iconIdx + ".png) 16 16 , default"

    // initialize timestamp
    MouseKey.strokeTimeStamp = Date.now();

    ////
    // pointer event
    // canvas.controls.domElement.addEventListener?
    // disable right click
    document.addEventListener("contextmenu", function (e) { });

    return;
};

