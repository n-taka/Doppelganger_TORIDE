import { request } from '../../js/request.js';
import { constructMeshLiFromUUID } from '../../js/constructMeshLiFrom.js';

const text = {
    "Toggle visibility": { "en": "Toggle visibility", "ja": "表示/非表示" }
};

////
// UI
const generateUI = async function () {
    constructMeshLiFromUUID.handlers.push(
        function (meshUUID, liRoot) {
            // for element, we cannot use getElementById ...
            const iIcon = liRoot.querySelector("#icon_" + meshUUID);
            const parameters = {};
            parameters["meshes"] = [meshUUID];
            return request("meshErrorInfo", parameters).then((response) => {
                const responseJson = JSON.parse(response);
                const withoutError = responseJson["meshes"][meshUUID]["closed"] && responseJson["meshes"][meshUUID]["edgeManifold"] && responseJson["meshes"][meshUUID]["vertexManifold"];
                iIcon.setAttribute("class", (withoutError ? "material-icons teal-text text-lighten-2 circle" : "material-icons orange-text text-lighten-2 circle"));
                iIcon.setAttribute("style", "font-size: 36px; background: rgba(255, 255, 255, 1);");
                iIcon.innerText = (withoutError ? "check_circle" : "warning");
            });
        }
    );
}

export const init = async function () {
    await generateUI();
}

