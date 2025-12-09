#pragma once
#include "globals.h"

class UndoStackElementV2 {
public:
    bool discardFromRedo = false;

    UndoStackElementV2() {}
    virtual ~UndoStackElementV2() = default;

    virtual Layer* getAffectedLayer() { return NULL; }

    virtual void undo(MainEditor* editor) = 0;
    virtual void redo(MainEditor* editor) = 0;
};

class UndoStackComposite : public UndoStackElementV2 {
private:
    std::vector<UndoStackElementV2*> elements;
public:
    UndoStackComposite(std::vector<UndoStackElementV2*> e) : elements(e) {}
    ~UndoStackComposite() {
        for (auto& e : elements) {
            delete e;
        }
    }

    void undo(MainEditor* editor) override {
        for (int i = (int)elements.size() - 1; i >= 0; i--) {
            elements[i]->undo(editor);
        }
    }
    void redo(MainEditor* editor) override {
        for (auto& e : elements) {
            e->redo(editor);
        }
	}
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
};


