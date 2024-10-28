# About custom formats

A few custom formats have been created for voidsprite. These formats were made to be simple to implement in code, and this document will explain the layout of the data.


## voidsprite Session (.voidsn)

> A .hexpat file for ImHex is included &gt; `voidsn.hexpat`

VOIDSN is a format that evolved over time, with every next iteration changing something in the file layout.  
The first byte will always be the version number.

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| 1 | uint8_t | `versionNumber` |

> All strings use UTF-8, but only data input by the user (layer names, comments) will contain UTF-8 characters.  
> Additionally, strings are not NULL-terminated.

### voidsprite Session v1 (.voidsnv1)

`versionNumber` will be equal to `0x01`

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| 4 | uint32_t | `imageWidth` |
| 4 | uint32_t | `imageHeight` |
| 4 | uint32_t | `numberOfLayers` |

For each layer:

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| 4 * `imageWidth` * `imageHeight` | uint32_t[`imageWidth` * `imageHeight`] | `pixelData` |


### voidsprite Session v2 (.voidsnv2)

`versionNumber` will be equal to `0x02`

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| 4 | uint32_t | `imageWidth` |
| 4 | uint32_t | `imageHeight` |
| 4 | uint32_t | `tileWidth` |
| 4 | uint32_t | `tileHeight` |
| 4 | uint32_t | `numberOfLayers` |

For each layer:

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| 4 | uint32_t | `lengthOfLayerName` |
| `lengthOfLayerName` | char[`lengthOfLayerName`] | `layerName` |
| 4 * `imageWidth` * `imageHeight` | uint32_t[`imageWidth` * `imageHeight`] | `pixelData` |


### voidsprite Session v3 (.voidsnv3)

`versionNumber` will be equal to `0x03`

#### Required types:

##### `VOIDSNString`:
| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| 4 | uint32_t | `lengthOfString` |
| `lengthOfString` | char[`lengthOfString`] | `stringText` |

##### `VOIDSNExtData`:
| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| &lt;variable&gt; | `VOIDSNString` | `key` |
| &lt;variable&gt; | `VOIDSNString` | `value` |

#### File:

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| 4 | uint32_t | `imageWidth` |
| 4 | uint32_t | `imageHeight` |
| 13 | char[13] | `extendedDataHeader` |
| 4 | uint32_t | `extendedDataCount` |
| &lt;variable&gt; | `VOIDSNExtData`[`extendedDataCount`] | `extendedData` |
| 4 | uint32_t | `numberOfLayers` |


For each layer:

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| &lt;variable&gt; | `VOIDSNString` | `layerName` |
| 1 | bool | `hasColorKey` |
| 4 | uint32_t | `colorKey` |
| 4 * `imageWidth` * `imageHeight` | uint32_t[`imageWidth` * `imageHeight`] | `pixelData` |

`extendedDataHeader` should be the string `/VOIDSN.META/`

#### Extended data keys:
`tile.dim.x`: Width of the tile grid. 0 if disabled.  
`tile.dim.y`: Height of the tile grid. 0 if disabled.  
`tile.dim.padrx`: Right padding of the tile grid. 0 if disabled.  
`tile.dim.padby`: Bottom padding of the tile grid. 0 if disabled.  
`sym.enabled`: Formatted string signifying what kind of symmetry is currently enabled. The format is `"XY"`, where both characters can be either `'0'` or `'1'`, for example `"01"` will mean that X symmetry is disabled and Y symmetry is enabled.  
`sym.x`: Current X symmetry position. Can be placed at 0.5 subpixel positions, so needs to be divided by 2 when reading.  
`sym.y`: Current Y symmetry position. Can be placed at 0.5 subpixel positions, so needs to be divided by 2 when reading.  
`layer.selected`: Index of the current active layer.  
`layer.visibility`: A string with a `'0'` or `'1'` character for every layer. `'0'` means the layer is hidden, `'1'` means it is not.  
`layer.opacity`: A formatted string in the format: `"<Layer 1 opacity>;<Layer 2 opacity>;<Layer 3 opacity>;..."`. The layer opacity is a number from 0 to 255.  
`comments`: Comment data in the following format:  
`(Num. comments);` then for every comment: `(Pos. X);(Pos. Y);(Comment string);`  
`guidelines`: Guidelines data in the following format:  
`(Num. guidelines);` then for every guideline: `(Orientation)-(Position);`, where orientation is either `v` or `h` indicating vertical or horizontal, and `Position` is its position on the canvas. Like symmetry lines, the position needs to be divided by 2 when reading.  

### voidsprite Session v4 (.voidsnv4)

`versionNumber` will be equal to `0x04`  
VOIDSNv4 is identical to v3, but adds support for palettized images.  
The image is palettized if the extended data key `palette.enabled` == `"1"`  
The palette colors can then be found in the ext. data key `palette.colors`, which is a formatted string:
`"<num colors>;<HEX COLOR 1>;<HEX COLOR 2>;<HEX COLOR 3>;..."`
`pixelData` becomes an array of int32 indexes. An index can be `-1`, meaning it's fully transparent.

### voidsprite Session v5 (.voidsnv5)

`versionNumber` will be equal to `0x05`
VOIDSNv5 is identical to v4, but changes the structure of the individual layers to accomodate for pixel data compression.  
The pixel data gets compressed with zlib.

For each layer:

| Num. bytes | C data type | Name |
| :--------: | :---------: | :--- |
| &lt;variable&gt; | `VOIDSNString` | `layerName` |
| 1 | bool | `hasColorKey` |
| 4 | uint32_t | `colorKey` |
| 8 | uint64_t | `sizeOfCompressedData` |
| `sizeOfCompressedData` | uint8_t[`sizeOfCompressedData`] | `compressedPixelData` |
