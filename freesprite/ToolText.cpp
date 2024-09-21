#include "ToolText.h"
#include "PopupTextTool.h"
#include "UITextField.h"
#include "maineditor.h"

void ToolText::clickPress(MainEditor* editor, XY pos)
{
	if (textSurface != NULL) {
		uint8_t* pixels = (uint8_t*)malloc(textSurface->w * textSurface->h * 4);
		int pitch = textSurface->w * 4;
		SDL_ConvertPixels(textSurface->w, textSurface->h, SDL_PIXELFORMAT_ARGB8888, textSurface->pixels, textSurface->pitch, SDL_PIXELFORMAT_ARGB8888, pixels, pitch);
		for (int y = 0; y < textSurface->h; y++) {
			for (int x = 0; x < textSurface->w; x++) {
				uint32_t* ppx = ((uint32_t*)pixels);
				uint32_t pixel = *(ppx + (y * textSurface->w) + x);
				uint32_t colorModifier = editor->pickedColor;
				pixel = (pixel & 0xff000000) | (colorModifier & 0xffffff);

				editor->SetPixel(XY{ pos.x + x, pos.y + y }, pixel);
			}
		}
		free(pixels);
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
		text = popup->textbox->text;
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
		SDL_DestroyTexture(cachedTextTexture);
		cachedTextTexture = NULL;
	}

	if (font == NULL) {
		//return;
		font = TTF_OpenFont(FONT_PATH, 20);
		//todo
	}

	TTF_SetFontSize(font, textSize);
	textSurface = TTF_RenderUTF8_Blended(font, text.c_str(), { 255, 255, 255, 255 });
	if (textSurface != NULL) {
		cachedTextTexture = SDL_CreateTextureFromSurface(g_rd, textSurface);
	}

}
