import { WSTasks } from '../../js/WSTasks.js';
import { getText } from '../../js/Text.js';
import { request } from '../../js/request.js';
import { Canvas } from '../../js/Canvas.js';
import { constructMeshFromParameters } from '../../js/constructMeshFrom.js';
import { constructMeshLiFromParameters, constructMeshLiFromUUID } from '../../js/constructMeshLiFrom.js';

const text = {
    "Toggle visibility": { "en": "Toggle visibility", "ja": "表示/非表示" }
};

////
// UI
const generateUI = async function () {
    constructMeshLiFromUUID.handlers.push(
        function (meshUUID, liRoot) {
            // for element, we cannot use getElementById ...
            const pButtons = liRoot.querySelector("#buttons_" + meshUUID);
            {
                const a = document.createElement("a");
                a.setAttribute("class", "tooltipped");
                a.setAttribute("data-position", "top");
                a.setAttribute("data-tooltip", getText(text, "Toggle visibility"));
                a.addEventListener('click', function (e) {
                    const parameters = {};
                    parameters["meshes"] = [meshUUID];
                    request("toggleMeshVisibility", parameters);
                    // don't fire click event on the parent (e.g. outlineOnClick)
                    e.stopPropagation();
                });
                const instance = M.Tooltip.init(a, {});
                
                {
                    const i = document.createElement("i");
                    i.setAttribute("class", "material-icons teal-text text-lighten-2");
                    const mesh = Canvas.UUIDToMesh[meshUUID];
                    i.innerText = (mesh.visibility ? "visibility" : "visibility_off");
                    a.appendChild(i);
                }
                pButtons.appendChild(a);
            }
        }
    );
}

////
// WS API
const toggleMeshVisibility = async function (parameters) {
    await constructMeshFromParameters(parameters);
    await constructMeshLiFromParameters(parameters);
}

export const init = async function () {
    await generateUI();
    WSTasks["toggleMeshVisibility"] = toggleMeshVisibility;
}

