import { UI } from '../../js/UI.js';
import { MouseKey } from '../../js/MouseKey.js';
import { Canvas } from '../../js/Canvas.js';

const generateUI = function () {
    UI.sliderDiv = document.createElement('div');
    UI.sliderDiv.setAttribute('style', 'position: absolute; left: 2.5vh; top: 0; height: 95vh; transform: translate(0, 2.5vh); background: rgba(100, 100, 100, .8); z-index: 2147483646;');

    {
        noUiSlider.create(UI.sliderDiv, {
            start: [0, 100],
            connect: [false, true, false],
            step: 1,
            direction: 'rtl',
            tooltips: false,
            orientation: 'vertical',
            range: {
                'min': 0,
                'max': 100
            }
        });
        UI.sliderDiv.noUiSlider.on('update', function () {
            const sliderValue = UI.sliderDiv.noUiSlider.get();
            const clippingNear = (parseFloat(sliderValue[0]) - 50.0) / 50.0;
            const clippingFar = (parseFloat(sliderValue[1]) - 50.0) / 50.0;
            Canvas.resetCamera(false, clippingNear, clippingFar);
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

