#pragma once
#include "BasePopup.h"
#include "EventCallbackListener.h"

enum ExportMode : int {
    EXPORTMODE_INTEGERSCALE = 0,
    EXPORTMODE_PIXELSCALE = 1
};

class PopupExportScaled : public BasePopup, public EventCallbackListener
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

    void eventFileSaved(int evt_id, PlatformNativePathString name, int exporterIndex = -1) override;

    void genExporterList();
    void recalc();
};

