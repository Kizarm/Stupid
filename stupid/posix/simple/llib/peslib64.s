  .section .rodata
  .globl pesllxx
  .globl tscript
  .align 8
pesllxx:
  .quad peslld
tscript:
  .quad script
  .align 8
peslld:
  .incbin "./llib/simpledef.ll"
  .byte 0
  .align 8
script:
  .incbin "./llib/script.ld"
  .byte 0

