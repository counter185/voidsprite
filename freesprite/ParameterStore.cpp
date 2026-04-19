#include "ParameterStore.h"
#include "UILabel.h"
#include "Panel.h"
#include "UICheckbox.h"
#include "UISlider.h"
#include "UIColorInputField.h"
#include "UIDoubleSlider.h"


Parameter& ParameterStore::getParam(std::string key)
{
	for (auto& p : parameters) {
		if (p.name == key) {
			return p;
		}
	}
	return PARAM_INVALID;
}

Panel* ParameterStore::generateVerticalUI(std::function<void()> onChangedCallback)
{
	auto updateLabelFn = [](Parameter& p, UILabel* l) {
        switch (p.paramType) {
            case PT_BOOL:
                break;
            case PT_INT:
            case PT_COLOR_L:
                l->setText(std::to_string((int)p.vNum));
                break;
            case PT_FLOAT:
                l->setText(frmt("{:.1f}", p.vNum));
                break;
            case PT_INT_RANGE:
                l->setText(std::to_string((int)p.vNum) + ":" + std::to_string((int)p.vNum2));
                break;
        }
    };

    Panel* panel = new Panel();
    panel->sizeToContent = true;
    panel->passThroughMouse = true;

    int y = 0;
    int i = 0;
    for (auto& p : parameters) {
        UILabel* label = new UILabel(p.name);
        label->position = XY{ 10, y + 2 };
        panel->subWidgets.addDrawable(label);

        switch (p.paramType) {
        case PT_BOOL:
        {
            UICheckbox* checkbox = new UICheckbox("", p.vNum == 1);
            checkbox->position = XY{ 250, y };
            checkbox->onStateChangeCallback = [this, i, onChangedCallback](UICheckbox* c, bool v) {
                parameters.at(i).vNum = v ? 1 : 0;
                onChangedCallback();
            };
            panel->subWidgets.addDrawable(checkbox);
        }
        break;
        case PT_COLOR_L:
        case PT_INT:
        case PT_FLOAT:
        {
            float v = (p.vNum - p.numMinValue) / (p.numMaxValue - p.numMinValue);
            UISlider* slider = new UISlider();
            slider->position = XY{ 285, y };
            slider->sliderPos = v;
            slider->wxHeight = 25;
            if (p.paramType == PT_COLOR_L) {
                slider->backgroundFill = Fill::Gradient(0xA0000000, 0xA0FFFFFF, 0xA0000000, 0xA0FFFFFF);
            }
            panel->subWidgets.addDrawable(slider);

            UILabel* valueLabel = new UILabel();
            valueLabel->position = xySubtract(slider->position, { 84, 0 });
            panel->subWidgets.addDrawable(valueLabel);

            slider->onChangeValueCallback = [this, i, valueLabel, onChangedCallback, updateLabelFn](UISlider* s, float v) {
                Parameter& p = parameters.at(i);
                switch (p.paramType) {
                    case PT_INT:
                        p.vNum = (int)(p.numMinValue + (p.numMaxValue - p.numMinValue) * v);
                        break;
                    case PT_COLOR_L:
                        p.vNum = (int)(255 * v);
                        break;
                    case PT_FLOAT:
                        p.vNum = p.numMinValue + (p.numMaxValue - p.numMinValue) * v;
                        break;
                    default: break;
                }
                updateLabelFn(p, valueLabel);
                onChangedCallback();
            };
            updateLabelFn(p, valueLabel);
        }
        break;
        case PT_COLOR_RGB:
        {
            UIColorInputField* colorInput = new UIColorInputField();
            colorInput->position = XY{ 250, y };
            colorInput->wxHeight = 25;
            colorInput->setColor(p.vU32);
            panel->subWidgets.addDrawable(colorInput);
            colorInput->onColorChangedCallback = [this, i, onChangedCallback](UIColorInputField* p, u32 c) {
                parameters.at(i).vU32 = c;
                onChangedCallback();
            };
        }
        break;
        case PT_INT_RANGE:
        {
            float vl = (p.vNum - p.numMinValue) / (p.numMaxValue - p.numMinValue);
            float vm = (p.vNum2 - p.numMinValue) / (p.numMaxValue - p.numMinValue);
            UIDoubleSlider* slider = new UIDoubleSlider();
            slider->position = XY{ 285, y };
            slider->sliderPos.min = vl;
            slider->sliderPos.max = vm;
            slider->bodyColor = p.vU32;
            slider->wxHeight = 25;
            panel->subWidgets.addDrawable(slider);

            UILabel* valueLabel = new UILabel();
            valueLabel->position = xySubtract(slider->position, { 84, 0 });
            panel->subWidgets.addDrawable(valueLabel);

            slider->onChangeValueCallback = [this, i, valueLabel, onChangedCallback, updateLabelFn](UIDoubleSlider* s, UIDoubleSliderBounds v) {
                Parameter& p = parameters.at(i);
                p.vNum = (int)(p.numMinValue + (p.numMaxValue - p.numMinValue) * v.min);
                p.vNum2 = (int)(p.numMinValue + (p.numMaxValue - p.numMinValue) * v.max);
                updateLabelFn(p, valueLabel);
                onChangedCallback();
            };
            updateLabelFn(p, valueLabel);
        }
        break;
        }
        y += 40;
        i++;
    }
    return panel;
}

Panel* ParameterStore::generateHorizontalUI(std::function<void()> onChangedCallback)
{
	//todo
	return NULL;
}

std::map<std::string, std::string> ParameterStore::buildParameterMap()
{
    std::map<std::string, std::string> ret = {};
    for (auto& p : parameters) {
        switch (p.paramType) {
        case PT_COLOR_RGB:
            ret[p.name] = frmt("{:08X}", p.vU32);
            break;
        case PT_INT_RANGE:
            ret[p.name + ".min"] = std::to_string(p.vNum);
            ret[p.name + ".max"] = std::to_string(p.vNum2);
            break;
        default:
            ret[p.name] = std::to_string(p.vNum);
            break;
        }
    }
    return ret;
}

void ParameterStore::setParametersFromParameterMap(std::map<std::string, std::string> paramMap)
{
    for (Parameter& p : parameters) {
        if (paramMap.contains(p.name)
            || (p.paramType == PT_INT_RANGE && paramMap.contains(p.name + ".min") && paramMap.contains(p.name + ".max"))) {
            try {
                switch (p.paramType) {
                case PT_BOOL:
                    p.vNum = paramMap[p.name] == "1" ? 1 : 0;
                    break;
                case PT_INT:
                case PT_COLOR_L:
                    p.vNum = std::stoi(paramMap[p.name]);
                    break;
                case PT_FLOAT:
                    p.vNum = std::stod(paramMap[p.name]);
                    break;
                case PT_INT_RANGE:
                    p.vNum = std::stoi(paramMap[p.name + ".min"]);
                    p.vNum2 = std::stoi(paramMap[p.name + ".max"]);
                    break;
                case PT_COLOR_RGB:
                    p.vU32 = std::stoi(paramMap[p.name], NULL, 16);
                    break;
                }
            }
            catch (std::exception&) {

            }
        }
    }
}
