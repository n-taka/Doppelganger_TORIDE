import { WSTasks } from '../../js/WSTasks.js';
import { Canvas } from '../../js/Canvas.js';
import { getText } from '../../js/Text.js';
import { request } from '../../js/request.js';
import { constructMeshFromParameters } from '../../js/constructMeshFrom.js';
import { constructMeshLiFromParameters, constructMeshLiFromUUID } from '../../js/constructMeshLiFrom.js';

const text = {
    "Load texture": { "en": "Load texture", "ja": "テクスチャ読み込み" }
};

////
// UI
const generateUI = async function () {
    constructMeshLiFromUUID.handlers.push(
        function (meshUUID, liRoot) {
            const mesh = Canvas.UUIDToMesh[meshUUID];
            const hasUV = (mesh.geometry.hasAttribute('uv') && mesh.geometry.getAttribute('uv').count > 0);

            // for element, we cannot use getElementById ...
            const pButtons = liRoot.querySelector("#buttons_" + meshUUID);
            {
                const a = document.createElement("a");
                a.setAttribute("class", "tooltipped");
                a.setAttribute("data-position", "top");
                a.setAttribute("data-tooltip", getText(text, "Load texture"));
                if (hasUV) {
                    a.addEventListener('click', function (e) {
                        loadTextures(meshUUID);
                        // don't fire click event on the parent (e.g. outlineOnClick)
                        e.stopPropagation();
                    });
                } else {
                    a.setAttribute("style", "pointer-events: none;");
                }
                const instance = M.Tooltip.init(a, {});

                {
                    const i = document.createElement("i");
                    i.innerText = "texture";
                    if (hasUV) {
                        i.setAttribute("class", "material-icons teal-text text-lighten-2");
                    } else {
                        i.setAttribute("class", "material-icons blue-grey-text text-lighten-2");
                    }

                    a.appendChild(i);
                }
                pButtons.appendChild(a);
            }
        }
    );
}

const loadTextures = async function (meshUUID) {
    // http://pirosikick.hateblo.jp/entry/2014/08/11/003235
    if (loadTextures.inputElement) {
        document.body.removeChild(loadTextures.inputElement);
    }

    const inputElement = loadTextures.inputElement = document.createElement('input');

    inputElement.type = 'file';
    inputElement.multiple = 'multiple';
    inputElement.accept = 'image/jpeg,image/png,image/bmp';

    inputElement.style.visibility = 'hidden';
    inputElement.style.position = 'absolute';
    inputElement.style.left = '-9999px';

    document.body.appendChild(inputElement);

    // https://www.html5rocks.com/ja/tutorials/file/dndfiles/
    // https://threejs.org/docs/#examples/loaders/GLTFLoader
    // https://qiita.com/weal/items/1a2af81138cd8f49937d
    inputElement.addEventListener('change', function (e) {
        if (e.target.files.length > 0) {
            for (let file of e.target.files) {
                loadTexture(file, meshUUID);
            }
        }
    }, false);
    inputElement.click();
}

const loadTexture = async function (file, meshUUID) {
    const freader = new FileReader();
    freader.onload = function (event) {
        const fileName = file.name;
        const type = fileName.split('.');
        const base64 = freader.result.substring(freader.result.indexOf(',') + 1);

        const json = {
            "meshUUID": meshUUID,
            "texture": {
                "name": type.slice(0, -1).join("."),
                "file": {
                    "type": type[type.length - 1].toLowerCase(),
                    "base64Str": base64
                }
            }
        };
        request("loadTexture", json);
    };
    freader.readAsDataURL(file);
}

////
// WS API
const handler = async function (parameters) {
    await constructMeshFromParameters(parameters);
    await constructMeshLiFromParameters(parameters);
}

export const init = async function () {
    await generateUI();
    WSTasks["loadTexture"] = handler;
}

