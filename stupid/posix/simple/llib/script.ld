/* Entry Point - musí být definován     */
ENTRY(GetPesApiPtr)

/* Specifikace paměťových prostorů čipu */
MEMORY {
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
  ROM (xr)        : ORIGIN = 0x08008000, LENGTH = 64K
}
/* Kam umístíme stack - navrch RAM */
_estack = ORIGIN(RAM) + LENGTH(RAM);
/* Definice výstupních sekcí       */
SECTIONS {
  .fixed : {
    _vect_tab = .;
    *(.text.GetPesApiPtr)
    *(.text*)            /* .text*   sections (code)                              */
    *(.rodata*)          /* .rodata* sections (constants, strings, etc.)          */
    *(.eh_frame)
  } >ROM
  .reloc : {
    *(.data*)            /* .data* sections */
    *(.bss*)             /* .bss*  sections */
    *(COMMON)
  } >RAM

  .ARM.attributes 0 : { *(.ARM.attributes*) }
}
