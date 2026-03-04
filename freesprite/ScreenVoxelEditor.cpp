#include "ScreenVoxelEditor.h"
#include "FontRenderer.h"

void ScreenVoxelEditor::render() {

    renderWithBlurPanelsIfEnabled([this]() {
        renderGradient({ 0,0,g_windowW,g_windowH }, 0xFF000000, 0xFF000000, 0xFF000000, 0xFF202020);

        renderModel();
    });

    g_fnt->RenderString(frmt("targetpoint: {}, {}, {}", mouseVoxelTargetPoint.x, mouseVoxelTargetPoint.y, mouseVoxelTargetPoint.z), 5, 5);

    BaseScreen::render();
}

void ScreenVoxelEditor::takeInput(SDL_Event evt) {
    if (evt.type == SDL_EVENT_QUIT) {
        g_closeScreen(this);
        return;
    }

    //LALT_TO_SUMMON_NAVBAR;

    DrawableManager::processHoverEventInMultiple({ wxsManager }, evt);
    if (!DrawableManager::processInputEventInMultiple({ wxsManager }, evt)) {

        evt = convertTouchToMouseEvent(evt);

        if (evt.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            if (mousePlacing) {
                if (evt.button.button == SDL_BUTTON_LEFT) {
                    recalcMouseVoxelTargetPoint({ (int)evt.button.x, (int)evt.button.y });
                    placeVoxel(mouseVoxelTargetPoint);
                }
            } else {
                dragging = evt.button.button;
            }
        }
        else if (evt.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            dragging = -1;
        }
        else if (evt.type == SDL_EVENT_MOUSE_MOTION) {
            recalcMouseVoxelTargetPoint({ (int)evt.motion.x, (int)evt.motion.y });
            if (dragging == SDL_BUTTON_LEFT) {
                rotateFromMouseInput(evt.motion.xrel, evt.motion.yrel);
            }
            else if (dragging == SDL_BUTTON_MIDDLE || dragging == SDL_BUTTON_RIGHT) {
                screen00 = xyAdd(screen00, { (int)evt.motion.xrel, (int)evt.motion.yrel });
            }
        }
        else if (evt.type == SDL_EVENT_MOUSE_WHEEL) {
            size += evt.wheel.y;
            size = dclamp(5, size, 100);
        }
        else if (evt.type == SDL_EVENT_KEY_DOWN) {
            if (evt.key.scancode == SDL_SCANCODE_SPACE) {
                mousePlacing = !mousePlacing;
            }
        }
    }
}

void ScreenVoxelEditor::recalcMouseVoxelTargetPoint(XY mousePos) {
    XY relativePos = xySubtract(mousePos, screen00);
    XYZd worldPos = screenSpaceToWorldSpace({relativePos.x / size, relativePos.y / size}, rotAlpha * M_PI / 180, rotBeta * M_PI / 180);
    worldPos = { floor(worldPos.x), floor(worldPos.y), floor(worldPos.z) };
    mouseVoxelTargetPoint = worldPos;
}

void ScreenVoxelEditor::placeVoxel(XYZd pos) {
    voxels.push_back(Voxel{ pos, uint32ToSDLColor(currentColor) });
}

void ScreenVoxelEditor::renderModel() {

    renderFloorGrid();
    for (const Voxel& v : voxels) {
        renderBox(v.color, screen00, size, v.pos, 1, 1, 1, 0.5);
    }
    renderBox({255,255,255, 0x60}, screen00, size, mouseVoxelTargetPoint, 1,1,1,0);

}
