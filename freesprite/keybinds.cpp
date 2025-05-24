#include "keybinds.h"
#include "maineditor.h"

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
    g_keybindManager.addKeybind("maineditor", "focus_color_input", 
        KeyCombo(TL("vsp.keybinds.maineditor.focusoncolortextbox"), SDL_SCANCODE_INSERT, true, false, [](void* d) {
            ((MainEditor*)d)->focusOnColorInputTextBox();
        }));
    g_keybindManager.addKeybind("maineditor", "save", 
        KeyCombo(TL("vsp.keybinds.maineditor.save"), SDL_SCANCODE_S, true, false, [](void* d) {
            ((MainEditor*)d)->trySaveImage();
        }));
    g_keybindManager.addKeybind("maineditor", "save_as", 
        KeyCombo(TL("vsp.keybinds.maineditor.saveas"), SDL_SCANCODE_S, true, true, [](void* d) {
            ((MainEditor*)d)->trySaveAsImage();
        }));
    g_keybindManager.addKeybind("maineditor", "layer_rename", 
        KeyCombo(TL("vsp.keybinds.maineditor.renamelayer"), SDL_SCANCODE_F2, false, false, [](void* d) {
            ((MainEditor*)d)->layer_promptRename();
        }));
    g_keybindManager.addKeybind("maineditor", "clear_area", 
        KeyCombo(TL("vsp.keybinds.maineditor.cleararea"), SDL_SCANCODE_DELETE, false, false, [](void* d) {
            ((MainEditor*)d)->layer_clearSelectedArea();
        }));
}
