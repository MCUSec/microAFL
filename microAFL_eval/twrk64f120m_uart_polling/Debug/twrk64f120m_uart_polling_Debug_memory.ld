/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright (c) 2008-2013 Code Red Technologies Ltd,
 * Copyright 2015, 2018-2019 NXP
 * (c) NXP Semiconductors 2013-2022
 * Generated linker script file for MK64FN1M0xxx12
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.3.1 [Build 5262] [2021-04-02] on Feb 8, 2022 3:02:21 AM
 */

MEMORY
{
  /* Define each memory region */
  PROGRAM_FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x100000 /* 1M bytes (alias Flash) */  
  SRAM_UPPER (rwx) : ORIGIN = 0x20000000, LENGTH = 0x30000 /* 192K bytes (alias RAM) */  
  SRAM_LOWER (rwx) : ORIGIN = 0x1fff0000, LENGTH = 0x10000 /* 64K bytes (alias RAM2) */  
  FLEX_RAM (rwx) : ORIGIN = 0x14000000, LENGTH = 0x1000 /* 4K bytes (alias RAM3) */  
}

  /* Define a symbol for the top of each memory region */
  __base_PROGRAM_FLASH = 0x0  ; /* PROGRAM_FLASH */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_PROGRAM_FLASH = 0x0 + 0x100000 ; /* 1M bytes */  
  __top_Flash = 0x0 + 0x100000 ; /* 1M bytes */  
  __base_SRAM_UPPER = 0x20000000  ; /* SRAM_UPPER */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_SRAM_UPPER = 0x20000000 + 0x30000 ; /* 192K bytes */  
  __top_RAM = 0x20000000 + 0x30000 ; /* 192K bytes */  
  __base_SRAM_LOWER = 0x1fff0000  ; /* SRAM_LOWER */  
  __base_RAM2 = 0x1fff0000 ; /* RAM2 */  
  __top_SRAM_LOWER = 0x1fff0000 + 0x10000 ; /* 64K bytes */  
  __top_RAM2 = 0x1fff0000 + 0x10000 ; /* 64K bytes */  
  __base_FLEX_RAM = 0x14000000  ; /* FLEX_RAM */  
  __base_RAM3 = 0x14000000 ; /* RAM3 */  
  __top_FLEX_RAM = 0x14000000 + 0x1000 ; /* 4K bytes */  
  __top_RAM3 = 0x14000000 + 0x1000 ; /* 4K bytes */  
