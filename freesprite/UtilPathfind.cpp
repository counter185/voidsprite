#include <cstdlib>
#include <cstdio>
#include <string>
#include <fstream>
#include <vector>
#include <new>

#include "UtilPathfind.h"
#include "Layer.h"
#include "Notification.h"

template <typename T>
void vector_remove(std::vector<T>& vec, int index) {
    vec.erase(vec.begin() + index);
}

template <typename T>
void vector_removeElement(std::vector<T*>& vec, T* element) {
    for (int x = 0; x != vec.size(); x++) {
        if (vec[x] == element) {
            vector_remove(vec, x);
            return;
        }
    }
}

float XYDistanceSq(XY p1, XY p2) {
    return (float)(p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y);
}

#define PRINT_COLORS true

bool __util_anyEquals(Node* val, std::vector<Node*> list) {
    for (Node*& a : list) {
        if (val->positionEquals(*a)) {
            return true;
        }
    }
    return false;
}
bool __util_anyEquals_A(Node val, std::vector<Node> list) {
    for (Node& a : list) {
        if (val.positionEquals(a)) {
            return true;
        }
    }
    return false;
}
bool __util_anyEqualsAndGGreater(Node* val, std::vector<Node*> list) {
    for (Node*& a : list) {
        if (val->positionEquals(*a) && val->g > a->g) {
            return true;
        }
    }
    return false;
}

/*char** ReadMapFromFile(std::string filepath, int w, int h) {
    char** outmap = (char**)tracked_malloc(sizeof(char*) * h);
    for (int x = 0; x < h; x++) {
        outmap[x] = (char*)tracked_malloc(sizeof(char) * w);
    }
    std::ifstream infile(filepath);
    if (!infile.good()) {
        logprintf("FILE OPEN FAIL\n");
    }
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int a;
            infile >> a;
            outmap[y][x] = (char)a;
        }
    }
    infile.close();
    return outmap;
}*/

/*void PrintMap(char** outmap, int w, int h, std::vector<Node> drawNodes = std::vector<Node>()) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int val = outmap[y][x];
#if PRINT_COLORS
            if (val == 5) {
                logprintf("\e[0;31m");
            }
            else if (__util_anyEquals_A(Node(x, y), drawNodes)) {
                logprintf("\e[0;33m");
                val = 1;
            }
            else {
                logprintf("\e[0;37m");
            }
#endif
            logprintf("%i,", val);
        }
        logprintf("\n");
    }
}*/

bool CanWalkOnMapPoint(Layer* map, int x, int y, uint32_t col1, uint32_t col2) {

    if (x < 0 || y < 0 || x >= map->w || y >= map->h) {
        return false;
    }
    uint32_t mapPixel = map->getPixelAt(XY{ x,y });
    return mapPixel == col1 || mapPixel == col2 || (!map->isPalettized && mapPixel >> 24 == 0 && (col1 >> 24 == 0 || col2 >> 24 == 0));
}

uint64_t startTime;

bool shouldAntiDeadlock() {
    return SDL_GetTicks64() - startTime > 5000;
}

std::vector<Node> genAStar(Layer* mainMap, XY start, XY end) {
    Node* startNode = new Node(start.x, start.y);     //START POSITION
    Node* endNode = new Node(end.x, end.y);     //END POSITION
    startTime = SDL_GetTicks64();
    if (start.x < 0 || start.y < 0 || start.x >= mainMap->w || start.y >= mainMap->h
        || end.x < 0 || end.y < 0 || end.x >= mainMap->w || end.y >= mainMap->h) {
        return std::vector<Node>();
    }
    uint32_t c1 = mainMap->getPixelAt(start);
    uint32_t c2 = mainMap->getPixelAt(end);

    //int MAPW = w;
    //int MAPH = h;

    std::vector<Node*> openList;
    std::vector<Node*> closedList;

    openList.push_back(startNode);

    int step = 0;
    while (openList.size() > 0) {
        step++;
        //PrintMap(mainMap, MAPW, MAPH);

        Node* currentNode = openList[0];
        for (Node*& a : openList) {
            if (currentNode->f > a->f) {
                currentNode = a;
            }
        }

        vector_removeElement(openList, currentNode);
        closedList.push_back(currentNode);

        if (currentNode->positionEquals(*endNode)) {
            std::vector<Node> nodePath;
            Node* pathN = currentNode;
            while (pathN != NULL) {
                nodePath.push_back(*pathN);
                Node* npathN = pathN->parent;
                delete pathN;
                pathN = npathN;
            }
            g_addNotification(SuccessShortNotification(std::format("A* finished in {} steps", step), ""));
            logprintf("genAStar finished in %i steps\n", step);
            return nodePath;
        }
        else {
            XY nxy[4] = {
                XY(-1,0), XY(1,0),
                XY(0,1), XY(0,-1)
            };

            std::vector<Node*> nextNodes;
            for (int x = 0; x < 4; x++) {
                int pointX = currentNode->x + nxy[x].x;
                int pointY = currentNode->y + nxy[x].y;
                if (CanWalkOnMapPoint(mainMap, pointX, pointY, c1, c2)) {
                    nextNodes.push_back(new Node(pointX, pointY, currentNode));
                }
            }

            for (Node*& child : nextNodes) {
                if (__util_anyEquals(child, closedList)) {
                    delete child;
                    continue;
                }

                if (shouldAntiDeadlock()) {
                    for (Node*& n : closedList) {
                        delete n;
                    }
                    for (Node*& n : openList) {
                        delete n;
                    }
                    g_addNotification(ErrorNotification("A* took too long", ""));
                    return std::vector<Node>();
                }

                int distanceBetweenChildAndCurrent = 1;
                child->g = currentNode->g + distanceBetweenChildAndCurrent;
                child->h = XYDistanceSq(XY(child->x, child->y), XY(endNode->x, endNode->y));

                child->f = child->g + child->h;

                if (__util_anyEqualsAndGGreater(child, openList)) {
                    delete child;
                    continue;
                }

                openList.push_back(child);
            }

        }
    }
    g_addNotification(ErrorNotification("A* could not find a path", ""));
    //logprintf("no path\n");
    return std::vector<Node>();
}