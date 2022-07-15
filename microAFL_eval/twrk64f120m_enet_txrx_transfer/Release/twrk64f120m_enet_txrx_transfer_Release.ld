/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2021
 * Generated linker script file for MK64FN1M0xxx12
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.3.1 [Build 5262] [2021-04-02] on Nov 13, 2021 7:42:45 PM
 */

INCLUDE "twrk64f120m_enet_txrx_transfer_Release_library.ld"
INCLUDE "twrk64f120m_enet_txrx_transfer_Release_memory.ld"

ENTRY(ResetISR)

SECTIONS
{
     /* MAIN TEXT SECTION */
    .text : ALIGN(8)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        LONG(LOADADDR(.data_RAM3));
        LONG(    ADDR(.data_RAM3));
        LONG(  SIZEOF(.data_RAM3));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        LONG(    ADDR(.bss_RAM3));
        LONG(  SIZEOF(.bss_RAM3));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        *(.after_vectors*)

        /* Kinetis Flash Configuration data */
        . = 0x400 ;
        PROVIDE(__FLASH_CONFIG_START__ = .) ;
        KEEP(*(.FlashConfig))
        PROVIDE(__FLASH_CONFIG_END__ = .) ;
        ASSERT(!(__FLASH_CONFIG_START__ == __FLASH_CONFIG_END__), "Linker Flash Config Support Enabled, but no .FlashConfig section provided within application");
        /* End of Kinetis Flash Configuration data */
        
    } > PROGRAM_FLASH

    .text : ALIGN(8)
    {
       *(.text*)
       *(.rodata .rodata.* .constdata .constdata.*)
       . = ALIGN(8);
    } > PROGRAM_FLASH
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(8)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > PROGRAM_FLASH

    .ARM.exidx : ALIGN(8)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > PROGRAM_FLASH
 
    _etext = .;
        
 
    /* USB_RAM */
    .m_usb_data (NOLOAD) :
    {
        *(m_usb_bdt)
    } > SRAM_UPPER AT> SRAM_UPPER
    /* DATA section for SRAM_LOWER */

    .data_RAM2 : ALIGN(8)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        PROVIDE(__start_data_SRAM_LOWER = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$SRAM_LOWER)
        *(.data.$RAM2)
        *(.data.$SRAM_LOWER)
        *(.data.$RAM2.*)
        *(.data.$SRAM_LOWER.*)
        . = ALIGN(8) ;
        PROVIDE(__end_data_RAM2 = .) ;
        PROVIDE(__end_data_SRAM_LOWER = .) ;
     } > SRAM_LOWER AT>PROGRAM_FLASH

    /* DATA section for FLEX_RAM */

    .data_RAM3 : ALIGN(8)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM3 = .) ;
        PROVIDE(__start_data_FLEX_RAM = .) ;
        *(.ramfunc.$RAM3)
        *(.ramfunc.$FLEX_RAM)
        *(.data.$RAM3)
        *(.data.$FLEX_RAM)
        *(.data.$RAM3.*)
        *(.data.$FLEX_RAM.*)
        . = ALIGN(8) ;
        PROVIDE(__end_data_RAM3 = .) ;
        PROVIDE(__end_data_FLEX_RAM = .) ;
     } > FLEX_RAM AT>PROGRAM_FLASH

    /* MAIN DATA SECTION */
    .uninit_RESERVED (NOLOAD) : ALIGN(8)
    {
        _start_uninit_RESERVED = .;
        KEEP(*(.bss.$RESERVED*))
       . = ALIGN(8) ;
        _end_uninit_RESERVED = .;
    } > SRAM_UPPER AT> SRAM_UPPER

    /* Main DATA section (SRAM_UPPER) */
    .data : ALIGN(8)
    {
       FILL(0xff)
       _data = . ;
       PROVIDE(__start_data_RAM = .) ;
       PROVIDE(__start_data_SRAM_UPPER = .) ;
       *(vtable)
       *(.ramfunc*)
       KEEP(*(CodeQuickAccess))
       KEEP(*(DataQuickAccess))
       *(RamFunction)
       *(.data*)
       . = ALIGN(8) ;
       _edata = . ;
       PROVIDE(__end_data_RAM = .) ;
       PROVIDE(__end_data_SRAM_UPPER = .) ;
    } > SRAM_UPPER AT>PROGRAM_FLASH

    /* BSS section for SRAM_LOWER */
    .bss_RAM2 : ALIGN(8)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
       PROVIDE(__start_bss_SRAM_LOWER = .) ;
       *(.bss.$RAM2)
       *(.bss.$SRAM_LOWER)
       *(.bss.$RAM2.*)
       *(.bss.$SRAM_LOWER.*)
       . = ALIGN (. != 0 ? 8 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
       PROVIDE(__end_bss_SRAM_LOWER = .) ;
    } > SRAM_LOWER AT> SRAM_LOWER

    /* BSS section for FLEX_RAM */
    .bss_RAM3 : ALIGN(8)
    {
       PROVIDE(__start_bss_RAM3 = .) ;
       PROVIDE(__start_bss_FLEX_RAM = .) ;
       *(.bss.$RAM3)
       *(.bss.$FLEX_RAM)
       *(.bss.$RAM3.*)
       *(.bss.$FLEX_RAM.*)
       . = ALIGN (. != 0 ? 8 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM3 = .) ;
       PROVIDE(__end_bss_FLEX_RAM = .) ;
    } > FLEX_RAM AT> FLEX_RAM

    /* MAIN BSS SECTION */
    .bss : ALIGN(8)
    {
        _bss = .;
        PROVIDE(__start_bss_RAM = .) ;
        PROVIDE(__start_bss_SRAM_UPPER = .) ;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8) ;
        _ebss = .;
        PROVIDE(__end_bss_RAM = .) ;
        PROVIDE(__end_bss_SRAM_UPPER = .) ;
        PROVIDE(end = .);
    } > SRAM_UPPER AT> SRAM_UPPER

    /* NOINIT section for SRAM_LOWER */
    .noinit_RAM2 (NOLOAD) : ALIGN(8)
    {
       PROVIDE(__start_noinit_RAM2 = .) ;
       PROVIDE(__start_noinit_SRAM_LOWER = .) ;
       *(.noinit.$RAM2)
       *(.noinit.$SRAM_LOWER)
       *(.noinit.$RAM2.*)
       *(.noinit.$SRAM_LOWER.*)
       . = ALIGN(8) ;
       PROVIDE(__end_noinit_RAM2 = .) ;
       PROVIDE(__end_noinit_SRAM_LOWER = .) ;
    } > SRAM_LOWER AT> SRAM_LOWER

    /* NOINIT section for FLEX_RAM */
    .noinit_RAM3 (NOLOAD) : ALIGN(8)
    {
       PROVIDE(__start_noinit_RAM3 = .) ;
       PROVIDE(__start_noinit_FLEX_RAM = .) ;
       *(.noinit.$RAM3)
       *(.noinit.$FLEX_RAM)
       *(.noinit.$RAM3.*)
       *(.noinit.$FLEX_RAM.*)
       . = ALIGN(8) ;
       PROVIDE(__end_noinit_RAM3 = .) ;
       PROVIDE(__end_noinit_FLEX_RAM = .) ;
    } > FLEX_RAM AT> FLEX_RAM

    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(8)
    {
        _noinit = .;
        PROVIDE(__start_noinit_RAM = .) ;
        PROVIDE(__start_noinit_SRAM_UPPER = .) ;
        *(.noinit*)
         . = ALIGN(8) ;
        _end_noinit = .;
       PROVIDE(__end_noinit_RAM = .) ;
       PROVIDE(__end_noinit_SRAM_UPPER = .) ;        
    } > SRAM_UPPER AT> SRAM_UPPER

    /* Reserve and place Heap within memory map */
    _HeapSize = 0x1000;
    .heap :  ALIGN(8)
    {
        _pvHeapStart = .;
        . += _HeapSize;
        . = ALIGN(8);
        _pvHeapLimit = .;
    } > SRAM_UPPER

     _StackSize = 0x1000;
     /* Reserve space in memory for Stack */
    .heap2stackfill  :
    {
        . += _StackSize;
    } > SRAM_UPPER
    /* Locate actual Stack in memory map */
    .stack ORIGIN(SRAM_UPPER) + LENGTH(SRAM_UPPER) - _StackSize - 0:  ALIGN(8)
    {
        _vStackBase = .;
        . = ALIGN(8);
        _vStackTop = . + _StackSize;
    } > SRAM_UPPER

    /* Provide basic symbols giving location and size of main text
     * block, including initial values of RW data sections. Note that
     * these will need extending to give a complete picture with
     * complex images (e.g multiple Flash banks).
     */
    _image_start = LOADADDR(.text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}