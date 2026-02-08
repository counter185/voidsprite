#pragma once

#include "globals.h"

class Node {
public:
    int x, y;
    float g = 0, h = 0, f = 0;
    Node* parent;

    Node(int x, int y, Node* parent = NULL) {
        this->x = x;
        this->y = y;
        this->parent = parent;
    }

    bool positionEquals(Node other) {
        return x == other.x && y == other.y;
    }

    Node copy() {
        return *this;
    }
};

std::vector<Node> genAStar(Layer* mainMap, XY start, XY end, OperationProgressReport* progressReport);