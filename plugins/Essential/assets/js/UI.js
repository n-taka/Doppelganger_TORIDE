export const UI = {};

UI.init = function () {
    // title
    document.title = 'TORIDE';

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
            UI.rootDiv.setAttribute('class', 'row');
            UI.rootDiv.setAttribute('style', 'height: 100vh; width: 100vw;');

            // top navbar
            {
                UI.topNavBar = document.createElement('nav');
                UI.topNavBar.setAttribute('class', 'sideNavOnRight teal lighten-2 z-depth-0');
                UI.topNavBar.setAttribute('style', 'user-select: none;');
                {
                    UI.topNavWrapper = document.createElement('div');
                    UI.topNavWrapper.setAttribute('class', 'nav-wrapper');
                    {
                        UI.topNavLogo = document.createElement('a');
                        UI.topNavLogo.setAttribute('href', '#');
                        UI.topNavLogo.setAttribute('class', 'brand-logo center');
                        {
                            UI.topNavLogoImage = document.createElement('img');
                            UI.topNavLogoImage.setAttribute('src', '../icon/toride_icon.png');
                            UI.topNavLogoImage.setAttribute('style', 'height: 64px;');
                            UI.topNavLogo.appendChild(UI.topNavLogoImage);
                        }
                        UI.topNavWrapper.appendChild(UI.topNavLogo);
                    }
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
                UI.rootDiv.appendChild(UI.topNavBar);
            }

            // WebGL canvas
            {
                UI.webGLDiv = document.createElement('div');
                UI.webGLDiv.setAttribute('class', 'sideNavOnRight');
                // 192px: 64px (height for navbar) * 3
                UI.webGLDiv.setAttribute('style', 'height: calc(100vh - 192px); position: relative;');
                {
                    UI.webGLOutputDiv = document.createElement('div');
                    UI.webGLOutputDiv.setAttribute('style', 'height: 100%; width: 100%;');
                    UI.webGLDiv.appendChild(UI.webGLOutputDiv);

                    // Fixed-action-button
                    // https://materializecss.com/floating-action-button.html
                    {
                        UI.FABDiv = document.createElement('div');
                        UI.FABDiv.setAttribute('class', 'fixed-action-btn');
                        UI.FABDiv.setAttribute('style', 'position: absolute;');
                        {
                            const FABLargeBtn = document.createElement('a');
                            FABLargeBtn.setAttribute('class', 'btn-floating btn-large teal lighten-2');
                            {
                                const FABLargeBtnIcon = document.createElement('i');
                                FABLargeBtnIcon.setAttribute('class', 'large material-icons');
                                FABLargeBtnIcon.innerText = 'menu';
                                FABLargeBtn.appendChild(FABLargeBtnIcon);
                            }
                            UI.FABDiv.appendChild(FABLargeBtn);
                        }
                        {
                            UI.FABUl = document.createElement('ul');
                            {
                                // actual elements are added by plugins
                            }
                            UI.FABDiv.appendChild(UI.FABUl);
                        }
                        UI.webGLDiv.appendChild(UI.FABDiv);
                    }

                    {
                        UI.sideNavTriggerDiv = document.createElement('div');
                        UI.sideNavTriggerDiv.setAttribute('style', 'position: absolute; right: 23px; bottom: 50%;');
                        {
                            const FABLargeBtn = document.createElement('a');
                            FABLargeBtn.setAttribute('class', 'btn-floating teal lighten-2 sidenav-trigger show-on-large');
                            FABLargeBtn.setAttribute('href', '#');
                            FABLargeBtn.setAttribute('data-target', 'slide-out');
                            {
                                const FABLargeBtnIcon = document.createElement('i');
                                FABLargeBtnIcon.setAttribute('class', 'large material-icons');
                                FABLargeBtnIcon.innerText = 'chevron_left';
                                FABLargeBtn.appendChild(FABLargeBtnIcon);
                            }
                            UI.sideNavTriggerDiv.appendChild(FABLargeBtn);
                        }
                        // we put this trigger button to rootDiv (under the sidenav)
                        UI.rootDiv.appendChild(UI.sideNavTriggerDiv);
                    }
                }
                UI.rootDiv.appendChild(UI.webGLDiv);
            }
            // navbar
            {
                // summary navbar (bottom)
                {
                    UI.summaryNavBar = document.createElement('nav');
                    UI.summaryNavBar.setAttribute('class', 'sideNavOnRight teal lighten-2 z-depth-0');
                    UI.summaryNavBar.setAttribute('style', 'position: absolute; bottom: 64px;');
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
                    UI.rootDiv.appendChild(UI.summaryNavBar);
                }

                // bottom navbar
                {
                    UI.bottomNavBar = document.createElement('nav');
                    UI.bottomNavBar.setAttribute('class', 'sideNavOnRight teal lighten-2 z-depth-0');
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
                    UI.rootDiv.appendChild(UI.bottomNavBar);
                }
            }
            // sidebar (sidenav)
            {
                {
                    UI.sideNavDiv = document.createElement('div');
                    UI.sideNavDiv.setAttribute('id', 'slide-out');
                    UI.sideNavDiv.setAttribute('class', 'sidenav sidenav-fixed');
                    UI.sideNavDiv.setAttribute('style', 'width: 500px; height: 100vh; overflow: hidden; padding: 0;');

                    // list of imported mesh
                    {
                        UI.meshCollectionUl = document.createElement('ul');
                        UI.meshCollectionUl.setAttribute('class', 'collection');
                        UI.meshCollectionUl.setAttribute('style', 'width: 100%; max-height: 100%; height: 100%; overflow-y: scroll; margin: 0%; border:0px;');
                        UI.sideNavDiv.appendChild(UI.meshCollectionUl);
                    }

                    UI.rootDiv.appendChild(UI.sideNavDiv);
                    // UI.rootDiv.appendChild(UI.sideNavDiv);
                }

            }
            document.body.appendChild(UI.rootDiv);

            M.FloatingActionButton.init(document.querySelectorAll('.fixed-action-btn'), {
                hoverEnabled: false
            });

            M.Sidenav.init(document.querySelectorAll('.sidenav'), {
                edge: 'right'
            });
        }
        // busy icon
        {
            UI.busyDiv = document.createElement('div');
            UI.busyDiv.setAttribute('style', 'visibility: hidden; position: absolute; left: 0; top: 0; width: 100%; height: 100%; background: rgba(100, 100, 100, .8); z-index: 2147483647;');
            {
                UI.busyIconA = document.createElement('a');
                UI.busyIconA.setAttribute('style', 'user-select: none; position: absolute; left: 0; top: 40vh; width: 100%; color: rgba(250, 250, 250, 1); text-align: center;');
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

