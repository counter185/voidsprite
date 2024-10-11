#include "ToolGuideline.h"
#include "TooltipsLayer.h"
#include "maineditor.h"

void ToolGuideline::clickPress(MainEditor* editor, XY pos)
{
    mouseLeftHeld = true;
}

void ToolGuideline::clickDrag(MainEditor* editor, XY from, XY to)
{
}

void ToolGuideline::clickRelease(MainEditor* editor, XY pos)
{
    if (mouseLeftHeld) {
        int symXPos = lastMouseMotionPos.x / 2;
        bool symXMiddle = lastMouseMotionPos.x % 2;

        if (editor->eraserMode)
        {
            for (int x = 0; x < editor->guidelines.size(); x++) {
                if (editor->guidelines[x].vertical && editor->guidelines[x].position == lastMouseMotionPos.x) {
                    editor->guidelines.erase(editor->guidelines.begin() + x);
                    break;
                }
            }
        }
        else {
            bool exists = false;
            for (int x = 0; x < editor->guidelines.size(); x++) {
                if (editor->guidelines[x].vertical && editor->guidelines[x].position == lastMouseMotionPos.x) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                editor->guidelines.push_back(Guideline{true, lastMouseMotionPos.x});
            }
        }
    }
    mouseLeftHeld = false;
}

void ToolGuideline::rightClickPress(MainEditor* editor, XY pos)
{
    mouseRightHeld = true;
}

void ToolGuideline::rightClickRelease(MainEditor* editor, XY pos)
{
    if (mouseRightHeld) {
        int symYPos = lastMouseMotionPos.y / 2;
        bool symYMiddle = lastMouseMotionPos.y % 2;

        if (editor->eraserMode)
        {
            for (int x = 0; x < editor->guidelines.size(); x++) {
                if (!editor->guidelines[x].vertical && editor->guidelines[x].position == lastMouseMotionPos.y) {
                    editor->guidelines.erase(editor->guidelines.begin() + x);
                    break;
                }
            }
        }
        else {
            bool exists = false;
            for (int x = 0; x < editor->guidelines.size(); x++) {
                if (!editor->guidelines[x].vertical && editor->guidelines[x].position == lastMouseMotionPos.y) {
                    exists = true;
                    break;
                }
            }
            if (!exists) {
                editor->guidelines.push_back(Guideline{ false, lastMouseMotionPos.y });
            }
        }
    }
    mouseRightHeld = false;
}

void ToolGuideline::renderOnCanvas(XY canvasDrawPoint, int scale)
{
    int symXPos = lastMouseMotionPos.x / 2;
    bool symXMiddle = lastMouseMotionPos.x % 2;
    int lineDrawXPoint = canvasDrawPoint.x + symXPos * scale + (symXMiddle ? scale / 2 : 0);

    if (mouseLeftHeld) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x30);
        SDL_RenderDrawLine(g_rd, lineDrawXPoint, 0, lineDrawXPoint, g_windowH);
        g_ttp->addTooltip(Tooltip{ {g_mouseX + 20, g_mouseY - 40 }, std::format("{}{}", symXPos, symXMiddle ? ".5" : "")});
    }

    int symYPos = lastMouseMotionPos.y / 2;
    bool symYMiddle = lastMouseMotionPos.y % 2;
    int lineDrawYPoint = canvasDrawPoint.y + symYPos * scale + (symYMiddle ? scale / 2 : 0);

    if (mouseRightHeld) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x20);
        SDL_RenderDrawLine(g_rd, 0, lineDrawYPoint, g_windowW, lineDrawYPoint);
        g_ttp->addTooltip(Tooltip{ {g_mouseX + 20, g_mouseY + 40 }, std::format("{}{}", symYPos, symYMiddle ? ".5" : "") });
    }
}
