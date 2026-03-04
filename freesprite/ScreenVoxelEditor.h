#pragma once

#include "ScreenIsomView.h"

struct Voxel {
    XYZd pos;
    SDL_Color color;
};

class ScreenVoxelEditor : public ScreenIsomView {
protected:
    std::vector<Voxel> voxels;
    int dragging = 0;
    XYZd mouseVoxelTargetPoint = {0,0,0};
    u32 currentColor = 0xFFFFFFFF;

    bool mousePlacing = false;
public:
    void render() override;
    void takeInput(SDL_Event evt) override;
    std::string getName() override { return "Voxel editor"; }

    void recalcMouseVoxelTargetPoint(XY mousePos);
    void placeVoxel(XYZd pos);

    void renderModel();
};