  .section .rodata
  .globl libwasm
  .globl libwasm_size
  .globl index_html
  .globl index_html_size
  .globl symbols_txt
  .globl symbols_txt_size
  .align 4
libwasm:
  .incbin "./wasm/libWASM.a"
e_libwasm:
  .align 4
libwasm_size:
  .long e_libwasm - libwasm
  
index_html:
  .incbin "./wasm/index.html"
e_index_html:
  .align 4
index_html_size:
  .long e_index_html - index_html

symbols_txt:
  .incbin "./wasm/symbols.txt"
e_symbols_txt:
  .align 4
symbols_txt_size:
  .long e_symbols_txt - symbols_txt
