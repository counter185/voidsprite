cmake_minimum_required(VERSION 3.6)

project(my_app)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -fpermissive -Wno-c++11-narrowing -Wno-macro-redefined -Wno-switch -Wno-inconsistent-missing-override -Wno-constant-conversion")

add_definitions(-DVOIDSPRITE_JXL_ENABLED=0)
if(DEFINED ENV{GIT_HASH})
    add_definitions(-DGIT_HASH="$ENV{GIT_HASH}")
endif()
if(DEFINED ENV{GIT_BRANCH})
    add_definitions(-DGIT_BRANCH="$ENV{GIT_BRANCH}")
endif()

if(NOT TARGET SDL3::SDL3)
    find_package(SDL3 CONFIG)
endif()
if(NOT TARGET SDL3_ttf::SDL3_ttf)
    find_package(SDL3_ttf CONFIG)
endif()
if(NOT TARGET SDL3_image::SDL3_image)
    find_package(SDL3_image CONFIG)
endif()

if(NOT TARGET liblcf)
    find_package(liblcf CONFIG)
endif()
if(NOT TARGET zlib)
    find_package(zlib CONFIG)
endif()

if(NOT TARGET SDL3::SDL3)
    find_library(SDL3_LIBRARY NAMES "SDL3")
    find_path(SDL3_INCLUDE_DIR NAMES "SDL3/SDL.h")
    add_library(SDL3::SDL3 UNKNOWN IMPORTED)
    set_property(TARGET SDL3::SDL3 PROPERTY IMPORTED_LOCATION "${SDL3_LIBRARY}")
    set_property(TARGET SDL3::SDL3 PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIR}")
endif()
if(NOT TARGET SDL3_ttf::SDL3_ttf)
    find_library(SDL3_TTF_LIBRARY NAMES "SDL3_ttf")
    find_path(SDL3_TTF_INCLUDE_DIR NAMES "SDL3_ttf/SDL_ttf.h")
    add_library(SDL3_ttf::SDL3_ttf UNKNOWN IMPORTED)
    set_property(TARGET SDL3_ttf::SDL3_ttf PROPERTY IMPORTED_LOCATION "${SDL3_TTF_LIBRARY}")
    set_property(TARGET SDL3_ttf::SDL3_ttf PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_TTF_INCLUDE_DIR}")
endif()
if(NOT TARGET SDL3_image::SDL3_image)
    find_library(SDL3_IMAGE_LIBRARY NAMES "SDL3_image")
    find_path(SDL3_IMAGE_INCLUDE_DIR NAMES "SDL3_image/SDL_image.h")
    add_library(SDL3_image::SDL3_image UNKNOWN IMPORTED)
    set_property(TARGET SDL3_image::SDL3_image PROPERTY IMPORTED_LOCATION "${SDL3_IMAGE_LIBRARY}")
    set_property(TARGET SDL3_image::SDL3_image PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SDL3_IMAGE_INCLUDE_DIR}")
endif()

