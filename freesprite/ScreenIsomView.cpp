#include "ScreenIsomView.h"
#include "maineditor.h"

void ScreenIsomView::renderQuadFromEditorContent(MainEditor* editor, XY origin00, double scale, XYZd ul, XYZd ur, XYZd dl, XYZd dr, SDL_Rect textureRegion, double shading)
{
    textureRegion = {
        (int)(textureRegion.x * pointScale),
        (int)(textureRegion.y * pointScale),
        (int)(textureRegion.w * pointScale),
        (int)(textureRegion.h * pointScale)
    };


    double alpha = rotAlpha * M_PI / 180;
    double beta = rotBeta * M_PI / 180;
    XYd ul2d = worldSpaceToScreenSpace(ul, alpha, beta) * scale;
    XYd ur2d = worldSpaceToScreenSpace(ur, alpha, beta) * scale;
    XYd dl2d = worldSpaceToScreenSpace(dl, alpha, beta) * scale;
    XYd dr2d = worldSpaceToScreenSpace(dr, alpha, beta) * scale;

    SDL_Vertex verts[4] =
    {
        {{origin00.x + ul2d.x, origin00.y + ul2d.y}},
        {{origin00.x + ur2d.x, origin00.y + ur2d.y}},
        {{origin00.x + dl2d.x, origin00.y + dl2d.y}},
        {{origin00.x + dr2d.x, origin00.y + dr2d.y}},
    };

    for (auto& v : verts) {
        v.color = SDL_FColor{ (float)(1.0f - shading),(float)(1.0f - shading),(float)(1.0f - shading),1.0f };
    }

    verts[0].tex_coord = { (float)textureRegion.x, (float)textureRegion.y };
    verts[1].tex_coord = { (float)(textureRegion.x + textureRegion.w), (float)textureRegion.y };
    verts[2].tex_coord = { (float)textureRegion.x, (float)(textureRegion.y + textureRegion.h) };
    verts[3].tex_coord = { (float)(textureRegion.x + textureRegion.w), (float)(textureRegion.y + textureRegion.h) };

    for (auto& v : verts) {
        v.tex_coord.x /= (float)editor->canvas.dimensions.x;
        v.tex_coord.y /= (float)editor->canvas.dimensions.y;
    }

    int indices[] = {0,1,2,2,1,3};
    for (Layer* l : editor->getLayerStack()) {
        for (SDL_Vertex& v : verts) {
            v.color.a = l->layerAlpha / 255.0f;
        }
        l->prerender();
        SDL_RenderGeometry(g_rd, l->renderData[g_rd].tex, verts, 4, indices, 6);
    }

    if (drawWireframe) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
        drawLine(xyAdd(origin00, xydToXy(ul2d)), xyAdd(origin00, xydToXy(ur2d)));
        drawLine(xyAdd(origin00, xydToXy(ur2d)), xyAdd(origin00, xydToXy(dr2d)));
        drawLine(xyAdd(origin00, xydToXy(dr2d)), xyAdd(origin00, xydToXy(dl2d)));
        drawLine(xyAdd(origin00, xydToXy(ul2d)), xyAdd(origin00, xydToXy(dl2d)));
    }
}

