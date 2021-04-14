
//{{BLOCK(tileset)

//======================================================================
//
//	tileset, 256x128@8, 
//	Transparent color : F8,F8,F8
//	+ palette 256 entries, not compressed
//	+ 512 tiles not compressed
//	Total size: 512 + 32768 = 33280
//
//	Time-stamp: 2021-04-13, 16:38:05
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.16
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TILESET_H
#define GRIT_TILESET_H

#define tilesetTilesLen 32768
extern const unsigned char tilesetTiles[32768];

#define tilesetPalLen 512
extern const unsigned short tilesetPal[256];

#endif // GRIT_TILESET_H

//}}BLOCK(tileset)
