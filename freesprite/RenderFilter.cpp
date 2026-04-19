#include <queue>
#include <random>

#include "RenderFilter.h"



Layer* RGBGeneFilter::run(Layer* src, ParameterStore* options) {
    Layer* c = copy(src);

    auto rng = std::default_random_engine {};

    int sizeR = options->getInt("size.r");// std::stoi(options["size.r"]);
    int sizeG = options->getInt("size.g");// std::stoi(options["size.g"]);
    int sizeB = options->getInt("size.b");// std::stoi(options["size.b"]);
    bool mutateUpwards = options->getBool("mutate upwards");// std::stoi(options["mutate upwards"]);
    int bias = options->getInt("bias");// std::stoi(options["bias"]);

    struct Plant {
        XY position;
        u8 r;
        u8 g;
        u8 b;
    };
    std::queue<Plant> plants;
    //std::map<u64, bool> closedList;

    std::function<int(int)> randomMutation = [bias](int size) -> int { return rand() % ((size)*2 + 1 + bias) - size; };
    std::function<u8(s16,int)> mutateChannel = [randomMutation, mutateUpwards](s16 channelValue, int size) {
        return (u8)ixmax(0, ixmin(255, channelValue + randomMutation(size) * (mutateUpwards ? 1 : -1)));
    };

    for (int x = 0; x < src->w; x++) {
        for (int y = 0; y < src->h; y++) {
            SDL_Color px = uint32ToSDLColor(c->getPixelAt({x,y}));
            if (px.a >= 0x80) {
                plants.push(Plant{{x,y}, px.r,px.g,px.b});
                //closedList[encodeXY({x,y})] = true;
            }
        }
    }
    if (plants.empty()) {
        plants.push(Plant{{c->w/2, c->h/2}, 0, 0, 0});
    }

    XY directions[] = {
        {-1,0}, {1,0}, {0,-1}, {0,1}
    };

    while (!plants.empty()) {
        Plant p = plants.front();
        plants.pop();
        //loginfo(frmt("processing next: {}:{}", p.position.x, p.position.y));

        c->setPixel(p.position, PackRGBAtoARGB(p.r, p.g, p.b, 255));

        std::vector<u8> nextDirections = std::vector<u8>{0,1,2,3};
        std::shuffle(nextDirections.begin(), nextDirections.end(), rng);
        int directionsValid = 0;
        for (int dir = 0; dir < 4; dir++) {
            int nextDirectionIdx = 0;//rand() % nextDirections.size();
            XY nextPos = xyAdd(p.position, directions[nextDirections[nextDirectionIdx]]);
            nextDirections.erase(nextDirections.begin()+nextDirectionIdx);

            bool directionValid = 
                /*!closedList.contains(encodeXY(nextPos))
                &&*/ nextPos.x >= 0 && nextPos.y >= 0 
                && nextPos.x < c->w && nextPos.y < c->h
                && uint32ToSDLColor(c->getPixelAt(nextPos)).a < 0x80;

            if (directionValid) {
                if (++directionsValid > 1) {
                    break;
                }
            }
        }

        if (directionsValid > 0) {
            //closedList[encodeXY(nextPos)] = true;
            XY nextPos = xyAdd(p.position, directions[rand() % 4]);
            bool directionValid = 
                /*!closedList.contains(encodeXY(nextPos))
                &&*/ nextPos.x >= 0 && nextPos.y >= 0 
                && nextPos.x < c->w && nextPos.y < c->h
                && uint32ToSDLColor(c->getPixelAt(nextPos)).a < 0x80;
            if (directionValid) {
                plants.push({
                    nextPos, 
                    mutateChannel(p.r, sizeR),
                    mutateChannel(p.g, sizeG),
                    mutateChannel(p.b, sizeB)
                });
                directionsValid--;
            }
        }

        if (directionsValid > 0) {
            plants.push(p);
        }
    }

    return c;
}

Layer* RenderGridFilter::run(Layer* src, ParameterStore* options)
{
    u32 color = options->getColorRGB("color") | 0xFF000000;// std::stoul(options["color"], NULL, 16) | 0xFF000000;
    int alpha = options->getInt("alpha"); //std::stoi(options["alpha"]);
    color = modAlpha(color, alpha);
    bool blend = options->getBool("blend"); //std::stoi(options["blend"]);
    int gridX = options->getInt("size.x");// std::stoi(options["size.x"]);
    int gridY = options->getInt("size.y");// std::stoi(options["size.y"]);

    Layer* c = copy(src);
    if (gridX > 0) {
        for (int xx = 0; xx < c->w; xx += gridX) {
            for (int yy = 0; yy < c->h; yy++) {
                c->setPixel({ xx,yy },
                    blend ? alphaBlend(c->getPixelAt({ xx,yy }), color) : color);
            }
        }
    }
    if (gridY > 0) {
        for (int yy = 0; yy < c->h; yy += gridY) {
            for (int xx = 0; xx < c->w; xx++) {
                c->setPixel({ xx,yy },
                    blend ? alphaBlend(c->getPixelAt({ xx,yy }), color) : color);
            }
        }
    }
    return c;
}