void ScreenIsomView::renderBoxFromEditorContent(MainEditor* editor, XY origin00, double scale, XYZd at, double sizeX, double sizeZ, double sizeY, XY textureBoxOrigin, double offset, bool flipUVX = false)
{
    XYZd l00 = xyzdAdd(at, { -offset,        -offset,  -offset }),
        lx0 = xyzdAdd(at,  { sizeX +offset,  -offset,  -offset }),
        lxz = xyzdAdd(at,  { sizeX + offset, -offset,  sizeZ + offset }),
        l0z = xyzdAdd(at,  { -offset,        -offset,  sizeZ + offset });

    XYZd h00 = xyzdAdd(l00, { 0, sizeY +offset*2, 0}),
        hx0 = xyzdAdd(lx0,  { 0, sizeY +offset*2, 0}),
        hxz = xyzdAdd(lxz,  { 0, sizeY +offset*2, 0}),
        h0z = xyzdAdd(l0z,  { 0, sizeY +offset*2, 0});

    SDL_Rect frontFace = { textureBoxOrigin.x, textureBoxOrigin.y + (int)sizeZ, (int)sizeX, (int)sizeY };
    SDL_Rect rightFace = { textureBoxOrigin.x + (int)sizeX, textureBoxOrigin.y + (int)sizeZ, (int)sizeZ, (int)sizeY };
    SDL_Rect leftFace = { textureBoxOrigin.x - (int)sizeZ, textureBoxOrigin.y + (int)sizeZ, (int)sizeZ, (int)sizeY };
    SDL_Rect backFace = { textureBoxOrigin.x + (int)sizeX + (int)sizeZ, textureBoxOrigin.y + (int)sizeZ, (int)sizeX, (int)sizeY };
    SDL_Rect bottomFace = { textureBoxOrigin.x + (int)sizeX, textureBoxOrigin.y, (int)sizeX, (int)sizeZ };
    SDL_Rect topFace = { textureBoxOrigin.x, textureBoxOrigin.y, (int)sizeX, (int)sizeZ };

    if (flipUVX) {
        frontFace = uvFlipHorizontal(frontFace);
        std::swap(leftFace, rightFace);
        leftFace = uvFlipHorizontal(leftFace);
        rightFace = uvFlipHorizontal(rightFace);
        bottomFace = uvFlipHorizontal(bottomFace);
        topFace = uvFlipHorizontal(topFace);
        backFace = uvFlipHorizontal(backFace);
    }

    //front face
    if (rotBeta <= 90 || rotBeta >= 270) {
            renderQuadFromEditorContent(editor, origin00, scale, h0z, hxz, l0z, lxz, frontFace,
            shade ? shadeFront : 0.0
        );
    }

    //right face
    if (rotBeta >= 0 && rotBeta <= 180) {
        renderQuadFromEditorContent(editor, origin00, scale, hxz, hx0, lxz, lx0, rightFace,
            shade ? shadeRight : 0.0
        );
    }

    //left face
    if (rotBeta >= 180) {
        renderQuadFromEditorContent(editor, origin00, scale, h00, h0z, l00, l0z, leftFace,
            shade ? shadeLeft : 0.0
        );
    }

    //back face
    if (rotBeta >= 90 && rotBeta <= 270) {
        renderQuadFromEditorContent(editor, origin00, scale, hx0, h00, lx0, l00, backFace,
            shade ? shadeBack : 0.0
        );
    }

    //bottom face
    if (rotAlpha < 0) {
        renderQuadFromEditorContent(editor, origin00, scale, l00, lx0, l0z, lxz, bottomFace,
            shade ? shadeBottom : 0.0
        );
    }

    //top face
    if (rotAlpha >= 0 && rotAlpha <= 180) {
        renderQuadFromEditorContent(editor, origin00, scale, h00, hx0, h0z, hxz, topFace,
            shade ? shadeTop : 0.0
        );
    }
}

void ScreenIsomView::renderQuad(SDL_Color color, XY origin00, double scale, XYZd ul, XYZd ur, XYZd dl, XYZd dr, double shading) {


    double alpha = rotAlpha * M_PI / 180;
    double beta = rotBeta * M_PI / 180;
    XYd ul2d = worldSpaceToScreenSpace(ul, alpha, beta) * scale;
    XYd ur2d = worldSpaceToScreenSpace(ur, alpha, beta) * scale;
    XYd dl2d = worldSpaceToScreenSpace(dl, alpha, beta) * scale;
    XYd dr2d = worldSpaceToScreenSpace(dr, alpha, beta) * scale;

    SDL_Vertex verts[4] =
    {
        {{origin00.x + ul2d.x, origin00.y + ul2d.y}},
        {{origin00.x + ur2d.x, origin00.y + ur2d.y}},
        {{origin00.x + dl2d.x, origin00.y + dl2d.y}},
        {{origin00.x + dr2d.x, origin00.y + dr2d.y}},
    };

    for (auto& v : verts) {
        v.color = SDL_FColor{color.r/255.0f, color.g/255.0f, color.b/255.0f, color.a/255.0f};
    }

    int indices[] = {0,1,2,2,1,3};
    SDL_RenderGeometry(g_rd, NULL, verts, 4, indices, 6);

    if (drawWireframe) {
        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 0x90);
        drawLine(xyAdd(origin00, xydToXy(ul2d)), xyAdd(origin00, xydToXy(ur2d)));
        drawLine(xyAdd(origin00, xydToXy(ur2d)), xyAdd(origin00, xydToXy(dr2d)));
        drawLine(xyAdd(origin00, xydToXy(dr2d)), xyAdd(origin00, xydToXy(dl2d)));
        drawLine(xyAdd(origin00, xydToXy(ul2d)), xyAdd(origin00, xydToXy(dl2d)));
    }
}

