import { Core } from "./Core.js";

export async function request(path, parameterJson, contentType) {
    // location.pathname.split('/')[1]: roomUUID
    const splitPathName = location.pathname.split('/');
    let uri = location.protocol + "//" + location.host + "/";
    for (let i = 1; i < splitPathName.length - 2; ++i) {
        uri += splitPathName[i]
        uri += "/";
    }
    uri += path;
    
    const requestInfo = {};
    requestInfo["method"] = "POST";
    const payloadJson = {};
    payloadJson["sessionUUID"] = Core.UUID;
    if (parameterJson) {
        payloadJson["parameters"] = parameterJson;
    }
    else {
        payloadJson["parameters"] = {};
    }
    requestInfo["body"] = JSON.stringify(payloadJson);
    if (contentType) {
        requestInfo["headers"] = {};
        requestInfo["headers"]["Content-Type"] = contentType;
    }
    console.log(uri);
    console.log(payloadJson);
    return fetch(uri, requestInfo).then(async response => {
        if (response.ok) {
            return response.text();
        }
        else {
            throw new Error(`Request failed: ${response.status}`);
        }
    });
}

export function beacon(path, parameterJson) {
    const splitPathName = location.pathname.split('/');
    const uri = location.protocol + "//" + location.host + "/";
    for (let i = 1; i < splitPathName.length - 2; ++i) {
        uri.concat('/', splitPathName[i]);
    }
    uri.concat('/', path);

    const payloadJson = {};
    payloadJson["sessionUUID"] = Core.UUID;
    if (parameterJson) {
        payloadJson["parameters"] = parameterJson;
    }
    else {
        payloadJson["parameters"] = {};
    }
    const data = new Blob([JSON.stringify(payloadJson)], {
        type: "application/json"
    });

    return navigator.sendBeacon(uri, data);
}
