    .section .iwram, "ax", %progbits
    .text
    .align 2
    .code 32
    .global mem_cpy16, mem_set16
    .type mem_cpy16 STT_FUNC

mem_cpy16:
    @ r0 = void *dst
    @ r1 = void *src
    @ r2 = uint size
    lsr r2, #1

    push {r4}

.loop:
    ldrh r4, [r1]
    strh r4, [r0]

    add r0, r0, #2
    add r1, r1, #2

    subs r2, #1
    bne .loop

    pop {r4}

    bx lr

    .type mem_set16 STT_FUNC
mem_set16:
    
    @ r0 = void *dst
    @ r1 = u16 value
    @ r2 = uint size
.loop:
    