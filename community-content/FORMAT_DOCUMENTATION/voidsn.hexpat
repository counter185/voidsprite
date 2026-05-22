struct VOIDSNString {
    u32 stringLength;
    char string[stringLength];
};

struct VOIDSNExtData {
    VOIDSNString key;
    VOIDSNString value;
};


//  _______________
// |
// | VOIDSNv6
// |

struct VOIDSNv6LayerData {
    VOIDSNString name;
    u64 dataSize;
    u8 data[dataSize];
};
struct VOIDSNv6Layer {
    u32 numData;
    VOIDSNv6LayerData layerData[numData];
};
struct VOIDSNv6 {
    char magic[11];
    u32 imageWidth;
    u32 imageHeight;
    char extDataHeader[13];
    u32 extDataSize;
    VOIDSNExtData extData[extDataSize];
    u32 numLayers;
    
    VOIDSNv6Layer layers[numLayers];
};


//  _______________
// |
// | VOIDSNv5
// |

struct VOIDSNv5Layer {
    VOIDSNString name;
    bool hasColorKey;
    u32 colorKey;
    u64 compressedSize;
    u8 compressedPixelData[compressedSize];
};
struct VOIDSNv5 {
    u32 imageWidth;
    u32 imageHeight;
    char extDataHeader[13];
    u32 extDataSize;
    VOIDSNExtData extData[extDataSize];
    u32 numLayers;
    
    VOIDSNv5Layer layers[numLayers];
};


//  _______________
// |
// | VOIDSNv3/4
// |

struct VOIDSNv3Layer {
    VOIDSNString name;
    bool hasColorKey;
    u32 colorKey;
    u32 layerData[parent.imageWidth * parent.imageHeight];
};
struct VOIDSNv3 {
    u32 imageWidth;
    u32 imageHeight;
    char extDataHeader[13];
    u32 extDataSize;
    VOIDSNExtData extData[extDataSize];
    u32 numLayers;
    
    VOIDSNv3Layer layers[numLayers];
};


//  _______________
// |
// | VOIDSNv2
// |

struct VOIDSNv2Layer {
    VOIDSNString name;
    u32 layerData[parent.imageWidth * parent.imageHeight];
};
struct VOIDSNv2 {
    u32 imageWidth;
    u32 imageHeight;
    u32 tileWidth;
    u32 tileHeight;
    u32 numLayers;
    
    VOIDSNv2Layer layers[numLayers];
};


//  _______________
// |
// | VOIDSNv1
// |

struct VOIDSNv1Layer {
    u32 layerData[parent.imageWidth * parent.imageHeight];
};
struct VOIDSNv1 {
    u32 imageWidth;
    u32 imageHeight;
    u32 numLayers;
    
    VOIDSNv1Layer layers[numLayers];
};



struct VOIDSNfile {
    u8 version;
    if (version == 1) {
        VOIDSNv1 v1File;
    }
    else if (version == 2) {
        VOIDSNv2 v2File;
    }
    else if (version == 3) {
        VOIDSNv3 v3File;
    }
    else if (version == 4) {
        VOIDSNv3 v4File;
    }
    else if (version == 5) {
        VOIDSNv5 v5File;
    }
    else if (version == 6) {
        VOIDSNv6 v6File;
    }
};

VOIDSNfile voidsn @ 0;