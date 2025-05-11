#pragma once



class LayerListCompound;

class LayerListEntry {
protected:
    LayerListCompound* parent = NULL;

    int positionInParent();
public:
    virtual ~LayerListEntry() = default;

    virtual bool isCompound() { return false; }
    virtual std::string name() = 0;
    virtual std::vector<Layer*> iterLayers() = 0;
};

class LayerListLayer : public LayerListEntry {
public:
    Layer* layer = NULL;

    LayerListLayer(Layer* l) : layer(l) {}

    std::string name() override { return layer->name; }
    std::vector<Layer*> iterLayers() override {
		return { layer };
    }
};

class LayerListCompound : public LayerListEntry {
public:
    std::string groupName = "";
    std::vector<LayerListEntry*> entries;

    LayerListCompound(std::string name) : groupName(name) {}

    ~LayerListCompound() {
        for (LayerListEntry* entry : entries) {
            delete entry;
        }
    }

    bool isCompound() override { return true; }
    std::string name() override { return groupName; }
    std::vector<Layer*> iterLayers() override {
        std::vector<Layer*> ret = {};
		for (LayerListEntry* entry : entries) {
			std::vector<Layer*> layers = entry->iterLayers();
			ret.insert(ret.end(), layers.begin(), layers.end());
		}
        return ret;
	}
    bool moveLayerUp(LayerListEntry* l) {
        auto pos = std::find(entries.begin(), entries.end(), l);
        if (pos == entries.end()) {
            for (LayerListEntry* entry : entries) {
                if (entry->isCompound()) {
                    LayerListCompound* compound = (LayerListCompound*)entry;
                    if (compound->moveLayerUp(l)) {
                        return true;
                    }
                }
            }
        } else {
            int index = pos - entries.begin();
            if (index == 0) {
                if (parent != NULL) {
                    parent->entries.insert(parent->entries.begin() + positionInParent(), l);
                    entries.erase(pos);
                    return true;
                }
            } else {
                LayerListEntry* aboveEntry = entries[index - 1];
                entries.erase(pos);
                if (aboveEntry->isCompound()) {
                    ((LayerListCompound*)aboveEntry)->entries.push_back(l);
                } else {
                    entries.insert(entries.begin() + index - 1, l);
                }
                return true;
            }
        }

        return false;
    }

    bool moveLayerDown(LayerListEntry* l) {
        auto pos = std::find(entries.begin(), entries.end(), l);
        if (pos == entries.end()) {
            for (LayerListEntry* entry : entries) {
                if (entry->isCompound()) {
                    LayerListCompound* compound = static_cast<LayerListCompound*>(entry);
                    if (compound->moveLayerDown(l)) {
                        return true;
                    }
                }
            }
        } else {
            int index = pos - entries.begin();
            if (index == entries.size() - 1) {
                if (parent != NULL) {
                    parent->entries.insert(parent->entries.begin() + positionInParent() + 1, l);
                    entries.erase(pos);
                    return true;
                }
            } else {
                LayerListEntry* belowEntry = entries[index + 1];
                entries.erase(pos);
                if (belowEntry->isCompound()) {
                    ((LayerListCompound*)belowEntry)->entries.insert(((LayerListCompound*)belowEntry)->entries.begin(), l);
                } else {
                    entries.insert(entries.begin() + index + 1, l);
                }
                return true;
            }
        }

        return false;
    }

    bool removeLayer(LayerListLayer* l) {
        auto pos = std::find(entries.begin(), entries.end(), l);
        if (pos == entries.end()) {
            for (LayerListEntry* entry : entries) {
                if (entry->isCompound()) {
                    if (((LayerListCompound*)entry)->removeLayer(l)) {
                        return true;
                    }
                }
            }
        } else {
            entries.erase(pos);
            return true;
        }

        return false;
    }

    LayerListLayer* getLayerEntry(Layer* l) {
        for (LayerListEntry* entry : entries) {
            if (entry->isCompound()) {
                LayerListLayer* ret = ((LayerListCompound*)entry)->getLayerEntry(l);
                if (ret != NULL) {
                    return ret;
                }
            } else {
                LayerListLayer* layerEntry = static_cast<LayerListLayer*>(entry);
                if (layerEntry->layer == l) {
                    return layerEntry;
                }
            }
        }
        return NULL;
    }
};

inline int LayerListEntry::positionInParent() {
    if (parent == NULL) {
        return -1;
    }
    return std::find(parent->entries.begin(), parent->entries.end(), this) - parent->entries.begin();
}
