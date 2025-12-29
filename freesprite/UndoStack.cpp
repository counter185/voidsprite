#include "UndoStack.h"
#include "maineditor.h"
#include "MainEditorPalettized.h"
#include "EditorLayerPicker.h"
#include "EditorFramePicker.h"

void UndoLayerCreated::undo(MainEditor* editor)
{
    //remove layer from list
    auto& layers = f->layers;
    auto pos = std::find(layers.begin(), layers.end(), l);
    if (pos != layers.end()) {
        layers.erase(pos);
    }
    if (editor->selLayer >= layers.size()) {
        editor->switchActiveLayer(layers.size() - 1);
    }
    editor->layerPicker->updateLayers();
}

void UndoLayerCreated::redo(MainEditor* editor)
{
    auto& layers = f->layers;
    layers.insert(layers.begin() + insertAt, l);
    editor->layerPicker->updateLayers();
    if (editor->isPalettized) {
        ((MainEditorPalettized*)editor)->updatePalette();
    }
}


void UndoLayersResized::undo(MainEditor* editor)
{
    for (auto& [layer, data] : storedLayerData) {
        auto oldData = layer->layerData;
        layer->layerData = data;
        layer->w = oldDimensions.x;
        layer->h = oldDimensions.y;
        layer->markLayerDirty();
        storedLayerData[layer] = oldData;
    }
    editor->canvas.dimensions = oldDimensions;
    editor->tileDimensions = oldTileSize;
}

void UndoLayersResized::redo(MainEditor* editor)
{
    for (auto& [layer, data] : storedLayerData) {
        auto oldData = layer->layerData;
        layer->layerData = data;
        layer->w = newDimensions.x;
        layer->h = newDimensions.y;
        layer->markLayerDirty();
        storedLayerData[layer] = oldData;
    }
    editor->canvas.dimensions = newDimensions;
    editor->tileDimensions = newTileSize;
}


void UndoLayerModified::undo(MainEditor* editor)
{
    if (storedData != NULL) {
        auto& variant = l->layerData[targetVariant];
        u8* oldData = variant.pixelData;
        variant.pixelData = storedData;
        storedData = oldData;
        l->markLayerDirty();
    }
}

void UndoLayerModified::redo(MainEditor* editor)
{
    undo(editor);
}


void UndoLayerReordered::undo(MainEditor* editor)
{
    auto& layers = f->layers;
    layers.erase(layers.begin() + newIndex);
    layers.insert(layers.begin() + oldIndex, l);
    editor->layerPicker->updateLayers();
}

void UndoLayerReordered::redo(MainEditor* editor)
{
    auto& layers = f->layers;
    layers.erase(layers.begin() + oldIndex);
    layers.insert(layers.begin() + newIndex, l);
    editor->layerPicker->updateLayers();
}


void UndoLayerVariantCreated::undo(MainEditor* editor)
{
    storedVariant = l->layerData[variantIndex];
    l->layerData.erase(l->layerData.begin() + variantIndex);
    if (variantIndex == l->currentLayerVariant) {
        editor->layer_switchVariant(l, ixmin(variantIndex, l->layerData.size() - 1));
    }
    editor->layerPicker->updateLayers();
}

void UndoLayerVariantCreated::redo(MainEditor* editor)
{
    l->layerData.insert(l->layerData.begin() + variantIndex, storedVariant);
    if (variantIndex == l->currentLayerVariant) {
        editor->layer_switchVariant(l, ixmin(variantIndex, l->layerData.size() - 1));
    }
    editor->layerPicker->updateLayers();
}

void UndoLayerOpacityChanged::undo(MainEditor* editor)
{
    l->layerAlpha = l->lastConfirmedlayerAlpha = oldOpacity;
    editor->layerPicker->updateLayers();
}

void UndoLayerOpacityChanged::redo(MainEditor* editor)
{
    l->layerAlpha = l->lastConfirmedlayerAlpha = newOpacity;
    editor->layerPicker->updateLayers();
}

void UndoCommentAdded::undo(MainEditor* editor)
{
    auto findAtPos = std::find_if(editor->comments.begin(), editor->comments.end(),
        [this](CommentData& c) {
            return xyEqual(c.position, comment.position);
        });
    if (findAtPos != editor->comments.end()) {
        editor->comments.erase(findAtPos);
    }
}

void UndoCommentAdded::redo(MainEditor* editor)
{
    editor->comments.push_back(comment);
}


void UndoFrameCreated::undo(MainEditor* editor)
{
    editor->frames.erase(editor->frames.begin() + insertAt);
    if (editor->activeFrame >= editor->frames.size()) {
        editor->activeFrame = editor->frames.size() - 1;
    }
    editor->framePicker->createFrameButtons();
    editor->layerPicker->updateLayers();
}

void UndoFrameCreated::redo(MainEditor* editor)
{
    editor->frames.insert(editor->frames.begin() + insertAt, f);
    editor->framePicker->createFrameButtons();
    editor->layerPicker->updateLayers();
}
