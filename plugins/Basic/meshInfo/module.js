import { Canvas } from '../../js/Canvas.js';
import { getText } from '../../js/Text.js';
import { constructMeshLiFromUUID } from '../../js/constructMeshLiFrom.js';

const text = {
    "#Triangle": { "en": "#Triangle", "ja": "ポリゴン数" }
};

////
// UI
const generateUI = async function () {
    constructMeshLiFromUUID.handlers.push(
        function (meshUUID, liRoot) {
            // for element, we cannot use getElementById ...
            const divMetaInfo = liRoot.querySelector("#metaInfo_" + meshUUID);
            {
                if (divMetaInfo.innerHTML == "<p><br></p>") {
                    // if only dummy element is present, we remove it.
                    divMetaInfo.innerText = '';
                }
                const mesh = Canvas.UUIDToMesh[meshUUID];

                // size info
                // here, we only show XX.XX
                {
                    const tmpGeometry = mesh.geometry.clone();
                    tmpGeometry.applyMatrix4(mesh.matrixWorld);
                    tmpGeometry.computeBoundingBox();
                    const BBox = tmpGeometry.boundingBox;
                    const XYZSize = BBox.max.clone();
                    XYZSize.sub(BBox.min);

                    const pSize = document.createElement("p");
                    pSize.setAttribute("style", "text-align: left; float: left;");
                    {
                        const xSize = document.createElement("span");
                        xSize.setAttribute("style", "background: linear-gradient(transparent 75%, #ff8080 75%);");
                        xSize.innerText = XYZSize.x.toFixed(2);
                        pSize.appendChild(xSize);
                    }
                    {
                        const x = document.createElement("span");
                        x.innerText = " × ";
                        pSize.appendChild(x);
                    }
                    {
                        const ySize = document.createElement("span");
                        ySize.setAttribute("style", "background: linear-gradient(transparent 75%, #80ff80 75%);");
                        ySize.innerText = XYZSize.y.toFixed(2);
                        pSize.appendChild(ySize);
                    }
                    {
                        const x = document.createElement("span");
                        x.innerText = " × ";
                        pSize.appendChild(x);
                    }
                    {
                        const zSize = document.createElement("span");
                        zSize.setAttribute("style", "background: linear-gradient(transparent 75%, #8080ff 75%);");
                        zSize.innerText = XYZSize.z.toFixed(2);
                        pSize.appendChild(zSize);
                    }

                    divMetaInfo.appendChild(pSize);
                }

                // vertex / triangle count
                {
                    const pCount = document.createElement("p");
                    pCount.setAttribute("style", "text-align: right;");
                    // const vertexCount = mesh.geometry.getAttribute('position').count;
                    const triangleCount = mesh.geometry.index.count / 3;
                    const infoString = getText(text, "#Triangle") + ": " + triangleCount;
                    pCount.innerText = infoString;
                    divMetaInfo.appendChild(pCount);
                }
            }
        }
    );
}

export const init = async function () {
    await generateUI();
}

