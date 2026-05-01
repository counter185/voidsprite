#pragma once
#include "Panel.h"

class HAlign;

class HAlignPoint : public Drawable {
public:
    int w = 0;
    bool xPointSet = false;
    int originalXPoint = 0;
    ~HAlignPoint();
    HAlign* parent = NULL;

    XY getDimensions() override;
};

class HAlign {
public:
    int refCount = 0;
    UIStackPanel* target = NULL;
    std::vector<HAlignPoint*> alignPoints;
    bool updatePoints = true;

    HAlignPoint* alignPoint() {
        refCount++;
        HAlignPoint* p = new HAlignPoint();
        p->parent = this;
        alignPoints.push_back(p);
        updatePoints = true;
        return p;
    }

    void updateAlignPoints() {
        if (updatePoints) {
            int maxX = 0;
            for (HAlignPoint* p : alignPoints) {
                maxX = ixmax(maxX, p->originalXPoint);
            }
            for (HAlignPoint* p : alignPoints) {
                p->w = (int)abs(maxX - p->originalXPoint);
            }
            updatePoints = false;
        }
    }
};

class UIStackPanel :
    public Panel
{
protected:
    static UIStackPanel* FromContent(std::vector<Drawable*> content, bool vertical = true);
public:
    bool orientationVertical = true;
    bool manuallyRecalculateLayout = false;
    XY contentBoxSize = XY{ 0,0 };
    int spacing = 0;

    UIStackPanel();

    static UIStackPanel* Vertical(int spacing, std::vector<Drawable*> content, XY position = {0,0});
    static UIStackPanel* Horizontal(int spacing, std::vector<Drawable*> content, XY position = {0,0});

    void render(XY at) override;

    void addWidget(Drawable* d);
    void recalculateLayout();

    XY getDimensions() override {
        if (manuallyRecalculateLayout) {
            return contentBoxSize;
        }
        return Panel::getDimensions();
    }
};

