    .section .rodata
    .align 2
    .global FLASH_ID_TEXT
FLASH_ID_TEXT:
    .string "FLASH512_Vnnn"
    .word 0,0

    .text
    .align 2
    .code 16
    .global foo
    .type foo, %function
foo:
    bx lr