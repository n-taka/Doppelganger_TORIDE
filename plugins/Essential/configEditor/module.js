import { UI } from '../../js/UI.js';
import { getText } from '../../js/Text.js';
import { request } from "../../js/request.js";

const text = {
    "Config editor": { "en": "Config editor", "ja": "設定" },
    "Server config": { "en": "Server config", "ja": "サーバー設定" },
    "Protocol": { "en": "Protocol", "ja": "プロトコル" },
    "Host (IP adress)": { "en": "Host (IP adress)", "ja": "ホスト (IPアドレス)" },
    "Port (0|[49152-65535])": { "en": "Port (0|[49152-65535])", "ja": "ポート (0|[49152-65535])" },
    "Browser config": { "en": "Browser config", "ja": "ブラウザ設定" },
    "Browser": { "en": "Browser", "ja": "ブラウザ" },
    "Open as...": { "en": "Open as...", "ja": "起動モード" },
    "Open browser on startup": { "en": "Open browser on startup", "ja": "アプリ起動時にブラウザを開く" },
    "Plugin config": { "en": "Plugin config", "ja": "プラグイン設定" },
    "Directory for plugin": { "en": "Directory for plugin", "ja": "プラグイン用のディレクトリ" },
    "Open plugin directory": { "en": "Open plugin directory", "ja": "プラグイン用のディレクトリを開く" },
    "Basically, please keep blank (directory is automatically set)": { "en": "Basically, please keep blank", "ja": "基本的には空欄のままにしてください (ディレクトリは自動で作成されます)" },
    "List of URL for plugin list (one URL per line)": { "en": "List of URL for plugin list (one URL per line)", "ja": "プラグインリストのURL (1行に1URLずつ記載してください)" },
    "Output config": { "en": "Output config", "ja": "書き出し設定" },
    "Directory for output": { "en": "Directory for output", "ja": "書き出し用のディレクトリ" },
    "Store via browser": { "en": "Store via browser", "ja": "ブラウザ経由でダウンロードする" },
    "Store into local storage": { "en": "Store into local storage", "ja": "ローカルに保存する" },
    "Open output directory": { "en": "Open output directory", "ja": "書き出し用のディレクトリを開く" },
    "Log config": { "en": "Log config", "ja": "ログ設定" },
    "Log level": { "en": "Log level", "ja": "ログレベル" },
    "SYSTEM": { "en": "SYSTEM", "ja": "システム" },
    "APICALL": { "en": "APICALL", "ja": "API呼び出し" },
    "WSCALL": { "en": "WSCALL", "ja": "API呼び出し (WS)" },
    "ERROR": { "en": "ERROR", "ja": "エラー" },
    "MISC": { "en": "MISC", "ja": "その他" },
    "DEBUG": { "en": "DEBUG", "ja": "デバッグ" },
    "Log destination": { "en": "Log destination", "ja": "ログ出力先" },
    "STDOUT": { "en": "STDOUT", "ja": "標準出力" },
    "FILE": { "en": "FILE", "ja": "ファイル" },
    "Directory for log": { "en": "Directory for log", "ja": "ログ用のディレクトリ" },
    "Open log directory": { "en": "Open log directory", "ja": "ログ用のディレクトリを開く" },
    "Update & Shutdown": { "en": "Update & Shutdown", "ja": "更新して終了" },
    "Cancel": { "en": "Cancel", "ja": "キャンセル" }
};