if(NOT TARGET SDL3::SDL3)
    message(FATAL_ERROR "Cannot find SDL3.

Possible ways to fix this:
- Use a SDL3 Android aar archive, and configure gradle to use it: prefab is required.
- Add add_subdirectory(path/to/SDL) to your CMake script, and make sure a vendored SDL is present there.
")
endif()

add_library(main SHARED
        voidsprite/astc_dec/astc_decomp.cpp
        voidsprite/easybmp/EasyBMP.cpp
        voidsprite/libpng/png.c
        voidsprite/libpng/pngerror.c
        voidsprite/libpng/pngget.c
        voidsprite/libpng/pngmem.c
        voidsprite/libpng/pngpread.c
        voidsprite/libpng/pngread.c
        voidsprite/libpng/pngrio.c
        voidsprite/libpng/pngrtran.c
        voidsprite/libpng/pngrutil.c
        voidsprite/libpng/pngset.c
        voidsprite/libpng/pngtrans.c
        voidsprite/libpng/pngwio.c
        voidsprite/libpng/pngwrite.c
        voidsprite/libpng/pngwtran.c
        voidsprite/libpng/pngwutil.c
        voidsprite/libtga/tga.c
        voidsprite/libtga/tgaread.c
        voidsprite/libtga/tgawrite.c
        voidsprite/localization/localization.cpp
        voidsprite/pugixml/pugixml.cpp
        voidsprite/zip/zip.c
        voidsprite/BaseFilter.cpp
        voidsprite/BasePopup.cpp
        voidsprite/BaseScreen.cpp
        voidsprite/BaseTemplate.cpp
        voidsprite/brush/BaseBrush.cpp
        voidsprite/brush/Brush1pxLine.cpp
        voidsprite/brush/Brush1pxLinePathfind.cpp
        voidsprite/brush/Brush1x1.cpp
        voidsprite/brush/Brush1x1ArcX.cpp
        voidsprite/brush/Brush1x1ArcY.cpp
        voidsprite/brush/Brush3pxCircle.cpp
        voidsprite/brush/Brush9SegmentRect.cpp
        voidsprite/brush/BrushBezierLine.cpp
        voidsprite/brush/BrushCircle.cpp
        voidsprite/brush/BrushDiamond.cpp
        voidsprite/brush/BrushFill.cpp
        voidsprite/brush/BrushRect.cpp
        voidsprite/brush/BrushRectFill.cpp
        voidsprite/brush/BrushReplaceColor.cpp
        voidsprite/brush/ToolColorPicker.cpp
        voidsprite/brush/ToolComment.cpp
        voidsprite/brush/ToolGuideline.cpp
        voidsprite/brush/ToolMeasure.cpp
        voidsprite/brush/ToolRectClone.cpp
        voidsprite/brush/ToolRectFlip.cpp
        voidsprite/brush/ToolRectIsolate.cpp
        voidsprite/brush/ToolRectMove.cpp
        voidsprite/brush/ToolRectRotate.cpp
        voidsprite/brush/ToolRectSwap.cpp
        voidsprite/brush/ToolSetXSymmetry.cpp
        voidsprite/brush/ToolSetYSymmetry.cpp
        voidsprite/brush/ToolText.cpp
        voidsprite/ButtonStartScreenSession.cpp
        voidsprite/Canvas.cpp
        voidsprite/CollapsableDraggablePanel.cpp
        voidsprite/colors.cpp
        voidsprite/CustomTemplate.cpp
        voidsprite/DraggablePanel.cpp
        voidsprite/drawable.cpp
        voidsprite/DrawableManager.cpp
        voidsprite/EditorBrushPicker.cpp
        voidsprite/EditorColorPicker.cpp
        voidsprite/EditorLayerPicker.cpp
        voidsprite/EditorSpritesheetPreview.cpp
        voidsprite/EditorTouchToggle.cpp
        voidsprite/ExtractDataScreen.cpp
        voidsprite/FileIO.cpp
        voidsprite/fills.cpp
        voidsprite/FontRenderer.cpp
        voidsprite/Gamepad.cpp
        voidsprite/io/io_aseprite.cpp
        voidsprite/io/io_dibv5.cpp
        voidsprite/io/io_gim.cpp
        voidsprite/io/io_jxl.cpp
        voidsprite/io/io_piskel.cpp
        voidsprite/io/io_png.cpp
        voidsprite/io/io_rpgm.cpp
        voidsprite/io/io_valve_spr.cpp
        voidsprite/io/io_voidsprite.cpp
        voidsprite/keybinds.cpp
        voidsprite/Layer.cpp
        voidsprite/LayerPalettized.cpp
        voidsprite/log.cpp
        voidsprite/main.cpp
        voidsprite/maineditor.cpp
        voidsprite/MainEditorPalettized.cpp
        voidsprite/mathops.cpp
        voidsprite/MinecraftBlockPreviewScreen.cpp
        voidsprite/MinecraftSkinPreviewScreen.cpp
        voidsprite/multiwindow.cpp
        voidsprite/NineSegmentPatternEditorScreen.cpp
        voidsprite/Notification.cpp
        voidsprite/NotifSound.cpp
        voidsprite/PalettizedEditorColorPicker.cpp
        voidsprite/PalettizedEditorLayerPicker.cpp
        voidsprite/Panel.cpp
        voidsprite/PanelMCBlockPreview.cpp
        voidsprite/PanelReference.cpp
        voidsprite/PanelRPG2KTilemapPreview.cpp
        voidsprite/PanelSpritesheetPreview.cpp
        voidsprite/PanelTilemapPreview.cpp
        voidsprite/Pattern.cpp
        voidsprite/platform.cpp
        voidsprite/PopupApplyFilter.cpp
        voidsprite/PopupChooseExtsToAssoc.cpp
        voidsprite/PopupExportScaled.cpp
        voidsprite/PopupGlobalConfig.cpp
        voidsprite/PopupIntegerScale.cpp
        voidsprite/PopupListRecoveryAutosaves.cpp
        voidsprite/PopupMessageBox.cpp
        voidsprite/PopupPickColor.cpp
        voidsprite/PopupQuickConvert.cpp
        voidsprite/PopupSet9SPattern.cpp
        voidsprite/PopupSetEditorPixelGrid.cpp
        voidsprite/PopupTextBox.cpp
        voidsprite/PopupTextTool.cpp
        voidsprite/PopupTileGeneric.cpp
        voidsprite/PopupYesNo.cpp
        voidsprite/PopupFilePicker.cpp
        voidsprite/RenderFilter.cpp
        voidsprite/RPG2KTilemapPreviewScreen.cpp
        voidsprite/ScreenNonogramPlayer.cpp
        voidsprite/ScreenWideActionBar.cpp
        voidsprite/ScreenWideNavBar.cpp
        voidsprite/ScrollingPanel.cpp
        voidsprite/settings.cpp
        voidsprite/sdk_impl.cpp
        voidsprite/SplitSessionEditor.cpp
        voidsprite/SpritesheetPreviewScreen.cpp
        voidsprite/StartScreen.cpp
        voidsprite/TabbedView.cpp
        voidsprite/TemplateMC64x32Skin.cpp
        voidsprite/TemplateRPG2KBattleAnim.cpp
        voidsprite/TemplateRPG2KCharset.cpp
        voidsprite/TemplateRPG2KChipset.cpp
        voidsprite/TemplateRPG2KFaceset.cpp
        voidsprite/TemplateRPG2KSystem.cpp
        voidsprite/TilemapEditorLayerPicker.cpp
        voidsprite/TilemapPreviewScreen.cpp
        voidsprite/Timer64.cpp
        voidsprite/TooltipsLayer.cpp
        voidsprite/UIButton.cpp
        voidsprite/UICheckbox.cpp
        voidsprite/UIColorInputField.cpp
        voidsprite/UIColorSlider.cpp
        voidsprite/UIColorPicker.cpp
        voidsprite/UIDoubleSlider.cpp
        voidsprite/UIDropdown.cpp
        voidsprite/UIHueSlider.cpp
        voidsprite/UILabel.cpp
        voidsprite/UILayerButton.cpp
        voidsprite/UISlider.cpp
        voidsprite/UISVPicker.cpp
        voidsprite/UITextField.cpp
        voidsprite/UtilPathfind.cpp
        voidsprite/vfx.cpp
        voidsprite/visual_config.cpp
)
target_link_libraries(main PRIVATE SDL3::SDL3)
target_link_libraries(main PRIVATE SDL3_ttf::SDL3_ttf)
target_link_libraries(main PRIVATE SDL3_image::SDL3_image)
target_link_libraries(main PRIVATE liblcf)
target_link_libraries(main PRIVATE ZLIB::ZLIB)