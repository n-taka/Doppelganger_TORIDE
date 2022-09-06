import { UI } from '../../js/UI.js';
import { getText } from '../../js/Text.js';
import { Canvas } from '../../js/Canvas.js';
import { request } from '../../js/request.js';

const text = {
    "Choose file format to save": { "en": "Choose file format to save", "ja": "保存するフォーマットを選んでください" },
    "Cancel": { "en": "Cancel", "ja": "キャンセル" },
    "Save screenshot as single file. (for PSD, each meshes are rendered in separate layers)": { "en": "Save screenshot as single file. (for PSD, each meshes are rendered in separate layers)", "ja": "単一の画像にまとめてレンダリングします。 (PSDファイルの場合、それぞれのメッシュは異なるレイヤーに書き出されます)" },
    "Save screenshot": { "en": "Save screenshot", "ja": "スクリーンショットを保存" }
};

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
                            jpegDiv.setAttribute("class", "col s2 offset-s1");
                            const jpegBtn = document.createElement("a");
                            jpegBtn.setAttribute("class", "waves-effect waves-light btn");
                            jpegBtn.setAttribute("style", "width: 100%;");
                            jpegBtn.innerText = "JPEG";
                            jpegBtn.addEventListener('click', function () {
                                const meshUUIDToBeRendered = [];
                                for (let meshUUID of Object.keys(Canvas.UUIDToMesh)) {
                                    if (Canvas.UUIDToMesh[meshUUID].visible) {
                                        meshUUIDToBeRendered.push(meshUUID);
                                    }
                                }
                                saveScreenshot(meshUUIDToBeRendered, "jpeg", document.getElementById('singleFile').checked);
                            });
                            jpegDiv.appendChild(jpegBtn);
                            divRow.appendChild(jpegDiv);
                        }
                        {
                            const pngDiv = document.createElement("div");
                            pngDiv.setAttribute("class", "col s2");
                            const pngBtn = document.createElement("a");
                            pngBtn.setAttribute("class", "waves-effect waves-light btn");
                            pngBtn.setAttribute("style", "width: 100%;");
                            pngBtn.innerText = "PNG";
                            pngBtn.addEventListener('click', function () {
                                const meshUUIDToBeRendered = [];
                                for (let meshUUID of Object.keys(Canvas.UUIDToMesh)) {
                                    if (Canvas.UUIDToMesh[meshUUID].visible) {
                                        meshUUIDToBeRendered.push(meshUUID);
                                    }
                                }
                                saveScreenshot(meshUUIDToBeRendered, "png", document.getElementById('singleFile').checked);
                            });
                            pngDiv.appendChild(pngBtn);
                            divRow.appendChild(pngDiv);
                        }
                        {
                            const bmpDiv = document.createElement("div");
                            bmpDiv.setAttribute("class", "col s2");
                            const bmpBtn = document.createElement("a");
                            bmpBtn.setAttribute("class", "waves-effect waves-light btn");
                            bmpBtn.setAttribute("style", "width: 100%;");
                            bmpBtn.innerText = "BMP";
                            bmpBtn.addEventListener('click', function () {
                                const meshUUIDToBeRendered = [];
                                for (let meshUUID of Object.keys(Canvas.UUIDToMesh)) {
                                    if (Canvas.UUIDToMesh[meshUUID].visible) {
                                        meshUUIDToBeRendered.push(meshUUID);
                                    }
                                }
                                saveScreenshot(meshUUIDToBeRendered, "bmp", document.getElementById('singleFile').checked);
                            });
                            bmpDiv.appendChild(bmpBtn);
                            divRow.appendChild(bmpDiv);
                        }
                        {
                            const tgaDiv = document.createElement("div");
                            tgaDiv.setAttribute("class", "col s2");
                            const tgaBtn = document.createElement("a");
                            tgaBtn.setAttribute("class", "waves-effect waves-light btn");
                            tgaBtn.setAttribute("style", "width: 100%;");
                            tgaBtn.innerText = "TGA";
                            tgaBtn.addEventListener('click', function () {
                                const meshUUIDToBeRendered = [];
                                for (let meshUUID of Object.keys(Canvas.UUIDToMesh)) {
                                    if (Canvas.UUIDToMesh[meshUUID].visible) {
                                        meshUUIDToBeRendered.push(meshUUID);
                                    }
                                }
                                saveScreenshot(meshUUIDToBeRendered, "tga", document.getElementById('singleFile').checked);
                            });
                            tgaDiv.appendChild(tgaBtn);
                            divRow.appendChild(tgaDiv);
                        }
                        {
                            const psdDiv = document.createElement("div");
                            psdDiv.setAttribute("class", "col s2");
                            const psdBtn = document.createElement("a");
                            psdBtn.setAttribute("class", "waves-effect waves-light btn");
                            psdBtn.setAttribute("style", "width: 100%;");
                            psdBtn.innerText = "PSD";
                            psdBtn.addEventListener('click', function () {
                                const meshUUIDToBeRendered = [];
                                for (let meshUUID of Object.keys(Canvas.UUIDToMesh)) {
                                    if (Canvas.UUIDToMesh[meshUUID].visible) {
                                        meshUUIDToBeRendered.push(meshUUID);
                                    }
                                }
                                saveScreenshot(meshUUIDToBeRendered, "psd", document.getElementById('singleFile').checked);
                            });
                            psdDiv.appendChild(psdBtn);
                            divRow.appendChild(psdDiv);
                        }
                        li.appendChild(divRow);
                    }
                    ul.appendChild(li);
                }
                {
                    const liCheckbox = document.createElement("li");
                    {
                        const pCheckbox = document.createElement("p");
                        {
                            const labelCheckbox = document.createElement("label");
                            {
                                const inputCheckbox = document.createElement("input");
                                inputCheckbox.setAttribute("type", "checkbox")
                                inputCheckbox.setAttribute("id", "singleFile");
                                inputCheckbox.checked = true;
                                labelCheckbox.appendChild(inputCheckbox);
                            }
                            {
                                const spanCheckbox = document.createElement("span");
                                spanCheckbox.innerText = getText(text, "Save screenshot as single file. (for PSD, each meshes are rendered in separate layers)");
                                labelCheckbox.appendChild(spanCheckbox);
                            }
                            pCheckbox.appendChild(labelCheckbox);
                        }

                        liCheckbox.appendChild(pCheckbox);
                    }
                    ul.appendChild(liCheckbox);
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
    {
        const li = document.createElement("li");
        {
            const a = document.createElement("a");
            a.addEventListener('click', function () {
                const instance = M.Modal.getInstance(modal);
                instance.open();
            });
            a.setAttribute("class", "tooltipped");
            a.setAttribute("data-position", "top");
            a.setAttribute("data-tooltip", getText(text, "Save screenshot"));
            {
                const i = document.createElement("i");
                i.innerText = "photo_camera";
                i.setAttribute("class", "material-icons");
                a.appendChild(i);
            }
            li.appendChild(a);
        }
        UI.bottomMenuLeftUl.appendChild(li);
    }
}

