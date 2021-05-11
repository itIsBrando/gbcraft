    .section .iwram, "ax", %progbits
    .text
    .align 2
    .code 32
    .global memcpy16, memset16
    .type memcpy16 STT_FUNC

memcpy16:
    @ r0 = void *dst
    @ r1 = void *src
    @ r2 = uint size
    lsrs r2, #1
    bxeq lr

    push {r4}

.memcpyLoop:
    ldrh r4, [r1]
    strh r4, [r0]

    add r0, r0, #2
    add r1, r1, #2

    subs r2, #1
    bne .memcpyLoop

    pop {r4}

    bx lr


    .type memset16 STT_FUNC
memset16:
    
    @ r0 = void *dst
    @ r1 = u16 value
    @ r2 = uint size (in bytes)
    lsrs r2, #1
    bxeq lr

.memsetLoop:
    strh r1, [r0]

    add r0, r0, #2

    subs r2, #1
    bne .memsetLoop

    bx lr