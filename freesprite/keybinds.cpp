#include "keybinds.h"
#include "maineditor.h"
#include "EditorLayerPicker.h"
#include "BaseBrush.h"

void g_initKeybinds()
{
    g_keybindManager.globalReservedKeys = {
        SDL_SCANCODE_LEFTBRACKET,
        SDL_SCANCODE_RIGHTBRACKET,
        SDL_SCANCODE_F11,
        SDL_SCANCODE_LALT,
    };

    //main editor keybinds
    g_keybindManager.regions["maineditor"].displayName = TL("vsp.keybinds.region.maineditor");
    g_keybindManager.regions["maineditor"].reservedKeys = {
        SDL_SCANCODE_Q,
    };
    g_keybindManager.addKeybind("maineditor", "toggle_eraser", 
        KeyCombo(TL("vsp.keybinds.maineditor.eraser"), SDL_SCANCODE_E, false, false, [](void* d) {
            ((MainEditor*)d)->colorPicker->eraserButton->click();
        }));
    g_keybindManager.addKeybind("maineditor", "toggle_blendmode", 
        KeyCombo(TL("vsp.keybinds.maineditor.blend"), SDL_SCANCODE_INSERT, false, false, [](void* d) {
            if (((MainEditor*)d)->colorPicker->blendModeButton != NULL)  ((MainEditor*)d)->colorPicker->blendModeButton->click();
        }));
    g_keybindManager.addKeybind("maineditor", "zoom_in_canvas", 
        KeyCombo(TL("vsp.keybinds.maineditor.zoomin"), KEY_UNASSIGNED, false, false, [](void* d) {
            ((MainEditor*)d)->zoom(1);
        }));
    g_keybindManager.addKeybind("maineditor", "zoom_out_canvas", 
        KeyCombo(TL("vsp.keybinds.maineditor.zoomout"), KEY_UNASSIGNED, false, false, [](void* d) {
            ((MainEditor*)d)->zoom(-1);
        }));
    g_keybindManager.addKeybind("maineditor", "focus_color_input", 
        KeyCombo(TL("vsp.keybinds.maineditor.focusoncolortextbox"), SDL_SCANCODE_INSERT, true, false, [](void* d) {
            ((MainEditor*)d)->focusOnColorInputTextBox();
        }));
    g_keybindManager.addKeybind("maineditor", "copy_layer", 
        KeyCombo(TL("vsp.keybinds.maineditor.copylayer"), SDL_SCANCODE_C, true, false, [](void* d) {
            ((MainEditor*)d)->copyLayerToClipboard(((MainEditor*)d)->getCurrentLayer());
        }));
    g_keybindManager.addKeybind("maineditor", "copy_image", 
        KeyCombo(TL("vsp.keybinds.maineditor.copyimage"), SDL_SCANCODE_C, true, true, [](void* d) {
            ((MainEditor*)d)->copyImageToClipboard();
        }));
    g_keybindManager.addKeybind("maineditor", "save", 
        KeyCombo(TL("vsp.keybinds.maineditor.save"), SDL_SCANCODE_S, true, false, [](void* d) {
            ((MainEditor*)d)->trySaveImage();
        }));
    g_keybindManager.addKeybind("maineditor", "save_as", 
        KeyCombo(TL("vsp.keybinds.maineditor.saveas"), SDL_SCANCODE_S, true, true, [](void* d) {
            ((MainEditor*)d)->trySaveAsImage();
        }));
    g_keybindManager.addKeybind("maineditor", "undo", 
        KeyCombo(TL("vsp.keybinds.maineditor.undo"), SDL_SCANCODE_Z, true, false, [](void* d) {
            ((MainEditor*)d)->undo();
        }));
    g_keybindManager.addKeybind("maineditor", "redo", 
        KeyCombo(TL("vsp.keybinds.maineditor.redo"), SDL_SCANCODE_Z, true, true, [](void* d) {
            ((MainEditor*)d)->redo();
        }));
    g_keybindManager.addKeybind("maineditor", "redo_alt", 
        KeyCombo(TL("vsp.keybinds.maineditor.redoalt"), SDL_SCANCODE_Y, true, false, [](void* d) {
            ((MainEditor*)d)->redo();
        }));
    g_keybindManager.addKeybind("maineditor", "layer_rename", 
        KeyCombo(TL("vsp.keybinds.maineditor.renamelayer"), SDL_SCANCODE_F2, false, false, [](void* d) {
            ((MainEditor*)d)->layer_promptRename();
        }));
    g_keybindManager.addKeybind("maineditor", "clear_area", 
        KeyCombo(TL("vsp.keybinds.maineditor.cleararea"), SDL_SCANCODE_DELETE, false, false, [](void* d) {
            ((MainEditor*)d)->layer_clearSelectedArea();
        }));
    g_keybindManager.addKeybind("maineditor", "new_layer", 
        KeyCombo(TL("vsp.keybinds.maineditor.newlayer"), KEY_UNASSIGNED, false, false, [](void* d) {
            ((MainEditor*)d)->newLayer(); ((MainEditor*)d)->layerPicker->updateLayers();
        }));
    g_keybindManager.addKeybind("maineditor", "duplicate_layer", 
        KeyCombo(TL("vsp.keybinds.maineditor.duplicatelayer"), KEY_UNASSIGNED, false, false, [](void* d) {
            ((MainEditor*)d)->duplicateLayer(((MainEditor*)d)->selLayer); ((MainEditor*)d)->layerPicker->updateLayers();
        }));

    int i = 0;
    for (auto& brush : g_brushes) {
        KeyCombo kc = KeyCombo(std::format("{}: {}", TL("vsp.keybinds.maineditor.switchtobrush"), brush->getName()),
            KEY_UNASSIGNED, false, false, [brush](void* d) {
                ((MainEditor*)d)->setActiveBrush(brush);
            }
        );
        kc.icon = brush->cachedIcon;
        g_keybindManager.addKeybind("maineditor", std::format("brush_{}", i++), kc);
    }


    for (std::string& kb : g_config.keybinds) {
        g_keybindManager.deserializeKeybindLine(kb);
    }
}
