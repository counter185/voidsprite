#include "BrushGradient.h"
#include "../Notification.h"

void BrushGradient::lineSelected(MainEditor* editor, XY start, XY end)
{
	if (!editor->isPalettized) {
		double maxDistance = xyDistance(start, end);

		u32 color = editor->getActiveColor();

		g_startNewOperation([maxDistance, color, editor, start]() {
			for (int y = 0; y < editor->canvas.dimensions.y; y++) {
				for (int x = 0; x < editor->canvas.dimensions.x; x++) {
					double dist = xyDistance({ x,y }, start);
					if (dist <= maxDistance) {
						double percent = 1.0 - (dist / maxDistance);
						editor->SetPixel({ x,y }, modAlpha(color, (u8)(255 * percent)), false);
					}
				}
			}
		});
	}
	else {
		g_addNotification(ErrorNotification(TL("vsp.cmn.error"), "Cannot make gradient in indexed mode."));
	}
}
