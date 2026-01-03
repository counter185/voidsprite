
#include "io_base.h"
#include "io_aseprite.h"
#include "io_avi.h"


bool writeAVI(PlatformNativePathString path, MainEditor* session)
{
    __AVIMAINHEADER aviHeader{};
    aviHeader.dwMicroSecPerFrame = session->frameAnimMSPerFrame * 1000;
    aviHeader.dwWidth = session->canvas.dimensions.x;
    aviHeader.dwHeight = session->canvas.dimensions.y;
    aviHeader.dwTotalFrames = session->frames.size();
    aviHeader.dwMaxBytesPerSec = aviHeader.dwWidth * aviHeader.dwHeight * 4 * (1000000 / aviHeader.dwMicroSecPerFrame);
    aviHeader.dwStreams = 1;
    aviHeader.dwInitialFrames = 0;
    aviHeader.dwSuggestedBufferSize = aviHeader.dwWidth * aviHeader.dwHeight * 4;
    aviHeader.dwFlags = 0;// 0x10;

    __BITMAPINFOHEADER strfData{};
    strfData.biSize = sizeof(__BITMAPINFOHEADER);
    strfData.biWidth = session->canvas.dimensions.x;
    strfData.biHeight = session->canvas.dimensions.y;
    strfData.biPlanes = 1;
    strfData.biBitCount = 32;
    strfData.biCompression = 0;
    strfData.biSizeImage = aviHeader.dwWidth * aviHeader.dwHeight * 4;
    strfData.biXPelsPerMeter = strfData.biYPelsPerMeter = 0;
    strfData.biClrUsed = 0;
    strfData.biClrImportant = 0;

    __AVIStreamHeader strhData{};
    strhData.fccType = 'sdiv';
    strhData.fccHandler = 'ABGR';
    strhData.dwFlags = 0;
    strhData.wPriority = 0;
    strhData.wLanguage = 0;
    strhData.dwInitialFrames = 0;
    strhData.dwScale = session->frameAnimMSPerFrame;
    strhData.dwRate = session->frameAnimMSPerFrame * session->frames.size();
    strhData.dwStart = 0;
    strhData.dwLength = session->frames.size();
    strhData.dwSuggestedBufferSize = aviHeader.dwSuggestedBufferSize;
    strhData.dwQuality = -1;
    strhData.dwSampleSize = 0;
    strhData.rcFrame[0] = strhData.rcFrame[1] = 0;
    strhData.rcFrame[2] = session->canvas.dimensions.x;
    strhData.rcFrame[3] = session->canvas.dimensions.y;

    std::string name = "voidsprite animation";

    std::vector<RIFFChunk*> frameChunks;
    for (auto*& frame : session->frames) {
        frameChunks.push_back(new RIFFLargeDataChunk("00db", [session, frame]() {
            Layer* frameFlat = session->flattenFrame(frame);
            std::vector<u8> ret;

            u32* srcpx = frameFlat->pixels32();
            for (int y = frameFlat->h - 1; y >= 0; y--) {
                for (int x = 0; x < frameFlat->w; x++) {
                    SDL_Color srcColor = uint32ToSDLColor(frameFlat->getPixelAt({ x, y }));
                    ret.push_back(srcColor.b);
                    ret.push_back(srcColor.g);
                    ret.push_back(srcColor.r);
                    ret.push_back(srcColor.a);
                }
            }
            delete frameFlat;

            return ret;
        }));
    }

    RIFFListChunk* root =
        new RIFFListChunk("RIFF", "AVI ", {
            new RIFFListChunk("LIST", "hdrl", {
                new RIFFChunk("avih", (u8*)&aviHeader, sizeof(__AVIMAINHEADER)),
                new RIFFListChunk("LIST", "strl", {
                    new RIFFChunk("strh", (u8*)&strhData, sizeof(__AVIStreamHeader)),
                    new RIFFChunk("strf", (u8*)&strfData, sizeof(__BITMAPINFOHEADER)),
                    new RIFFChunk("strn", (u8*)name.c_str(), name.size()+1),
                })
            }),
            new RIFFJUNKChunk(512),
            new RIFFListChunk("LIST", "movi", frameChunks)
        });

    FILE* f = platformOpenFile(path, PlatformFileModeWB);
    if (f != NULL) {
        root->write(f);
        fclose(f);
        delete root;
        return true;
    }
    delete root;

    return false;
}
