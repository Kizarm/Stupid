  .text
  .globl pesll
pesll:
  .quad peslld
peslld:
  .incbin "./llib/simpledef.ll"
  .byte 0
