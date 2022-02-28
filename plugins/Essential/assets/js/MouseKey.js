export const MouseKey = {};

MouseKey.init = async function () {
    ////
    // icon
    MouseKey.iconIdx = Math.floor(Math.random() * 10);
    document.body.style.cursor = "url(../icon/cursorIcon" + MouseKey.iconIdx + ".png) 16 16 , default"

    // initialize timestamp (unix time)
    MouseKey.lastInteractionTimeStamp = -1;
    MouseKey.activeInteractionTimeStamp = 0;

    ////
    // pointer event
    // canvas.controls.domElement.addEventListener?
    // disable right click
    document.addEventListener("contextmenu", function (e) { });

    return;
};