////
// callback
const saveScreenshot = function (visibleMeshUUIDArray, format, mergedImages) {
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

    const captureCurrentView = function (meshName) {
        Canvas.renderer.setClearAlpha(0.0);
        Canvas.effectComposer.render();
        const screenshotDataURL = Canvas.renderer.domElement.toDataURL("image/png");
        const base64 = screenshotDataURL.substring(screenshotDataURL.indexOf(',') + 1);
        Canvas.renderer.setClearAlpha(1.0);
        window.URL.revokeObjectURL(screenshotDataURL);
        return {
            "name": meshName,
            "format": "png",
            "base64Str": base64
        };
    }

    ////
    // store current settings
    const originalVisibility = {};
    for (let meshUUID in Canvas.UUIDToMesh) {
        originalVisibility[meshUUID] = Canvas.UUIDToMesh[meshUUID].visible;
        Canvas.UUIDToMesh[meshUUID].visible = false;
    }
    let originalSelectedObjects = [];
    if (Canvas.outlinePass) {
        originalSelectedObjects = Canvas.outlinePass.selectedObjects;
        Canvas.outlinePass.selectedObjects = [];
    }

    ////
    // prepare json
    const json = {
        "screenshots": {
            "name": "screenshot",
            "saveFileFormat": format,
            "imageAsLayers": mergedImages,
            "images": []
        }
    };
    if (mergedImages && format != "psd") {
        for (let meshUUID of visibleMeshUUIDArray) {
            Canvas.UUIDToMesh[meshUUID].visible = true;
        }
        json["screenshots"]["images"].push(captureCurrentView("screenshot_all"));
    } else if (mergedImages && format == "psd") {
        // merged(layers) for psd
        //   sort visibleMeshUUIDArray based on the "depth" of the bounding sphere
        const visibleMeshUUIDToBSphereCenterDepth = {};
        for (let meshUUID of visibleMeshUUIDArray) {
            const mesh = Canvas.UUIDToMesh[meshUUID];
            const tmpGeometry = mesh.geometry.clone();
            tmpGeometry.applyMatrix4(mesh.matrixWorld);
            tmpGeometry.computeBoundingSphere();

            const cameraToBSphereCenter = tmpGeometry.boundingSphere.center;
            cameraToBSphereCenter.sub(Canvas.camera.position);

            const cameraToTargetUnit = Canvas.controls.target.clone();
            cameraToTargetUnit.sub(Canvas.camera.position);
            cameraToTargetUnit.normalize();
    
            visibleMeshUUIDToBSphereCenterDepth[meshUUID] = cameraToBSphereCenter.dot(cameraToTargetUnit);
        }

        // deeper comes first
        visibleMeshUUIDArray.sort((a, b) => visibleMeshUUIDToBSphereCenterDepth[b] - visibleMeshUUIDToBSphereCenterDepth[a]);

        for (let meshUUID of visibleMeshUUIDArray) {
            Canvas.UUIDToMesh[meshUUID].visible = true;
            json["screenshots"]["images"].push(captureCurrentView(Canvas.UUIDToMesh[meshUUID].name));
            Canvas.UUIDToMesh[meshUUID].visible = false;
        }
    } else {
        // per-mesh rendering
        for (let meshUUID of visibleMeshUUIDArray) {
            Canvas.UUIDToMesh[meshUUID].visible = true;
            json["screenshots"]["images"].push(captureCurrentView("screenshot_" + Canvas.UUIDToMesh[meshUUID].name));
            Canvas.UUIDToMesh[meshUUID].visible = false;
        }
    }

    request("saveScreenshotAll", json).then((response) => {
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

    ////
    // restore settings
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

