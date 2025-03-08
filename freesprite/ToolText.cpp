#include "ToolText.h"
#include "PopupTextTool.h"
#include "UITextField.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "Notification.h"

void ToolText::clickPress(MainEditor* editor, XY pos)
{
	if (textSurface != NULL) {
		uint8_t* pixels = (uint8_t*)textSurface->pixels;//(uint8_t*)tracked_malloc(textSurface->w * textSurface->h * 4);
		int pitch = textSurface->w * 4;
		//SDL_ConvertPixels(textSurface->w, textSurface->h, SDL_PIXELFORMAT_ARGB8888, textSurface->pixels, textSurface->pitch, SDL_PIXELFORMAT_ARGB8888, pixels, pitch);
		for (int y = 0; y < textSurface->h; y++) {
			for (int x = 0; x < textSurface->w; x++) {
				//uint32_t* ppx = ((uint32_t*)pixels);
				if (((uint32_t*)pixels)[y * textSurface->w + x] == 0) {
					continue;
				}
				//uint32_t pixel = *(ppx + (y * textSurface->w) + x);
				uint32_t colorModifier = editor->pickedColor;
				//pixel = (pixel & 0xff000000) | (colorModifier & 0xffffff);

				editor->SetPixel(XY{ pos.x + x, pos.y + y }, editor->isPalettized ? ((MainEditorPalettized*)editor)->pickedPaletteIndex : editor->pickedColor);
			}
		}
		//tracked_free(pixels);
	}
}

void ToolText::rightClickPress(MainEditor* editor, XY pos)
{
	PopupTextTool* popup = new PopupTextTool(this, "Text", "Enter text:");
	g_addPopup(popup);
}

void ToolText::renderOnCanvas(XY canvasDrawPoint, int scale)
{
	if (textSurface != NULL && cachedTextTexture != NULL) {
		SDL_Rect textRect = SDL_Rect{
			canvasDrawPoint.x + lastMouseMotionPos.x * scale,
			canvasDrawPoint.y + lastMouseMotionPos.y * scale,
			textSurface->w * scale,
			textSurface->h * scale
		};
		SDL_SetTextureAlphaMod(cachedTextTexture, 0x80);
		SDL_RenderCopy(g_rd, cachedTextTexture, NULL, &textRect);
		SDL_SetRenderDrawColor(g_rd, 0xff, 0xff, 0xff, 0x30);
		SDL_RenderDrawRect(g_rd, &textRect);
	}
}

void ToolText::eventPopupClosed(int evt_id, BasePopup* target)
{
	if (evt_id == EVENT_TOOLTEXT_POSTCONFIG) {
		PopupTextTool* popup = (PopupTextTool*)target;
		text = popup->textbox->getText();
		textSize = popup->textSize;
		renderText();
	}
}

void ToolText::renderText()
{
	if (textSurface != NULL) {
		SDL_FreeSurface(textSurface);
		textSurface = NULL;
	}
	if (cachedTextTexture != NULL) {
		tracked_destroyTexture(cachedTextTexture);
		cachedTextTexture = NULL;
	}

	if (font == NULL) {
		//return;
		font = TTF_OpenFont(FONT_PATH, 20);
		//todo
	}

	if (font != NULL) {
		TTF_SetFontSize(font, textSize);
		textSurface = TTF_RenderText_Solid(font, text.c_str(), 0, { 255, 255, 255, 255 });
		if (textSurface != NULL) {
			SDL_Surface* converted = SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_ARGB8888);
			SDL_FreeSurface(textSurface);
			textSurface = converted;
			cachedTextTexture = tracked_createTextureFromSurface(g_rd, textSurface);
		}
		else {
			g_addNotification(ErrorNotification("Error", "Failed to render text"));
		}
	}
	else {
		g_addNotification(ErrorNotification("TTF Error", "Error loading font"));
	}

}
