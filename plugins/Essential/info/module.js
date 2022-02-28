import { UI } from '../../js/UI.js';
import { getText } from '../../js/Text.js';

const text = {
    "Info": { "en": "Information", "ja": "情報" },
    "Terms of use": { "en": "Terms of use", "ja": "利用規約" },
    "Terms of use_1": {
        "en": "This software (including the web version) may be stopped without any notice.",
        "ja": "本ソフトウェア(Web版含む)は予告なく公開停止する可能性があります。"
    },
    "Terms of use_2": {
        "en": "The author is not responsible for any disadvantage caused by the use of this software (including the web version).",
        "ja": "本ソフトウェア(Web版含む)を利用したことによるあらゆる不利益に関して、作者は責任を負いません。"
    },
    "Link": { "en": "Link", "ja": "リンク" },
    "Doppelganger": { "en": "Doppelganger", "ja": "Doppelganger" },
    "Plugin": { "en": "Plugin", "ja": "Plugin" },
    "Acknowledgement": { "en": "Acknowledgement", "ja": "謝辞" },
    "Acknowledgement_1": {
        "en": "This work was supported by JST, PRESTO Grant Number JPMJPR19J5, Japan.",
        "ja": "本ソフトウェアは、JST、さきがけ、JPMJPR19J5 の支援を受けたものです。"
    },
    "OK": { "en": "OK", "ja": "OK" }
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
                heading.innerText = getText(text, "Info");
                modalContentDiv.appendChild(heading);
            }
            ////
            // Terms of use
            {
                const heading = document.createElement("h5");
                heading.innerText = getText(text, "Terms of use");
                modalContentDiv.appendChild(heading);
            }
            {
                const ul = document.createElement('ul');
                {
                    const li = document.createElement('li');
                    li.innerText = getText(text, "Terms of use_1");
                    ul.appendChild(li);
                }
                {
                    const li = document.createElement('li');
                    li.innerText = getText(text, "Terms of use_2");
                    ul.appendChild(li);
                }
                modalContentDiv.appendChild(ul);
            }
            ////
            // Link
            {
                const heading = document.createElement("h5");
                heading.innerText = getText(text, "Link");
                modalContentDiv.appendChild(heading);
            }
            {
                const ul = document.createElement('ul');
                {
                    const li = document.createElement('li');
                    {
                        const a = document.createElement('a');
                        a.href = 'https://github.com/n-taka/Doppelganger';
                        a.innerText = getText(text, "Doppelganger");
                        a.target = "_blank";
                        a.rel = "noopener noreferrer";
                        li.appendChild(a);
                    }
                    ul.appendChild(li);
                }
                {
                    const li = document.createElement('li');
                    {
                        const a = document.createElement('a');
                        a.href = 'https://github.com/n-taka/Doppelganger_Plugin';
                        a.innerText = getText(text, "Plugin");
                        a.target = "_blank";
                        a.rel = "noopener noreferrer";
                        li.appendChild(a);
                    }
                    ul.appendChild(li);
                }
                modalContentDiv.appendChild(ul);
            }
            ////
            // Acknowledgement
            {
                const heading = document.createElement("h5");
                heading.innerText = getText(text, "Acknowledgement");
                modalContentDiv.appendChild(heading);
            }
            {
                const ul = document.createElement('ul');
                {
                    const li = document.createElement('li');
                    li.innerText = getText(text, "Acknowledgement_1");
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
                const modalFooterOKA = document.createElement("a");
                modalFooterOKA.setAttribute("class", "modal-close waves-effect waves-green btn-flat");
                modalFooterOKA.setAttribute("href", "#!");
                modalFooterOKA.innerHTML = getText(text, "OK");
                modalFooterDiv.appendChild(modalFooterOKA);
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
            a.setAttribute("data-tooltip", getText(text, "Info"));
            {
                const i = document.createElement("i");
                a.appendChild(i);
                i.innerText = "info_outline";
                i.setAttribute("class", "material-icons");
            }
            li.appendChild(a);
        }
        UI.bottomMenuRightUl.appendChild(li);
    }
};

////
// UI
export const init = async function () {
    await generateUI();
}
