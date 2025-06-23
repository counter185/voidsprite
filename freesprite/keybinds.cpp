#include "keybinds.h"
#include "maineditor.h"
#include "SplitSessionEditor.h"
#include "EditorLayerPicker.h"
#include "EditorColorPicker.h"
#include "StartScreen.h"
#include "brush/BaseBrush.h"
#include "main.h"
#include "multiwindow.h"

void g_initKeybinds()
{
    g_keybindManager.globalReservedKeys = {
        SDL_SCANCODE_LALT,
    };

    //global keybinds
    g_keybindManager.newRegion("global", TL("vsp.keybinds.region.global"));
    g_keybindManager.addKeybind("global", "switch_screen_left",
        KeyCombo(TL("vsp.keybinds.global.screenleft"), SDL_SCANCODE_LEFTBRACKET, false, false, [](void* d) {
            g_currentWindow->switchScreenLeft();
        }));
    g_keybindManager.addKeybind("global", "switch_screen_right",
        KeyCombo(TL("vsp.keybinds.global.screenright"), SDL_SCANCODE_RIGHTBRACKET, false, false, [](void* d) {
            g_currentWindow->switchScreenRight();
        }));
    if (platformSupportsFeature(VSP_FEATURE_MULTIWINDOW)) {
        g_keybindManager.addKeybind("global", "new_window",
            KeyCombo(TL("vsp.keybinds.global.newwindow"), SDL_SCANCODE_RIGHT, true, false, [](void* d) {
                main_newWindow("");
            }));
        g_keybindManager.addKeybind("global", "detach_workspace",
            KeyCombo(TL("vsp.keybinds.global.detachworkspace"), SDL_SCANCODE_UP, true, false, [](void* d) {
                main_currentWorkspaceToNewWindow("");
            }));
        g_keybindManager.addKeybind("global", "attach_to_main_window",
            KeyCombo(TL("vsp.keybinds.global.attachtomainwindow"), SDL_SCANCODE_LEFT, true, false, [](void* d) {
                main_attachCurrentWorkspaceToMainWindow();
            }));
    }
    g_keybindManager.addKeybind("global", "set_fav_workspace",
        KeyCombo(TL("vsp.keybinds.global.setfavworkspace"), SDL_SCANCODE_W, true, false, [](void* d) {
            main_assignFavScreen();
        }));
    g_keybindManager.addKeybind("global", "switch_to_fav_workspace",
        KeyCombo(TL("vsp.keybinds.global.switchfavworkspace"), SDL_SCANCODE_W, true, true, [](void* d) {
            main_switchToFavScreen();
        }));
    g_keybindManager.addKeybind("global", "fullscreen",
        KeyCombo(TL("vsp.keybinds.global.fullscreen"), SDL_SCANCODE_F11, false, false, [](void* d) {
            main_toggleFullscreen();
        }));
    g_keybindManager.addKeybind("global", "render_scale_up",
        KeyCombo(TL("vsp.keybinds.global.renderscaleup"), SDL_SCANCODE_EQUALS, true, false, [](void* d) {
            main_renderScaleUp();
        }));
    g_keybindManager.addKeybind("global", "render_scale_down",
        KeyCombo(TL("vsp.keybinds.global.renderscaledown"), SDL_SCANCODE_MINUS, true, false, [](void* d) {
            main_renderScaleDown();
        }));


    //launchpad keybinds
    g_keybindManager.newRegion("startscreen", TL("vsp.keybinds.region.startscreen"));
    g_keybindManager.addKeybind("startscreen", "open_clipboard", 
        KeyCombo(TL("vsp.keybinds.startscreen.openclipboard"),SDL_SCANCODE_V, true, false, [](void* d) {
            ((StartScreen*)d)->tryOpenImageFromClipboard();
        }));


    //main editor keybinds
    g_keybindManager.newRegion("maineditor", TL("vsp.keybinds.region.maineditor"));
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
    g_keybindManager.addKeybind("maineditor", "lock_tile_preview", 
        KeyCombo(TL("vsp.keybinds.maineditor.locktilepreview"), SDL_SCANCODE_Q, true, false, [](void* d) {
            ((MainEditor*)d)->tryToggleTilePreviewLockAtMousePos();
        }));
    g_keybindManager.addKeybind("maineditor", "layer_rename", 
        KeyCombo(TL("vsp.keybinds.maineditor.renamelayer"), SDL_SCANCODE_F2, false, false, [](void* d) {
            ((MainEditor*)d)->layer_promptRename();
        }));
    g_keybindManager.addKeybind("maineditor", "clear_area", 
        KeyCombo(TL("vsp.keybinds.maineditor.cleararea"), SDL_SCANCODE_DELETE, false, false, [](void* d) {
            ((MainEditor*)d)->layer_clearSelectedArea();
        }));
    g_keybindManager.addKeybind("maineditor", "fill_area", 
        KeyCombo(TL("vsp.keybinds.maineditor.fillarea"), SDL_SCANCODE_UNKNOWN, false, false, [](void* d) {
            ((MainEditor*)d)->layer_fillActiveColor();
        }));
    g_keybindManager.addKeybind("maineditor", "new_layer", 
        KeyCombo(TL("vsp.keybinds.maineditor.newlayer"), KEY_UNASSIGNED, false, false, [](void* d) {
            ((MainEditor*)d)->newLayer(); ((MainEditor*)d)->layerPicker->updateLayers();
        }));
    g_keybindManager.addKeybind("maineditor", "duplicate_layer", 
        KeyCombo(TL("vsp.keybinds.maineditor.duplicatelayer"), KEY_UNASSIGNED, false, false, [](void* d) {
            ((MainEditor*)d)->duplicateLayer(((MainEditor*)d)->selLayer); ((MainEditor*)d)->layerPicker->updateLayers();
        }));


    //split session editor keybinds
    g_keybindManager.newRegion("splitsessioneditor", TL("vsp.keybinds.region.splitsessioneditor"));
    g_keybindManager.addKeybind("splitsessioneditor", "import_image",
        KeyCombo(TL("vsp.keybinds.splitsessioneditor.importimage"), SDL_SCANCODE_O, true, false, [](void* d) {
            ((SplitSessionEditor*)d)->promptAddImageToSplitSession();
        }));
    g_keybindManager.addKeybind("splitsessioneditor", "save",
        KeyCombo(TL("vsp.keybinds.splitsessioneditor.save"), SDL_SCANCODE_S, true, false, [](void* d) {
            ((SplitSessionEditor*)d)->trySave(false);
        }));
    g_keybindManager.addKeybind("splitsessioneditor", "save_and_open",
        KeyCombo(TL("vsp.keybinds.splitsessioneditor.saveandopen"), SDL_SCANCODE_Q, true, false, [](void* d) {
            ((SplitSessionEditor*)d)->trySave(true);
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
