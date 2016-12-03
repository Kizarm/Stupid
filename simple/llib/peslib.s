  .text
  .globl pesll
  .globl tscript
pesll:
  .quad peslld
tscript:
  .quad script
peslld:
  .incbin "./llib/simpledef.ll"
  .byte 0
script:
  .incbin "./llib/script.ld"
  .byte 0

