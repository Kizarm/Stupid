  .section .rodata
  .globl pesllxx
  .globl tscript
pesllxx:
  .long peslld
tscript:
  .long script
peslld:
  .incbin "./llib/simpledef.ll"
  .byte 0
script:
  .incbin "./llib/script.ld"
  .byte 0

