#pragma once
#include "globals.h"
#include "Layer.h"
#include "maineditor.h"

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
protected:
    Layer* l;
    Frame* f;
    int insertAt;
    bool deleteOnRedo = true;
public:
    UndoLayerCreated(Frame* f, Layer* l, int insertAt) : f(f), l(l), insertAt(insertAt) {}
    ~UndoLayerCreated() {
        if (deleteOnRedo == discardFromRedo) {
            delete l;
        }
    }

    Layer* getAffectedLayer() override { return l; }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;

    std::string name() override { return TL("vsp.undo.layercreated"); }
};

class UndoLayerRemoved : public UndoLayerCreated {
public:
    UndoLayerRemoved(Frame* f, Layer* l, int insertAt) : UndoLayerCreated(f, l,insertAt) {
        deleteOnRedo = false;
    }

    void undo(MainEditor* editor) override { UndoLayerCreated::redo(editor); };
    void redo(MainEditor* editor) override { UndoLayerCreated::undo(editor); };

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
protected:
    Layer* l;
    int variantIndex;
    LayerVariant storedVariant;

    bool deleteOnRedo = true;
public:
    UndoLayerVariantCreated(Layer* l, int variantIndex) : l(l), variantIndex(variantIndex) {}
    ~UndoLayerVariantCreated()
    {
        if (deleteOnRedo == discardFromRedo) {
            tracked_free(storedVariant.pixelData);
        }
    }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;
    std::string name() override { return TL("vsp.undo.layervariantcreated"); }
};

class UndoLayerVariantRemoved : public UndoLayerVariantCreated {

public:
    UndoLayerVariantRemoved(Layer* l, int variantIndex, LayerVariant storedVariant) 
        : UndoLayerVariantCreated(l, variantIndex)
    {
        this->storedVariant = storedVariant;
        deleteOnRedo = false;
    }

    void undo(MainEditor* editor) override { UndoLayerVariantCreated::redo(editor); };
    void redo(MainEditor* editor) override { UndoLayerVariantCreated::undo(editor); };
};

class UndoLayerOpacityChanged : public UndoStackElementV2 {
private:
    Layer* l;
    uint8_t oldOpacity, newOpacity;
public:
    UndoLayerOpacityChanged(Layer* l, uint8_t oldOpacity, uint8_t newOpacity)
        : l(l), oldOpacity(oldOpacity), newOpacity(newOpacity) { }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;
};

class UndoCommentAdded : public UndoStackElementV2 {
protected:
    CommentData comment;
public:
    UndoCommentAdded(CommentData c) : comment(c) {}

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;
};

class UndoCommentRemoved : public UndoCommentAdded {
public:
    UndoCommentRemoved(CommentData c) : UndoCommentAdded(c) {}

    void undo(MainEditor* editor) override { UndoCommentAdded::redo(editor); };
    void redo(MainEditor* editor) override { UndoCommentAdded::undo(editor); };
};

class UndoFrameCreated : public UndoStackElementV2 {
protected:
    Frame* f = NULL;
    int insertAt;
public:
    UndoFrameCreated(Frame* ff, int iinsertAt) : f(ff), insertAt(iinsertAt) {}
    ~UndoFrameCreated() {
        if (discardFromRedo) {
            delete f;
        }
    }

    void undo(MainEditor* editor) override;
    void redo(MainEditor* editor) override;
};