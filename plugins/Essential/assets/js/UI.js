export const UI = {};

UI.init = function () {
    // title
    document.title = 'Doppelganger - HTML5 GUI for geometry processing with C++ backend';

    // language setting
    {
        UI.language = (window.navigator.languages && window.navigator.languages[0]) ||
            window.navigator.language ||
            window.navigator.userLanguage ||
            window.navigator.browserLanguage;
        // we only keep first two characters
        UI.language = UI.language.substring(0, 2);
    }

    // HTML elements
    {
        // modal
        {
            UI.modalDiv = document.createElement('div');
            document.body.appendChild(UI.modalDiv);
        }
        // dropdown
        {
            UI.dropdownDiv = document.createElement('div');
            document.body.appendChild(UI.dropdownDiv);
        }
        // canvas + sidebar
        {
            UI.rootDiv = document.createElement('div');
            UI.rootDiv.setAttribute('style', 'height: 100%; width: 100%; display: flex; flex-direction: row;');
            // WebGL canvas
            {
                UI.webGLDiv = document.createElement('div');
                // todo: dynamically resize based on #plugins installed
                UI.webGLDiv.setAttribute('style', 'height: 100%; width: calc(100% - 500px); order: 0;');
                {
                    UI.webGLOutputDiv = document.createElement('div');
                    UI.webGLOutputDiv.setAttribute('style', 'height: 100%; width: 100%;');
                    UI.webGLDiv.appendChild(UI.webGLOutputDiv);
                }
                UI.rootDiv.appendChild(UI.webGLDiv);
            }
            // sidebar (sidenav)
            {
                UI.sideNavDiv = document.createElement('div');
                // todo: dynamically resize based on #plugins installed
                UI.sideNavDiv.setAttribute('style', 'height: 100%; width: 500px; order: 1; position: relative;');
                // top navbar
                {
                    UI.topNavBar = document.createElement('nav');
                    UI.topNavBar.setAttribute('class', 'nav teal lighten-2 z-depth-0');
                    UI.topNavBar.setAttribute('style', 'user-select: none;');
                    {
                        UI.topNavWrapper = document.createElement('div');
                        UI.topNavWrapper.setAttribute('class', 'nav-wrapper');
                        {
                            UI.topMenuLeftUl = document.createElement('ul');
                            UI.topMenuLeftUl.setAttribute('class', 'left');
                            UI.topNavWrapper.appendChild(UI.topMenuLeftUl);
                        }
                        {
                            UI.topMenuRightUl = document.createElement('ul');
                            UI.topMenuRightUl.setAttribute('class', 'right');
                            UI.topNavWrapper.appendChild(UI.topMenuRightUl);
                        }
                        UI.topNavBar.appendChild(UI.topNavWrapper);
                    }
                    UI.sideNavDiv.appendChild(UI.topNavBar);
                }

                // summary navbar (bottom)
                {
                    UI.summaryNavBar = document.createElement('nav');
                    UI.summaryNavBar.setAttribute('class', 'nav teal lighten-2 z-depth-0');
                    UI.summaryNavBar.setAttribute('style', 'position: absolute; bottom: 64px;');
                    // UI.summaryNavBar.setAttribute('style', 'position: absolute; bottom: 64px; user-select: none;');
                    {
                        UI.summaryNavWrapper = document.createElement('div');
                        UI.summaryNavWrapper.setAttribute('class', 'nav-wrapper');
                        {
                            UI.summaryMenuLeftUl = document.createElement('ul');
                            UI.summaryMenuLeftUl.setAttribute('class', 'left');
                            UI.summaryNavWrapper.appendChild(UI.summaryMenuLeftUl);
                        }
                        {
                            UI.summaryMenuRightUl = document.createElement('ul');
                            UI.summaryMenuRightUl.setAttribute('class', 'right');
                            UI.summaryNavWrapper.appendChild(UI.summaryMenuRightUl);
                        }
                        UI.summaryNavBar.appendChild(UI.summaryNavWrapper);
                    }
                    UI.sideNavDiv.appendChild(UI.summaryNavBar);
                }

                // bottom navbar
                {
                    UI.bottomNavBar = document.createElement('nav');
                    UI.bottomNavBar.setAttribute('class', 'nav teal lighten-2 z-depth-0');
                    UI.bottomNavBar.setAttribute('style', 'position: absolute; bottom: 0px; user-select: none;');
                    {
                        UI.bottomNavWrapper = document.createElement('div');
                        UI.bottomNavWrapper.setAttribute('class', 'nav-wrapper');
                        {
                            UI.bottomMenuLeftUl = document.createElement('ul');
                            UI.bottomMenuLeftUl.setAttribute('class', 'left');
                            UI.bottomNavWrapper.appendChild(UI.bottomMenuLeftUl);
                        }
                        {
                            UI.bottomMenuRightUl = document.createElement('ul');
                            UI.bottomMenuRightUl.setAttribute('class', 'right');
                            UI.bottomNavWrapper.appendChild(UI.bottomMenuRightUl);
                        }
                        UI.bottomNavBar.appendChild(UI.bottomNavWrapper);
                    }
                    UI.sideNavDiv.appendChild(UI.bottomNavBar);
                }

                // list of imported mesh
                {
                    UI.meshCollectionUl = document.createElement('ul');
                    UI.meshCollectionUl.setAttribute('class', 'collection');
                    UI.meshCollectionUl.setAttribute('style', 'max-height: calc(100% - 192px); height: calc(100% - 192px); overflow-y: scroll; margin: 0%; border:0px;');
                    UI.sideNavDiv.appendChild(UI.meshCollectionUl);
                }

                UI.rootDiv.appendChild(UI.sideNavDiv);
            }
            document.body.appendChild(UI.rootDiv);
        }
        // busy icon
        {
            UI.busyDiv = document.createElement('div');
            UI.busyDiv.setAttribute('style', 'visibility: hidden; position: absolute; left: 0; top: 0; width: 100vw; height: 100vh; background: rgba(100, 100, 100, .8); z-index: 2147483647;');
            {
                UI.busyIconA = document.createElement('a');
                UI.busyIconA.setAttribute('style', 'user-select: none; position: absolute; left: 0; top: 40vh; width: 100vw; color: rgba(250, 250, 250, 1); text-align: center;');
                {
                    UI.busyIconI = document.createElement('i');
                    UI.busyIconI.setAttribute('class', 'material-icons');
                    UI.busyIconI.setAttribute('style', 'font-size: 128px;');
                    UI.busyIconI.innerText = 'timer';
                    UI.busyIconA.appendChild(UI.busyIconI);
                }
                UI.busyDiv.appendChild(UI.busyIconA);
            }
            document.body.appendChild(UI.busyDiv);
        }
    }

    // utility function definitions
    {
        UI.setBusyMode = function (mode) {
            UI.busyDiv.style.visibility = (mode ? 'visible' : 'hidden');
        };
    }

    // parameter for meshLi
    UI.UUIDToMeshLi = {};

    return;
};

