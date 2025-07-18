#pragma once

#include <fstream>

#include "../globals.h"
#include "../mathops.h"
#include "../maineditor.h"
#include "../Notification.h"
#include "../MainEditorPalettized.h"
#include "../LayerPalettized.h"

u8* decompressZlib(u8* data, u64 compressedSize, u64 decompressedSize);