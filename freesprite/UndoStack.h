#pragma once
#include "globals.h"
#include "Layer.h"

class UndoStackElementV2 {
public:
    bool discardFromRedo = false;

    UndoStackElementV2() {}
    virtual ~UndoStackElementV2() = default;

    virtual Layer* getAffectedLayer() { return NULL; }

    virtual void undo(MainEditor* editor) = 0;
    virtual void redo(MainEditor* editor) = 0;

    virtual std::string name() { return "Undo operation"; }
};

class UndoStackComposite : public UndoStackElementV2 {
private:
    std::vector<UndoStackElementV2*> elements;
public:
    std::string opName = "Multiple undo operations";

    UndoStackComposite(std::vector<UndoStackElementV2*> e) : elements(e) {}
    ~UndoStackComposite() {
        for (auto& e : elements) {
            delete e;
        }
    }

    void undo(MainEditor* editor) override {
        for (auto& e : elements) {
            e->undo(editor);
        }
    }
    void redo(MainEditor* editor) override {
        for (auto& e : elements) {
            e->redo(editor);
        }
    }

    std::string name() override { return opName; }
};

class UndoLayerModified : public UndoStackElementV2 {
private:
    Layer* l;
    int targetVariant;
    u8* storedData;
public:
    static UndoLayerModified* fromCurrentState(Layer* l) {
        int variantIndex = l->currentLayerVariant;
        u64 size = (u64)l->w * (u64)l->h * 4ull;
        u8* dataCopy = (u8*)tracked_malloc(size, "Layers");
        if (dataCopy != NULL) {
            memcpy(dataCopy, l->layerData[variantIndex].pixelData, size);
        }
        else {
            logerr("[UndoLayerModified] MALLOC FAILED");
        }
        return new UndoLayerModified(l, variantIndex, dataCopy);
    }

    UndoLayerModified(Layer* l, int targetVariant, u8* storedData) 
        : l(l), targetVariant(targetVariant), storedData(storedData) {
    }
    ~UndoLayerModified() {
        if (storedData != NULL) {
            tracked_free(storedData);
        }
    }
    Layer* getAffectedLayer() override { return l; }
    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;

    std::string name() override { return TL("vsp.undo.layermodified"); }
};

class UndoLayerCreated : public UndoStackElementV2 {
private:
    Layer* l;
    int insertAt;
public:
    UndoLayerCreated(Layer* l, int insertAt) : l(l), insertAt(insertAt) {}
    ~UndoLayerCreated() {
        if (discardFromRedo) {
            delete l;
        }
    }

    Layer* getAffectedLayer() override { return l; }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;

    std::string name() override { return TL("vsp.undo.layercreated"); }
};

class UndoLayerRemoved : public UndoStackElementV2 {
private:
    Layer* l;
    int insertAt;
public:
    UndoLayerRemoved(Layer* l, int insertAt) : l(l), insertAt(insertAt) {}
    ~UndoLayerRemoved() {
        if (!discardFromRedo) {
            delete l;
        }
    }

    Layer* getAffectedLayer() override { return l; }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;

    std::string name() override { return TL("vsp.undo.layerremoved"); }
};

class UndoLayersResized : public UndoStackElementV2 {
private:
    XY oldDimensions, newDimensions;
    XY oldTileSize, newTileSize;
public:
    std::map<Layer*, std::vector<LayerVariant>> storedLayerData;

    UndoLayersResized(XY oldDim, XY newDim, XY oldTiles, XY newTiles) 
        : oldDimensions(oldDim), newDimensions(newDim), oldTileSize(oldTiles), newTileSize(newTiles) {}
    ~UndoLayersResized() {
        for (auto& [layer, data] : storedLayerData) {
            for (auto& v : data) {
                delete v.pixelData;
            }
        }
    }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;

    std::string name() override { return TL("vsp.undo.layersresized"); }
};

class UndoLayerReordered : public UndoStackElementV2 {
private:
    Layer* l;
    int oldIndex, newIndex;
public:
    UndoLayerReordered(Layer* l, int oldIndex, int newIndex) : l(l), oldIndex(oldIndex), newIndex(newIndex) {}

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;

    std::string name() override { return TL("vsp.undo.layerreordered"); }
};

class UndoLayerVariantCreated : public UndoStackElementV2 {
private:
    Layer* l;
    int variantIndex;
    LayerVariant storedVariant;
public:
    UndoLayerVariantCreated(Layer* l, int variantIndex) : l(l), variantIndex(variantIndex) {}
    ~UndoLayerVariantCreated()
    {
        if (discardFromRedo) {
            tracked_free(storedVariant.pixelData);
        }
    }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;
    std::string name() override { return TL("vsp.undo.layervariantcreated"); }
};