grit character_idle.bmp -gu8 -ftc -gB4 

# tile width & height: 16
# tile trans: f8f8f8
# bpp: 4bit
# tile size: 8bit
# output type: .c
grit tileset.bmp -gTF8F8F8 -gu8 -thF -twf -ftc -gB4

mv *.c ../source
mv *.h ../source
