import { WSTasks } from '../../js/WSTasks.js';
import { WS } from '../../js/WS.js';
import { UI } from '../../js/UI.js';
import { getText } from '../../js/Text.js';
import { request } from "../../js/request.js";

const text = {
    "Are you OK?": { "en": "Are you OK?", "ja": "終了しますか？" },
    "Remove log": { "en": "Remove log", "ja": "ログを削除する" },
    "Remove output": { "en": "Remove output", "ja": "出力ファイルを削除する" },
    "If you are OK, please click shutdown.": { "en": "If you are OK, please click shutdown.", "ja": "終了させる場合は、シャットダウンをクリックしてください。" },
    "Shutdown": { "en": "Shutdown", "ja": "シャットダウン" },
    "Cancel": { "en": "Cancel", "ja": "キャンセル" }
};

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
                heading.innerText = getText(text, "Are you OK?");
                modalContentDiv.appendChild(heading);
            }
            {
                const p = document.createElement('p');
                modalContentDiv.appendChild(p);
                p.innerHTML = getText(text, "If you are OK, please click shutdown.");
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
                modalFooterApplyA.innerHTML = getText(text, "Shutdown");
                modalFooterApplyA.addEventListener("click", async function () {
                    await request("shutdown", {});
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
            a.setAttribute("data-tooltip", getText(text, "Shutdown"));
            {
                const i = document.createElement("i");
                i.innerText = "power_settings_new";
                i.setAttribute("class", "material-icons");
                a.appendChild(i);
            }
            li.appendChild(a);
        }
        UI.bottomMenuRightUl.appendChild(li);
    }
};

////
// WS API
const shutdown = async function (parameters) {
    // do not reconnect (see WS.js)
    WS.ws.onclose = function () { };
    // close websocket
    WS.ws.close();
    // this is not a good idea, but this works...
    window.open('about:blank', '_self').close();
}

////
// UI
export const init = async function () {
    await generateUI();
    WSTasks["shutdown"] = async function () { await shutdown() };
}
