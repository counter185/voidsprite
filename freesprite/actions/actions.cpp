#include "actions.h"
#include "../Notification.h"

void g_registerDefaultActions()
{
    g_defaultEditorActions.clear();

    g_defaultEditorActions.push_back(NamedEditorOperation{ "Generate Unity spritesheet", action_editorExportUnitySpritesheet });
}

void action_editorExportUnitySpritesheet(MainEditor* editor)
{
    if (editor->lastConfirmedSave && !editor->lastConfirmedSavePath.empty()) {

        auto metaPath = editor->lastConfirmedSavePath + convertStringOnWin32(".meta");

        std::ofstream f = platformOpenOFStream(metaPath);
        if (f.good()) {

            f << "fileFormatVersion: 2\n";
            f << "guid: " << randomUnityGUID() << "\n";

            f << "TextureImporter:\n";
            f << "  serializedVersion: 0\n";
            f << "  mipmaps:\n";
            f << "    mipMapMode: 0\n";
            f << "    enableMipMap: 0\n";
            f << "    sRGBTexture: 1\n";
            f << "    linearTexture: 0\n";
            f << "  isReadable: 0\n";
            f << "  streamingMipmaps: 0\n";
            f << "  streamingMipmapsPriority: 0\n";
            f << "  vTOnly: 0\n";
            f << "  ignoreMipmapLimit: 0\n";
            f << "  generateCubemap: 6\n";
            f << "  textureFormat: 1\n";
            f << "  maxTextureSize: 2048\n";
            f << "  textureSettings:\n";
            f << "    serializedVersion: 0\n";
            f << "    filterMode: 0\n";
            f << "    aniso: 1\n";
            f << "    mipBias: 0\n";
            f << "    wrapU: 1\n";
            f << "    wrapV: 1\n";
            f << "    wrapW: 1\n";
            f << "  spriteMode: 2\n";
            f << "  spriteExtrude: 1\n";
            f << "  spriteMeshType: 1\n";
            f << "  alignment: 0\n";
            f << "  spritePivot: {x: 0.5, y: 0.5}\n";
            f << "  spritePixelsToUnits: 100\n";
            f << "  spriteBorder: {x: 0, y: 0, z: 0, w: 0}\n";
            f << "  spriteGenerateFallbackPhysicsShape: 1\n";
            f << "  alphaUsage: 1\n";
            f << "  alphaIsTransparency: 1\n";
            f << "  spriteTessellationDetail: -1\n";
            f << "  textureType: 8\n";
            f << "  textureShape: 1\n";
            f << "  maxTextureSizeSet: 0\n";
            f << "  compressionQualitySet: 0\n";
            f << "  textureFormatSet: 0\n";
            f << "  ignorePngGamma: 0\n";
            f << "  applyGammaDecoding: 0\n";
            f << "  swizzle: 50462976\n";
            //we need most platformsettings here as unity will throw
            //"Assertion failed on expression: 'm_PlatformSettings.empty()'"
            //it doesn't affect anything because the meta file still loads but it's annoying
            f << "  platformSettings:\n";
            f << "  - serializedVersion: 4\n";
            f << "    buildTarget: DefaultTexturePlatform\n";
            f << "    maxTextureSize: 2048\n";
            f << "    resizeAlgorithm: 0\n";
            f << "    textureFormat: 4\n";
            f << "    textureCompression: 1\n";
            f << "    compressionQuality: 50\n";
            f << "    crunchedCompression: 0\n";
            f << "    allowsAlphaSplitting: 0\n";
            f << "    overriden: 0\n";
            f << "    ignorePlatformSupport: 0\n";
            f << "  - serializedVersion: 4\n";
            f << "    buildTarget: Standalone\n";
            f << "    maxTextureSize: 2048\n";
            f << "    resizeAlgorithm: 0\n";
            f << "    textureFormat: 4\n";
            f << "    textureCompression: 1\n";
            f << "    compressionQuality: 50\n";
            f << "    crunchedCompression: 0\n";
            f << "    allowsAlphaSplitting: 0\n";
            f << "    overriden: 0\n";
            f << "    ignorePlatformSupport: 0\n";

            if (!xyEqual(editor->ssne.tileDimensions, { 0,0 })) {
                std::string fileName = fileNameFromPath(convertStringToUTF8OnWin32(editor->lastConfirmedSavePath));

                std::map<std::string, int> spriteIDs;

                f << "  spriteSheet:\n";
                f << "    serializedVersion: 0\n";
                f << "    sprites:\n";

                XY tileSize = editor->ssne.tileDimensions;
                XY tileCounts = {
                    (int)ceil(editor->canvas.dimensions.x / (double)editor->ssne.tileDimensions.x),
                    (int)ceil(editor->canvas.dimensions.y / (double)editor->ssne.tileDimensions.y)
                };

                for (int x = 0; x < tileCounts.x; x++) {
                    for (int y = 0; y < tileCounts.y; y++) {
                        std::string newName = frmt("{}__{}_{}", fileName, y, x);
                        int newID = PackRGBAtoARGB((u8)randomInt(0, 256), (u8)randomInt(0, 256), (u8)randomInt(0, 256), (u8)randomInt(0, 256));
                        f << "    - serializedVersion: 0\n";
                        f << "      name: " << newName << "\n";
                        f << "      rect:\n";
                        f << "        serializedVersion: 0\n";
                        f << "        x: " << (x * tileSize.x) << "\n";
                        f << "        y: " << (editor->canvas.dimensions.y - ((1+y) * tileSize.y)) << "\n";
                        f << "        width: " << tileSize.x << "\n";
                        f << "        height: " << tileSize.y << "\n";
                        f << "      alignment: 0\n";
                        f << "      pivot: {x: 0, y: 0}\n";
                        f << "      border: {x: 0, y: 0, z: 0, w: 0}\n";
                        f << "      physicsShape: []\n";
                        f << "      tessellationDetail: 0\n";
                        f << "      bones: []\n";
                        f << "      spriteID: " << randomUnityGUID() << "\n";
                        f << "      internalID: " << newID << "\n";
                        f << "      vertices: []\n";
                        f << "      indices: \n";
                        f << "      edges: []\n";
                        f << "      weights: []\n";
                        spriteIDs[newName] = newID;
                    }
                }
                f << "    outline: []\n";
                f << "    physicsShape: []\n";
                f << "    bones: []\n";
                f << "    spriteID: " << randomUnityGUID() << "\n";
                f << "    internalID: 0\n";
                f << "    vertices: []\n";
                f << "    indices: \n";
                f << "    edges: []\n";
                f << "    weights: []\n";
                f << "    secondaryTextures: []\n";
                f << "    nameFileIdTable:\n";
                for (auto& id : spriteIDs) {
                    f << frmt("      {}: {}\n", id.first, id.second);
                }
            }
            f.close();

            g_addNotificationFromThread(SuccessNotification("Generated .meta file.", ""));
        } else {
            g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), "Failed to save file."));
        }
    }
    else {
        g_addNotificationFromThread(ErrorNotification(TL("vsp.cmn.error"), "Save the file first."));
    }
}
