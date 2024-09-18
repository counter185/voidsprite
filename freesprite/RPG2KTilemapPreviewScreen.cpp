#include "RPG2KTilemapPreviewScreen.h"
#include "ScreenWideNavBar.h"

RPG2KTilemapPreviewScreen::RPG2KTilemapPreviewScreen(MainEditor* parent)
{
    caller = parent;

    navbar = new ScreenWideNavBar<RPG2KTilemapPreviewScreen*>(this,
        {
            {
                SDLK_f,
                {
                    "File",
                    {},
                    {
                        {SDLK_o, { "Load layout from file",
                                [](RPG2KTilemapPreviewScreen* screen) {
                                    platformTryLoadOtherFile(screen, {{".lmu", "RPGM2000/2003 Map"}}, "Load tile layout", EVENT_OTHERFILE_OPENFILE);
                                }
                            }
                        },
                        /*{SDLK_s, {"Save layout to file",
                                [](TilemapPreviewScreen* screen) {
                                    platformTrySaveOtherFile(screen, { {".voidtile", "voidtile layout"} }, "Save tile layout", EVENT_OTHERFILE_SAVEFILE);
                                }
                            }
                        },*/
                    },
                    g_iconNavbarTabFile
                }
            }
        }, { SDLK_f });
    wxsManager.addDrawable(navbar);

    //resizeTilemap(32, 32);
}

RPG2KTilemapPreviewScreen::~RPG2KTilemapPreviewScreen()
{
}

void RPG2KTilemapPreviewScreen::render()
{
}

void RPG2KTilemapPreviewScreen::tick()
{
}

