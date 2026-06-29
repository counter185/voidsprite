#include "io_base.h"
#include "io_pep.h"

extern "C" {
#define PEP_IMPLEMENTATION 1
#include "../pep/pep.h"
}

Layer* readPEP(PlatformNativePathString path, u64 seek) {
	FILE* f = platformOpenFile(path, PlatformFileModeRB);
	if (f != NULL) {
		DoOnReturn closeFile([f]() {fclose(f); });

		std::vector<u8> fdata = readWholeFile(f);
		if (fdata.size() > 0) {
			pep p = pep_deserialize(fdata.data(), fdata.size());
			DoOnReturn freePep([p]() {pep_free((pep*)&p); });
			if (p.bytes != NULL) {
				u32* pepPixels = pep_decompress(&p, pep_bgra, 0, 0);
				//what do i do with pepPixels?
				if (pepPixels != NULL) {
					Layer* l = Layer::tryAllocLayer(p.width, p.height);
					if (l != NULL) {
						l->name = "PEP Layer";
						memcpy(l->pixels32(), pepPixels, l->w * l->h * 4);
						/*std::vector<u32> newPalette;
						for (u32& col : p.palette) {
							newPalette.push_back(col);
						}
						newPalette.resize(p.palette_size);
						l->palette = newPalette;*/

						return l;
					}
				}
			}
		}
	}
	return NULL;
}

bool writePEP(PlatformNativePathString path, Layer* l) {
	int uqColors = l->numUniqueColors();
	if (uqColors > 256) {
		g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), frmt("Too many colors for PEP ({} > max 256)", uqColors)));
		return false;
	}

	FILE* f = platformOpenFile(path, PlatformFileModeWB);
	if (f != NULL) {
		DoOnReturn closeFile([f]() {fclose(f); });

		pep p = pep_compress(l->pixels32(), l->w, l->h, pep_bgra, pep_8bit);
		DoOnReturn freeP([p]() {pep_free((pep*)&p); });

		u32 outSize = 0;
		u8* bytes = pep_serialize(&p, &outSize);

		if (bytes != NULL) {
			fwrite(bytes, 1, outSize, f);
			return true;
		}
	}
	return false;
}