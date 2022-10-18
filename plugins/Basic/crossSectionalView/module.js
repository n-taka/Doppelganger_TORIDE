import { UI } from '../../js/UI.js';
import { MouseKey } from '../../js/MouseKey.js';
import { Canvas } from '../../js/Canvas.js';

const generateUI = function () {
    UI.sliderDiv = document.createElement('div');
    UI.sliderDiv.setAttribute('style', 'position: absolute; left: 23px; top: 23px; height: calc(100% - 46px); background: rgba(100, 100, 100, .8); z-index: 2147483646;');

    {
        noUiSlider.create(UI.sliderDiv, {
            start: [0, 100],
            connect: [false, true, false],
            step: 0.1,
            direction: 'rtl',
            tooltips: false,
            orientation: 'vertical',
            range: {
                'min': 0,
                'max': 100
            }
        });
        UI.sliderDiv.noUiSlider.on('update', function () {
            Canvas.resetCamera();
        });
        UI.sliderDiv.noUiSlider.on('end', function () {
            document.body.style.cursor = "url(../icon/cursorIcon" + MouseKey.iconIdx + ".png) 16 16 , default"
        });
    }
    UI.webGLDiv.appendChild(UI.sliderDiv);
}

export const init = async function () {
    generateUI();
}

