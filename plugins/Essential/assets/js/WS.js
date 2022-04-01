import { Core } from './Core.js';
import { WSTasks } from './WSTasks.js';

export const WS = {};

function onOpen(event) {
    console.log("connected. (websocket)");
}
function onMessage(event) {
    if (event && event.data) {
        const json = JSON.parse(event.data);
        console.log(json);

        if ("API" in json) {
            if (json["API"] in WSTasks) {
                WSTasks[json["API"]](json["parameters"]);
                // navigator.locks.request('websocketTask', async lock => {
                //     return wsCallbackFuncs[j["task"]](j);
                // });
            }
            else {
                console.log("API " + json["API"] + " is not yet implemented.");
            }
        }
        else {
            console.log("json on websocket must have \"API\" entry!");
        }
    }
}
function onError(event) {
    console.log("Error... (websocket)");
}

function onClose(event) {
    WS.ws.close();
    console.log("disconnected... (websocket)");
    // this is not a good idea, but this works...
    window.open('about:blank','_self').close();
}

WS.init = async function () {
    // location.pathname.split('/')[1]: roomUUID
    const splitPathName = location.pathname.split('/');
    let uri = (location.protocol == "http:" ? "ws://" : "wss://") + location.host + "/";
    for (let i = 0; i < splitPathName.length - 2; ++i) {
        if (splitPathName[i].length > 0) {
            uri += splitPathName[i]
            uri += "/";
        }
    }

    if (WS.ws == null) {
        WS.ws = new WebSocket(uri);
        WS.ws.onopen = onOpen;
        WS.ws.onmessage = onMessage;
        WS.ws.onclose = onClose;
        WS.ws.onerror = onError;
    }

    // when we close tab/window, we gracefully close websocket
    // this is not reliable??
    window.addEventListener('beforeunload', function (e) {
        WS.ws.onclose = function () { };
        WS.ws.close();
    });
    return;
};

WS.sendMsg = async function (APIName, parameterJson) {
    const payloadJson = {};
    payloadJson["sessionUUID"] = Core.UUID;
    payloadJson["API"] = APIName;
    if (parameterJson) {
        payloadJson["parameters"] = parameterJson;
    }
    else {
        payloadJson["parameters"] = {};
    }

    if (WS.ws.readyState == 1) {
        WS.ws.send(JSON.stringify(payloadJson));
        return;
    }
    else {
        throw new Error("cannot sendMsg (WS)");
    }
}

