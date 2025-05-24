#include "io_base.h"
#include "io_valve_spr.h"

Layer* readValveSPR(PlatformNativePathString path, uint64_t seek)
{
	FILE* f = platformOpenFile(path, PlatformFileModeRB);
	if (f != NULL) {
		SPR_sprite spr;
		fread(&spr, sizeof(SPR_sprite), 1, f);
		if (spr.id != 0x50534449) {
			logerr(std::format("[SPR] Invalid file format {:08X}\n", spr.id));
			fclose(f);
			return NULL;
		}
		loginfo(std::format("[SPR] frame count: {}", spr.frameNum));
		loginfo(std::format("[SPR] sprite type: {}", spr.spriteType));
		loginfo(std::format("[SPR] transparency format: {}", (u32)spr.textFormat));
		loginfo(std::format("[SPR] colors in palette: {}", spr.paletteColorCount));
		//assuming version 2
		std::vector<u32> palette;
		for (short i = 0; i < spr.paletteColorCount; i++) {
			u8 r, g, b;
			fread(&r, 1, 1, f);
			fread(&g, 1, 1, f);
			fread(&b, 1, 1, f);
			palette.push_back(PackRGBAtoARGB(r, g, b, spr.textFormat == SPR_Transparency_IndexAlpha ? i : 255));
		}

		if (spr.textFormat == SPR_Transparency_AlphaTest && !palette.empty()) {
			palette[palette.size() - 1] &= 0xFFFFFF;
		}

		Layer* ret = NULL;

		for (u32 fr = 0; fr < spr.frameNum; fr++) {
			SPR_sprite_frame_header frame;
			fread(&frame, sizeof(SPR_sprite_frame_header), 1, f);

			LayerPalettized* l = LayerPalettized::tryAllocIndexedLayer(frame.width, frame.height);
			ret = l;
			if (l == NULL) {
				logerr(std::format("[SPR] Failed to allocate layer {}x{}\n", frame.width, frame.height));
			}
			else {
				l->name = "Valve SPR Layer";
				l->palette = palette;
				u32* ppx = (u32*)l->pixelData;
				for (u32 y = 0; y < frame.height; y++) {
					for (u32 x = 0; x < frame.width; x++) {
						u8 px;
						fread(&px, 1, 1, f);
						ARRAY2DPOINT(ppx, x, y, l->w) = px;
					}
				}
			}

			//todo: multiple frames once we get there
			break;
		}

		fclose(f);
		return ret;
	}
	return NULL;
}