void ScreenIsomView::renderBox(SDL_Color color, XY origin00, double scale, XYZd at, double sizeX, double sizeZ, double sizeY, double offset) {
        XYZd l00 = xyzdAdd(at, { -offset,        -offset,  -offset }),
        lx0 = xyzdAdd(at,  { sizeX +offset,  -offset,  -offset }),
        lxz = xyzdAdd(at,  { sizeX + offset, -offset,  sizeZ + offset }),
        l0z = xyzdAdd(at,  { -offset,        -offset,  sizeZ + offset });

    XYZd h00 = xyzdAdd(l00, { 0, sizeY +offset*2, 0}),
        hx0 = xyzdAdd(lx0,  { 0, sizeY +offset*2, 0}),
        hxz = xyzdAdd(lxz,  { 0, sizeY +offset*2, 0}),
        h0z = xyzdAdd(l0z,  { 0, sizeY +offset*2, 0});

    //front face
    if (rotBeta <= 90 || rotBeta >= 270) {
        renderQuad(color, origin00, scale, h0z, hxz, l0z, lxz,
            shade ? shadeFront : 0.0
        );
    }

    //right face
    if (rotBeta >= 0 && rotBeta <= 180) {
        renderQuad(color, origin00, scale, hxz, hx0, lxz, lx0,
            shade ? shadeRight : 0.0
        );
    }

    //left face
    if (rotBeta >= 180) {
        renderQuad(color, origin00, scale, h00, h0z, l00, l0z,
            shade ? shadeLeft : 0.0
        );
    }

    //back face
    if (rotBeta >= 90 && rotBeta <= 270) {
        renderQuad(color, origin00, scale, hx0, h00, lx0, l00,
            shade ? shadeBack : 0.0
        );
    }

    //bottom face
    if (rotAlpha < 0) {
        renderQuad(color, origin00, scale, l00, lx0, l0z, lxz,
            shade ? shadeBottom : 0.0
        );
    }

    //top face
    if (rotAlpha >= 0 && rotAlpha <= 180) {
        renderQuad(color, origin00, scale, h00, hx0, h0z, hxz,
            shade ? shadeTop : 0.0
        );
    }
}

void ScreenIsomView::rotateFromMouseInput(double xrel, double yrel) {
    rotBeta -= xrel;
    rotAlpha += yrel;
    if (rotBeta < 0) {
        rotBeta += 360;
    }
    if (rotBeta > 360) {
        rotBeta -= 360;
    }
    rotAlpha = dclamp(-90, rotAlpha, 90);
}

void ScreenIsomView::renderFloorGrid()
{
    double lineLength = 60;
    for (int i = -8; i <= 8; i++) {
        double gridSpaced = i * 4;
        XY proj = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ (double)gridSpaced,0,-lineLength }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));
        XY proj2 = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ (double)gridSpaced,0,lineLength }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));

        XY projA = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ -lineLength, 0, (double)gridSpaced }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));
        XY projA2 = xyAdd(screen00, xydToXy(worldSpaceToScreenSpace({ lineLength, 0, (double)gridSpaced }, rotAlpha * M_PI / 180, rotBeta * M_PI / 180) * size));

        SDL_SetRenderDrawColor(g_rd, 255, 255, 255, 30 / ixmax(1, abs(i)));
        drawLine(proj, proj2);
        drawLine(projA, projA2);
    }
}

XYd ScreenIsomView::worldSpaceToScreenSpace(XYZd point, double alpha, double beta)
{
    //so that y+ is up
    point.y *= -1;

    matrix a = {
        {1, 0, 0},
        {0, cos(alpha), sin(alpha)},
        {0, -sin(alpha), cos(alpha)}
    };

    matrix b = {
        {cos(beta), 0, -sin(beta)},
        {0, 1, 0},
        {sin(beta), 0, cos(beta)}
    };

    matrix c = { {point.x}, {point.y}, {point.z} };

    matrix xyId = {
        {1,0,0},
        {0,1,0},
        {0,0,0}
    };

    matrix r = matrixMultiply(xyId, matrixMultiply(matrixMultiply(a, b), c));

    return XYd{ r[0][0], r[1][0] };
}

XYZd ScreenIsomView::screenSpaceToWorldSpace(XYd point, double alpha, double beta) {
    matrix a = {
        {1, 0, 0},
        {0, cos(alpha), -sin(alpha)},
        {0, sin(alpha), cos(alpha)}
    };

    matrix b = {
        {cos(beta), 0, sin(beta)},
        {0, 1, 0},
        {-sin(beta), 0, cos(beta)}
    };

    matrix c = {
        {point.x},
        {point.y},
        {0}
    };

    matrix xyId = {
        {1,0,0},
        {0,1,0},
        {0,0,0}
    };

    matrix r = matrixMultiply(xyId, matrixMultiply(matrixMultiply(b, a), c));

    return XYZd{ r[0][0], -r[1][0], r[2][0] };
}

XY ScreenIsomView::scaledPoint(XY point)
{
    return { (int)(point.x * pointScale), (int)(point.y * pointScale) };
}

SDL_Rect ScreenIsomView::uvFlipHorizontal(SDL_Rect x)
{
    return { x.x + x.w, x.y, -x.w, x.h };
}