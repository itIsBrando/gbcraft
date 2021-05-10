grit character_idle.bmp -gTEBFB00 -gu8 -ftc -gB4

# tile width & height: 16
# tile trans: f8f8f8
# bpp: 8bit
# tile size: 8bit
# output type: .c
grit tileset.bmp -gTF8F8F8 -gu8 -thf -twf -ftc -gB8

# tile width & height: 8
# bpp: 4bit
# tile size: 8bit
# output type: .c
# no palette
grit tiles16.bmp -gu8 -th8 -tw8 -ftc -gB4 -p!

mv *.c ../source
mv *.h ../source
