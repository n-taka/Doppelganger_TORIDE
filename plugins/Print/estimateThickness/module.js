import { UI } from '../../js/UI.js';
import { WSTasks } from '../../js/WSTasks.js';
import { getText } from '../../js/Text.js';
import { request } from '../../js/request.js';
import { constructMeshFromParameters } from '../../js/constructMeshFrom.js';
import { constructMeshLiFromParameters, constructMeshLiFromJson } from '../../js/constructMeshLiFrom.js';

const text = {
    "Thickness estimation settings": { "en": "Thickness estimation settings", "ja": "厚み推定設定" },
    "Thickness estimation": { "en": "Thickness estimation", "ja": "厚み推定" },
    "Estimate": { "en": "Estimate", "ja": "推定" },
    "Minimum acceptable thickness [mm]": { "en": "Minimum acceptable thickness [mm]", "ja": "許容できる最小厚み [mm]" },
    "Sample count per vertex": { "en": "Sample count per vertex", "ja": "1頂点あたりのサンプル数" },
    "Cancel": { "en": "Cancel", "ja": "キャンセル" }
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
                heading.innerText = getText(text, "Thickness estimation settings");
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
                            const divCol = document.createElement('div');
                            divCol.setAttribute('class', 'input-field col s6');
                            {
                                const input = document.createElement('input');
                                input.setAttribute('type', 'number');
                                input.setAttribute('id', 'acceptableThickness');
                                input.setAttribute('value', '3.0');
                                input.setAttribute('class', 'validate');
                                input.setAttribute('min', '0.01');
                                input.setAttribute('max', '100.0');
                                input.setAttribute('step', '0.01');
                                input.setAttribute('style', 'text-align: center;');
                                divCol.appendChild(input);
                            }
                            {
                                const label = document.createElement('label');
                                label.setAttribute('for', "acceptableThickness");
                                label.innerText = getText(text, "Minimum acceptable thickness [mm]");
                                divCol.appendChild(label);
                            }
                            divRow.appendChild(divCol);
                        }
                        {
                            const divCol = document.createElement('div');
                            divCol.setAttribute('class', 'input-field col s6');
                            {
                                const input = document.createElement('input');
                                input.setAttribute('type', 'number');
                                input.setAttribute('id', 'sampleCount');
                                input.setAttribute('value', '200');
                                input.setAttribute('class', 'validate');
                                input.setAttribute('min', '1');
                                input.setAttribute('max', '1000');
                                input.setAttribute('step', '1');
                                input.setAttribute('style', 'text-align: center;');
                                divCol.appendChild(input);
                            }
                            {
                                const label = document.createElement('label');
                                label.setAttribute('for', "sampleCount");
                                label.innerText = getText(text, "Sample count per vertex");
                                divCol.appendChild(label);
                            }
                            divRow.appendChild(divCol);
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
                const modalFooterApplyA = document.createElement("a");
                modalFooterApplyA.setAttribute("class", "modal-close waves-effect waves-green btn-flat");
                modalFooterApplyA.setAttribute("href", "#!");
                modalFooterApplyA.innerHTML = getText(text, "Estimate");
                modalFooterApplyA.addEventListener("click", async function () {
                    const sampleCountInput = document.getElementById("sampleCount");
                    let sampleCount = parseInt(sampleCountInput.value);
                    if (Number.isNaN(sampleCount)) {
                        sampleCount = 100;
                        sampleCountInput.value = 100;
                    }
                    const acceptableThicknessInput = document.getElementById("acceptableThickness");
                    let acceptableThickness = parseFloat(acceptableThicknessInput.value);
                    if (Number.isNaN(acceptableThickness)) {
                        acceptableThickness = 3.0;
                        acceptableThicknessInput.value = 3.0;
                    }
                    parameters["sampleCount"] = sampleCount;
                    parameters["acceptableThickness"] = acceptableThickness;
                    request("estimateThickness", parameters);
                });
                modalFooterDiv.appendChild(modalFooterApplyA);
            }
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
    // handler
    constructMeshLiFromJson.handlers.push(
        function (json, liRoot) {
            // for element, we cannot use getElementById ...
            const pButtons = liRoot.querySelector("#buttons_" + json["UUID"]);
            {
                const a = document.createElement("a");
                a.setAttribute("class", "tooltipped");
                a.setAttribute("data-position", "top");
                a.setAttribute("data-tooltip", getText(text, "Thickness estimation"));
                a.addEventListener('click', function (e) {
                    parameters["meshes"] = [json["UUID"]];
                    const instance = M.Modal.getInstance(modal);
                    instance.open();
                    // don't fire click event on the parent (e.g. outlineOnClick)
                    e.stopPropagation();
                });
                const instance = M.Tooltip.init(a, {});

                {
                    const i = document.createElement("i");
                    i.setAttribute("class", "material-icons teal-text text-lighten-2");
                    i.innerText = "vertical_align_center";
                    a.appendChild(i);
                }
                pButtons.appendChild(a);
            }
        }
    );
}

////
// WS API
const estimateThickness = async function (parameters) {
    await constructMeshFromParameters(parameters);
    await constructMeshLiFromParameters(parameters);
}

export const init = async function () {
    await generateUI();
    WSTasks["estimateThickness"] = estimateThickness;
}

