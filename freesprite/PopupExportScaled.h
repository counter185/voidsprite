#pragma once
#include "BasePopup.h"

enum ExportMode : int {
    EXPORTMODE_INTEGERSCALE = 0,
    EXPORTMODE_PIXELSCALE = 1
};

class PopupExportScaled : public BasePopup
{
private:
    MainEditor* caller = NULL;
    ExportMode mode = EXPORTMODE_INTEGERSCALE;

    UILabel* labelOutputSize = NULL;

    UITextField* tboxISX, *tboxISY, *tboxPXSX, *tboxPXSY;
    XY resultSize = { 0,0 };

    std::vector<FileExporter*> exporterList;
public:
    PopupExportScaled(MainEditor* parent);

    bool exportWithExporter(FileExporter* exporter, PlatformNativePathString path);
    void genExporterList();
    void recalc();
};

