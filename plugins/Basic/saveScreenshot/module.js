import { UI } from '../../js/UI.js';
import { getText } from '../../js/Text.js';
import { Canvas } from '../../js/Canvas.js';
import { request } from '../../js/request.js';
import { constructMeshLiFromJson } from '../../js/constructMeshLiFrom.js';

const text = {
    "Choose file format to save": { "en": "Choose file format to save", "ja": "保存するフォーマットを選んでください" },
    "Cancel": { "en": "Cancel", "ja": "キャンセル" },
    "Save screenshot": { "en": "Save screenshot", "ja": "スクリーンショットを保存" }
};

const parameters = {};

////
// UI
const generateUI = async function () {
    ////
    // modal
    const modal = document.createElement("div");
    {
        modal.setAttribute("class", "modal");
        // content
        {
            const modalContentDiv = document.createElement('div');
            modalContentDiv.setAttribute("class", "modal-content");
            {
                const heading = document.createElement("h4");
                heading.innerText = getText(text, "Choose file format to save");
                modalContentDiv.appendChild(heading);
            }
            {
                const ul = document.createElement('ul');
                {
                    const li = document.createElement('li');
                    {
                        const divRow = document.createElement('div');
                        divRow.setAttribute('class', 'row');
                        {
                            const jpegDiv = document.createElement("div");
                            jpegDiv.setAttribute("class", "col s6");
                            const jpegBtn = document.createElement("a");
                            jpegBtn.setAttribute("class", "waves-effect waves-light btn");
                            jpegBtn.setAttribute("style", "width: 100%;");
                            jpegBtn.innerText = "JPEG (.jpeg)";
                            jpegBtn.addEventListener('click', function () {
                                saveScreenshot([parameters["meshUUID"]], "jpeg");
                            });
                            jpegDiv.appendChild(jpegBtn);
                            divRow.appendChild(jpegDiv);
                        }
                        {
                            const pngDiv = document.createElement("div");
                            pngDiv.setAttribute("class", "col s6");
                            const pngBtn = document.createElement("a");
                            pngBtn.setAttribute("class", "waves-effect waves-light btn");
                            pngBtn.setAttribute("style", "width: 100%;");
                            pngBtn.innerText = "PNG (.png)";
                            pngBtn.addEventListener('click', function () {
                                saveScreenshot([parameters["meshUUID"]], "png");
                            });
                            pngDiv.appendChild(pngBtn);
                            divRow.appendChild(pngDiv);
                        }
                        li.appendChild(divRow);
                    }
                    ul.appendChild(li);
                }
                modalContentDiv.appendChild(ul);
            }
            modal.appendChild(modalContentDiv);
        }
        // footer
        {
            const modalFooterDiv = document.createElement("div");
            modalFooterDiv.setAttribute("class", "modal-footer");

            {
                const modalFooterCancelA = document.createElement("a");
                modalFooterCancelA.setAttribute("class", "modal-close waves-effect waves-green btn-flat");
                modalFooterCancelA.setAttribute("href", "#!");
                modalFooterCancelA.innerHTML = getText(text, "Cancel");
                modalFooterDiv.appendChild(modalFooterCancelA);
            }
            modal.appendChild(modalFooterDiv);
        }
        UI.modalDiv.appendChild(modal);
    }

    ////
    // button
    constructMeshLiFromJson.handlers.push(
        function (json, liRoot) {
            // for element, we cannot use getElementById ...
            const pButtons = liRoot.querySelector("#buttons_" + json["UUID"]);
            {
                const a = document.createElement("a");
                a.setAttribute("class", "tooltipped");
                a.setAttribute("data-position", "top");
                a.setAttribute("data-tooltip", getText(text, "Save screenshot"));
                a.addEventListener('click', function (e) {
                    parameters["meshUUID"] = json["UUID"];
                    const instance = M.Modal.getInstance(modal);
                    instance.open();
                    // don't fire click event on the parent (e.g. outlineOnClick)
                    e.stopPropagation();
                });
                const instance = M.Tooltip.init(a, {});

                {
                    const i = document.createElement("i");
                    i.setAttribute("class", "material-icons teal-text text-lighten-2");
                    i.innerText = "photo_camera";
                    a.appendChild(i);
                }
                pButtons.appendChild(a);
            }
        }
    );
}

////
// callback
const saveScreenshot = function (visibleMeshUUIDArray, format) {
    const toBlob = function (base64) {
        const bin = atob(base64.replace(/^.*,/, ''));
        const buffer = new Uint8Array(bin.length);
        for (let i = 0; i < bin.length; i++) {
            buffer[i] = bin.charCodeAt(i);
        }
        try {
            const blob = new Blob([buffer.buffer], {});
            return blob;
        } catch (e) {
            return false;
        }
    }

    ////
    // store current settings
    const originalVisibility = {};
    for (let meshUUID in Canvas.UUIDToMesh) {
        originalVisibility[meshUUID] = Canvas.UUIDToMesh[meshUUID].visible;
        Canvas.UUIDToMesh[meshUUID].visible = false;
    }
    for (let meshUUID of visibleMeshUUIDArray) {
        Canvas.UUIDToMesh[meshUUID].visible = true;
    }
    let originalSelectedObjects = [];
    if (Canvas.outlinePass) {
        originalSelectedObjects = Canvas.outlinePass.selectedObjects;
        Canvas.outlinePass.selectedObjects = [];
    }

    // filename
    let screenshotFileName = "screenshot";
    if (visibleMeshUUIDArray.length == 1) {
        screenshotFileName += "_";
        screenshotFileName += Canvas.UUIDToMesh[visibleMeshUUIDArray[0]].name;
    }

    if (format == "png") {
        // temporary change alpha and force render (at least) once
        Canvas.renderer.setClearAlpha(0.0);
    }
    Canvas.effectComposer.render();
    const screenshotDataURL = Canvas.renderer.domElement.toDataURL("image/" + format);
    const base64 = screenshotDataURL.substring(screenshotDataURL.indexOf(',') + 1);

    const json = {
        "screenshot": {
            "name": screenshotFileName,
            "file": {
                "type": format.toLowerCase(),
                "base64Str": base64
            }
        }
    };
    request("saveScreenshot", json).then((response) => {
        const responseJson = JSON.parse(response);
        if ("screenshots" in responseJson) {
            for (let screenshotJson of responseJson["screenshots"]) {
                const fileName = screenshotJson["fileName"] + "." + screenshotJson["format"];
                const base64Str = screenshotJson["base64Str"];

                const file = toBlob(base64Str);
                if (window.navigator.msSaveOrOpenBlob) // IE10+
                    window.navigator.msSaveOrOpenBlob(file, fileName);
                else { // Others
                    const a = document.createElement("a");
                    const url = URL.createObjectURL(file);
                    a.href = url;
                    a.download = fileName;
                    document.body.appendChild(a);
                    a.click();
                    setTimeout(function () {
                        document.body.removeChild(a);
                        window.URL.revokeObjectURL(url);
                    }, 0);
                }
            }
        }
    });

    Canvas.renderer.setClearAlpha(1.0);
    window.URL.revokeObjectURL(screenshotDataURL);
    if (Canvas.outlinePass) {
        Canvas.outlinePass.selectedObjects = originalSelectedObjects;
    }
    for (let meshUUID in Canvas.UUIDToMesh) {
        Canvas.UUIDToMesh[meshUUID].visible = originalVisibility[meshUUID];
    }
}

export const init = async function () {
    await generateUI();
}

