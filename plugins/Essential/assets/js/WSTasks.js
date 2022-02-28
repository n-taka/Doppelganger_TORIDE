import { Core } from './Core.js';
import { UI } from './UI.js';

export const WSTasks = {};

WSTasks["initializeSession"] = initializeSession;
WSTasks["isServerBusy"] = isServerBusy;
WSTasks["forceReload"] = forceReload;
// WSTasks["syncMeshes"] = syncMeshes;

async function initializeSession(parameters) {
    ////
    // [IN]
    // parameters = {
    //  "sessionUUID": sessionUUID string
    // }
    Core.UUID = parameters["sessionUUID"];
}

async function isServerBusy(parameters) {
    ////
    // [IN]
    // parameters = {
    //  "isBusy": boolean value that represents the server is busy
    // }
    UI.setBusyMode(parameters["isBusy"]);
}

async function forceReload(parameters) {
    ////
    // [IN]
    // parameters = {
    // }
    location.reload();
}