const generateUI = async function () {
    // first, we get current config and construct a modal
    //   while constructing a modal, we implicitly assign default values
    return request("configEditor", { "forUIGeneration": true }).then((response) => {
        const configJson = JSON.parse(response);
        console.log(configJson);
        ////
        // modal
        const modal = document.createElement("div");
        {
            modal.setAttribute("class", "modal");
            // content
            // declare here for make them accessible from footer
            const selectServerProtocol = document.createElement('select');
            const inputServerHost = document.createElement('input');
            const inputServerPort = document.createElement('input');
            const inputCheckboxOpenOnStartup = document.createElement("input");
            const selectBrowser = document.createElement('select');
            const selectMode = document.createElement('select');

            {
                const modalContentDiv = document.createElement('div');
                modalContentDiv.setAttribute("class", "modal-content");
                {
                    const heading = document.createElement("h4");
                    heading.innerText = getText(text, "Config editor");
                    modalContentDiv.appendChild(heading);
                }

                ////
                // server settings
                {
                    const heading = document.createElement("h5");
                    heading.innerText = getText(text, "Server config");
                    modalContentDiv.appendChild(heading);
                }
                {
                    const divRow = document.createElement('div');
                    divRow.setAttribute('class', 'row');
                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s3');
                        {
                            {
                                const option = document.createElement('option');
                                option.setAttribute("value", "http://");
                                option.setAttribute('style', 'text-align: center; user-select: none;');
                                option.setAttribute("selected", true);
                                option.innerText = "http://";
                                selectServerProtocol.appendChild(option);
                            }
                            {
                                const option = document.createElement('option');
                                option.setAttribute("value", "https://");
                                option.setAttribute('style', 'text-align: center; user-select: none;');
                                // currently we don't support SSL
                                option.setAttribute("disabled", true);
                                option.innerText = "https://";
                                selectServerProtocol.appendChild(option);
                            }
                            divCol.appendChild(selectServerProtocol);
                        }
                        {
                            const label = document.createElement('label');
                            label.innerText = getText(text, "Protocol");
                            divCol.appendChild(label);
                        }
                        divRow.appendChild(divCol);
                    }
                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s6');
                        {
                            inputServerHost.setAttribute('type', 'text');
                            inputServerHost.setAttribute('id', 'host');
                            inputServerHost.setAttribute('value', (configJson["server"] && configJson["server"]["host"]) ? configJson["server"]["host"] : '127.0.0.1');
                            inputServerHost.setAttribute('class', 'validate');
                            inputServerHost.setAttribute('style', 'text-align: center;');
                            divCol.appendChild(inputServerHost);
                        }
                        {
                            const label = document.createElement('label');
                            label.setAttribute('for', "host");
                            label.innerText = getText(text, "Host (IP adress)");
                            divCol.appendChild(label);
                        }
                        divRow.appendChild(divCol);
                    }
                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s3');
                        {
                            inputServerPort.setAttribute('type', 'number');
                            inputServerPort.setAttribute('id', 'port');
                            inputServerPort.setAttribute('value', (configJson["server"] && configJson["server"]["port"]) ? configJson["server"]["port"] : '0');
                            inputServerPort.setAttribute('class', 'validate');
                            inputServerPort.setAttribute('min', '0');
                            inputServerPort.setAttribute('max', '65535');
                            inputServerPort.setAttribute('step', '1');
                            inputServerPort.setAttribute('style', 'text-align: center;');
                            divCol.appendChild(inputServerPort);
                        }
                        {
                            const label = document.createElement('label');
                            label.setAttribute('for', "port");
                            label.innerText = getText(text, "Port (0|[49152-65535])");
                            divCol.appendChild(label);
                        }
                        divRow.appendChild(divCol);
                    }
                    modalContentDiv.appendChild(divRow);
                }

                ////
                // browser settings
                {
                    const heading = document.createElement("h5");
                    heading.innerText = getText(text, "Browser config");
                    modalContentDiv.appendChild(heading);
                }
                {
                    const divRow = document.createElement('div');
                    divRow.setAttribute('class', 'row');
                    const browserToMode = {
                        "chrome": ["app", "window", "tab", "default"],
                        "firefox": ["window", "tab", "default"],
                        "edge": ["app", "window", "tab", "default"],
                        "safari": ["window", "tab", "default"],
                        "default": ["default"],
                    }
                    const divSelect = document.createElement('div');

                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s12');
                        {
                            const pCheckbox = document.createElement("p");
                            {
                                const labelCheckbox = document.createElement("label");
                                {
                                    inputCheckboxOpenOnStartup.setAttribute("type", "checkbox")
                                    inputCheckboxOpenOnStartup.setAttribute("id", "openOnStartup");
                                    inputCheckboxOpenOnStartup.checked = configJson["browser"] && configJson["browser"]["openOnStartup"];
                                    inputCheckboxOpenOnStartup.addEventListener("change", function () {
                                        divSelect.style.display = (inputCheckboxOpenOnStartup.checked) ? "inline" : "none";
                                    });
                                    labelCheckbox.appendChild(inputCheckboxOpenOnStartup);
                                }
                                {
                                    const spanCheckbox = document.createElement("span");
                                    spanCheckbox.innerText = getText(text, "Open browser on startup");
                                    labelCheckbox.appendChild(spanCheckbox);
                                }
                                pCheckbox.appendChild(labelCheckbox);
                            }
                            divCol.appendChild(pCheckbox);
                        }
                        divRow.appendChild(divCol);
                    }

                    {
                        // divSelect
                        {
                            const divCol = document.createElement('div');
                            divCol.setAttribute('class', 'input-field col s6');
                            {
                                const browsers = ["chrome", "firefox", "edge", "safari", "default"]
                                for (let browser of browsers) {
                                    const option = document.createElement('option');
                                    option.setAttribute("value", browser);
                                    option.setAttribute('style', 'text-align: center; user-select: none;');
                                    if (configJson["browser"] && configJson["browser"]["type"] && configJson["browser"]["type"] == browser) {
                                        option.setAttribute("selected", true);
                                    }
                                    if (configJson["browser"] && configJson["browser"]["availableBrowsers"] && !configJson["browser"]["availableBrowsers"].includes(browser)) {
                                        option.setAttribute("disabled", true);
                                    }
                                    option.innerText = browser;
                                    selectBrowser.appendChild(option);
                                }
                                divCol.appendChild(selectBrowser);
                            }
                            {
                                const label = document.createElement('label');
                                label.innerText = getText(text, "Browser");
                                divCol.appendChild(label);
                            }
                            divSelect.appendChild(divCol);
                        }
                        selectBrowser.addEventListener("change", function () {
                            const selectedBrowser = M.FormSelect.getInstance(selectBrowser).getSelectedValues()[0];
                            const supportedMode = browserToMode[selectedBrowser];
                            const dropdownUlMode = M.FormSelect.getInstance(selectMode).dropdownOptions;
                            for (let ul of dropdownUlMode.children) {
                                ul.classList.add("disabled");
                                ul.classList.remove("selected");
                                if (supportedMode.includes(ul.innerText)) {
                                    ul.classList.remove("disabled")
                                }
                                if (ul.innerText == "default") {
                                    // as far as I know, this is the best way to update materializecss select
                                    ul.click()
                                }
                            }
                        })
                        {
                            const divCol = document.createElement('div');
                            divCol.setAttribute('class', 'input-field col s6');
                            {
                                const openMode = ["app", "window", "tab", "default"]
                                for (let mode of openMode) {
                                    const option = document.createElement('option');
                                    option.setAttribute("value", mode);
                                    option.setAttribute('style', 'text-align: center; user-select: none;');
                                    if (configJson["browser"] && configJson["browser"]["openAs"] && configJson["browser"]["openAs"] == mode) {
                                        option.setAttribute("selected", true);
                                    }

                                    if (configJson["browser"] && configJson["browser"]["type"] && browserToMode[configJson["browser"]["type"]]) {
                                        const supportedMode = browserToMode[configJson["browser"]["type"]];
                                        if (!supportedMode.includes(mode)) {
                                            option.setAttribute("disabled", true);
                                        }
                                    }
                                    option.innerText = mode;
                                    selectMode.appendChild(option);
                                }
                                divCol.appendChild(selectMode);
                            }
                            {
                                const label = document.createElement('label');
                                label.innerText = getText(text, "Open as...");
                                divCol.appendChild(label);
                            }
                            divSelect.appendChild(divCol);
                        }
                        divRow.appendChild(divSelect)
                    }
                    modalContentDiv.appendChild(divRow);
                }

                ////
                // plugin settings
                {
                    const heading = document.createElement("h5");
                    heading.innerText = getText(text, "Plugin config");
                    modalContentDiv.appendChild(heading);
                }
                {
                    const divRow = document.createElement('div');
                    divRow.setAttribute('class', 'row');
                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s12');
                        {
                            const textarea = document.createElement('textarea');
                            textarea.setAttribute('id', 'pluginListURL');
                            textarea.setAttribute('class', 'materialize-textarea')
                            textarea.innerText = "";
                            if (configJson["plugin"] && configJson["plugin"]["listURL"]) {
                                for (let url of configJson["plugin"]["listURL"]) {
                                    textarea.innerHTML += url;
                                    textarea.innerHTML += '\n';
                                }
                            }
                            divCol.appendChild(textarea);
                        }
                        {
                            const label = document.createElement('label');
                            label.setAttribute('for', "pluginListURL");
                            label.innerText = getText(text, "List of URL for plugin list (one URL per line)");
                            divCol.appendChild(label);
                        }
                        divRow.appendChild(divCol);
                    }

                    {
                        const divCol = document.createElement('div');
                        // we make this textinput invisible
                        //   because storing arbitrary path would cause permission error (especially in macOS)
                        divCol.setAttribute('style', 'display: none;');
                        divCol.setAttribute('class', 'input-field col s12');
                        {
                            const input = document.createElement('input');
                            input.setAttribute('type', 'text');
                            input.setAttribute('id', 'pluginDir');
                            input.setAttribute('class', 'validate');
                            input.setAttribute('placeholder', getText(text, "Basically, please keep blank (directory is automatically set)"));
                            if (configJson["plugin"] && configJson["plugin"]["dir"] && configJson["plugin"]["dir"].length > 0) {
                                input.setAttribute("value", configJson["plugin"]["dir"]);
                            }
                            divCol.appendChild(input);
                        }
                        {
                            const label = document.createElement('label');
                            label.setAttribute('for', "pluginDir");
                            label.innerText = getText(text, "Directory for plugin");
                            divCol.appendChild(label);
                        }
                        divRow.appendChild(divCol);
                    }

                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'col s12');
                        {
                            const btn = document.createElement('a');
                            btn.setAttribute('class', 'waves-effect wave-light btn');
                            btn.setAttribute('style', 'width: 100%;');
                            btn.innerText = getText(text, "Open plugin directory");
                            btn.addEventListener("click", async function () {
                                request("configEditor", { "openDirectory": "plugin" });
                            });
                            divCol.appendChild(btn);
                        }
                        divRow.appendChild(divCol);
                    }
                    modalContentDiv.appendChild(divRow);
                }

                ////
                // output settings
                {
                    const heading = document.createElement("h5");
                    heading.innerText = getText(text, "Output config");
                    modalContentDiv.appendChild(heading);
                }
                {
                    const divRowOutput = document.createElement('div');
                    divRowOutput.setAttribute('class', 'row');
                    // declare here
                    const divColDirOpen = document.createElement('div');
                    // radio button (1)
                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s6');
                        {
                            const pRadioButtons = document.createElement("p");
                            {
                                const labelRadioButtons = document.createElement("label");
                                {
                                    const inputRadioButtons = document.createElement("input");
                                    inputRadioButtons.setAttribute("type", "radio")
                                    inputRadioButtons.setAttribute("name", "outputRadio");
                                    inputRadioButtons.setAttribute("class", "with-gap");
                                    inputRadioButtons.setAttribute("value", "download");
                                    inputRadioButtons.checked = configJson["output"] && configJson["output"]["type"] == "download";
                                    inputRadioButtons.addEventListener("change", function () {
                                        divColDirOpen.style.display = (inputRadioButtons.checked) ? "none" : "inline";
                                    });
                                    labelRadioButtons.appendChild(inputRadioButtons);
                                }
                                {
                                    const spanRadioButtons = document.createElement("span");
                                    spanRadioButtons.innerText = getText(text, "Store via browser");
                                    labelRadioButtons.appendChild(spanRadioButtons);
                                }
                                pRadioButtons.appendChild(labelRadioButtons);
                            }
                            divCol.appendChild(pRadioButtons);
                        }
                        divRowOutput.appendChild(divCol);
                    }
                    // radio button (2)
                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s6');
                        {
                            const pRadioButtons = document.createElement("p");
                            {
                                const labelRadioButtons = document.createElement("label");
                                {
                                    const inputRadioButtons = document.createElement("input");
                                    inputRadioButtons.setAttribute("type", "radio")
                                    inputRadioButtons.setAttribute("name", "outputRadio");
                                    inputRadioButtons.setAttribute("class", "with-gap");
                                    inputRadioButtons.setAttribute("value", "storage");
                                    inputRadioButtons.checked = configJson["output"] && configJson["output"]["type"] == "storage";
                                    inputRadioButtons.addEventListener("change", function () {
                                        divColDirOpen.style.display = (inputRadioButtons.checked) ? "inline" : "none";
                                    });
                                    labelRadioButtons.appendChild(inputRadioButtons);
                                }
                                {
                                    const spanRadioButtons = document.createElement("span");
                                    spanRadioButtons.innerText = getText(text, "Store into local storage");
                                    labelRadioButtons.appendChild(spanRadioButtons);
                                }
                                pRadioButtons.appendChild(labelRadioButtons);
                            }
                            divCol.appendChild(pRadioButtons);
                        }
                        divRowOutput.appendChild(divCol);
                    }
                    // text input for dir path (invisible)
                    {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s12');
                        // we make this textinput invisible
                        //   because storing arbitrary path would cause permission error (especially in macOS)
                        divCol.setAttribute('style', 'display: none;');
                        {
                            const input = document.createElement('input');
                            input.setAttribute('type', 'text');
                            input.setAttribute('id', 'outputDir');
                            input.setAttribute('class', 'validate');
                            input.setAttribute('placeholder', getText(text, "Basically, please keep blank (directory is automatically set)"));
                            if (configJson["output"] && configJson["output"]["dir"] && configJson["output"]["dir"].length > 0) {
                                input.setAttribute("value", configJson["output"]["dir"]);
                            }
                            divCol.appendChild(input);
                        }
                        {
                            const label = document.createElement('label');
                            label.setAttribute('for', "outputDir");
                            label.innerText = getText(text, "Directory for output");
                            divCol.appendChild(label);
                        }
                        divRowOutput.appendChild(divCol);
                    }
                    // open directory
                    {
                        // const divColDirOpen = document.createElement('div');
                        divColDirOpen.setAttribute('class', 'col s12');
                        divColDirOpen.setAttribute('style', (configJson["output"] && configJson["output"]["type"] == "storage") ? 'display: inline;' : 'display: none;');
                        {
                            const btn = document.createElement('a');
                            btn.setAttribute('class', 'waves-effect wave-light btn');
                            btn.setAttribute('style', 'width: 100%;');
                            btn.innerText = getText(text, "Open output directory");
                            btn.addEventListener("click", async function () {
                                request("configEditor", { "openDirectory": "output" });
                            });
                            divColDirOpen.appendChild(btn);
                        }
                        divRowOutput.appendChild(divColDirOpen);
                    }

                    modalContentDiv.appendChild(divRowOutput);
                }

                ////
                // log settings
                {
                    const heading = document.createElement("h5");
                    heading.innerText = getText(text, "Log config");
                    modalContentDiv.appendChild(heading);
                }
                {
                    const divRow = document.createElement('div');
                    divRow.setAttribute('class', 'row');
                    {
                        const heading = document.createElement("h6");
                        heading.innerText = getText(text, "Log level");
                        divRow.appendChild(heading);
                    }
                    const logLevels = ["SYSTEM", "APICALL", "WSCALL", "ERROR", "MISC", "DEBUG"];
                    for (let level of logLevels) {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s4');
                        {
                            const pCheckbox = document.createElement("p");
                            {
                                const labelCheckbox = document.createElement("label");
                                {
                                    const inputCheckbox = document.createElement("input");
                                    inputCheckbox.setAttribute("type", "checkbox")
                                    inputCheckbox.setAttribute("id", "log" + level);
                                    inputCheckbox.checked = configJson["log"] && configJson["log"]["level"] && configJson["log"]["level"].includes(level);
                                    labelCheckbox.appendChild(inputCheckbox);
                                }
                                {
                                    const spanCheckbox = document.createElement("span");
                                    spanCheckbox.innerText = getText(text, level);
                                    labelCheckbox.appendChild(spanCheckbox);
                                }
                                pCheckbox.appendChild(labelCheckbox);
                            }
                            divCol.appendChild(pCheckbox);
                        }
                        divRow.appendChild(divCol);
                    }
                    {
                        const heading = document.createElement("h6");
                        heading.innerText = getText(text, "Log destination");
                        divRow.appendChild(heading);
                    }
                    // declare here
                    const divColDirOpen = document.createElement('div');

                    const destinations = ["STDOUT", "FILE"];
                    for (let dest of destinations) {
                        const divCol = document.createElement('div');
                        divCol.setAttribute('class', 'input-field col s6');
                        {
                            const pCheckbox = document.createElement("p");
                            {
                                const labelCheckbox = document.createElement("label");
                                {
                                    const inputCheckbox = document.createElement("input");
                                    inputCheckbox.setAttribute("type", "checkbox")
                                    inputCheckbox.setAttribute("id", "log" + dest);
                                    inputCheckbox.checked = configJson["log"] && configJson["log"]["type"] && configJson["log"]["type"].includes(dest);
                                    if (dest == "FILE") {
                                        divColDirOpen.setAttribute('style', (inputCheckbox.checked) ? 'display: inline;' : 'display: none;');
                                        inputCheckbox.addEventListener("change", function () {
                                            divColDirOpen.setAttribute('style', (inputCheckbox.checked) ? 'display: inline;' : 'display: none;');
                                        });
                                    }

                                    labelCheckbox.appendChild(inputCheckbox);
                                }
                                {
                                    const spanCheckbox = document.createElement("span");
                                    spanCheckbox.innerText = getText(text, dest);
                                    labelCheckbox.appendChild(spanCheckbox);
                                }
                                pCheckbox.appendChild(labelCheckbox);
                            }
                            divCol.appendChild(pCheckbox);
                        }
                        divRow.appendChild(divCol);
                    }
                    {
                        const divCol = document.createElement('div');
                        // we make this textinput invisible
                        //   because storing arbitrary path would cause permission error (especially in macOS)
                        divCol.setAttribute('style', 'display: none;');
                        divCol.setAttribute('class', 'input-field col s12');
                        {
                            const input = document.createElement('input');
                            input.setAttribute('type', 'text');
                            input.setAttribute('id', 'logDir');
                            input.setAttribute('class', 'validate');
                            input.setAttribute('placeholder', getText(text, "Basically, please keep blank (directory is automatically set)"));
                            if (configJson["log"] && configJson["log"]["dir"] && configJson["log"]["dir"].length > 0) {
                                input.setAttribute("value", configJson["log"]["dir"]);
                            }
                            divCol.appendChild(input);
                        }
                        {
                            const label = document.createElement('label');
                            label.setAttribute('for', "logDir");
                            label.innerText = getText(text, "Directory for log");
                            divCol.appendChild(label);
                        }
                        divRow.appendChild(divCol);
                    }
                    {
                        divColDirOpen.setAttribute('class', 'col s12');
                        {
                            const btn = document.createElement('a');
                            btn.setAttribute('class', 'waves-effect wave-light btn');
                            btn.setAttribute('style', 'width: 100%;');
                            btn.innerText = getText(text, "Open log directory");
                            btn.addEventListener("click", async function () {
                                request("configEditor", { "openDirectory": "log" });
                            });
                            divColDirOpen.appendChild(btn);
                        }
                        divRow.appendChild(divColDirOpen);
                    }
                    modalContentDiv.appendChild(divRow);
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
                    modalFooterApplyA.innerHTML = getText(text, "Update & Shutdown");
                    modalFooterApplyA.addEventListener("click", async function () {
                        const updatedConfigJson = {};
                        // server
                        updatedConfigJson["server"] = {};
                        updatedConfigJson["server"]["protocol"] = M.FormSelect.getInstance(selectServerProtocol).getSelectedValues()[0];
                        updatedConfigJson["server"]["host"] = inputServerHost.value;
                        updatedConfigJson["server"]["port"] = parseInt(inputServerPort.value);
                        // browser
                        updatedConfigJson["browser"] = {};
                        updatedConfigJson["browser"]["type"] = M.FormSelect.getInstance(selectBrowser).getSelectedValues()[0];
                        updatedConfigJson["browser"]["openAs"] = M.FormSelect.getInstance(selectMode).getSelectedValues()[0];
                        updatedConfigJson["browser"]["openOnStartup"] = inputCheckboxOpenOnStartup.checked;
                        // log
                        updatedConfigJson["log"] = {};
                        updatedConfigJson["log"]["level"] = [];
                        const logLevels = ["SYSTEM", "APICALL", "WSCALL", "ERROR", "MISC", "DEBUG"];
                        for (let level of logLevels) {
                            if (document.getElementById("log" + level).checked) {
                                updatedConfigJson["log"]["level"].push(level);
                            }
                        }
                        updatedConfigJson["log"]["type"] = [];
                        const destinations = ["STDOUT", "FILE"];
                        for (let dest of destinations) {
                            if (document.getElementById("log" + dest).checked) {
                                updatedConfigJson["log"]["type"].push(dest);
                            }
                        }
                        // output
                        updatedConfigJson["output"] = {};
                        updatedConfigJson["output"]["type"] = document.querySelector('input[name="outputRadio"]:checked').value;
                        // plugin
                        updatedConfigJson["plugin"] = {};
                        updatedConfigJson["plugin"]["listURL"] = document.getElementById("pluginListURL").value.split("\n").filter((str => (str.length > 0)));
                        await request("configEditor", updatedConfigJson);
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
                a.setAttribute("data-tooltip", getText(text, "Config editor"));
                {
                    const i = document.createElement("i");
                    a.appendChild(i);
                    i.innerText = "settings";
                    i.setAttribute("class", "material-icons");
                }
                li.appendChild(a);
            }
            UI.bottomMenuRightUl.appendChild(li);
        }
    });
};

////
// UI
export const init = async function () {
    await generateUI();
    M.textareaAutoResize(document.getElementById('pluginListURL'));
}
