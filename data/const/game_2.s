	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
	.align 2


@ kinstone.c, worldEvent2.c
gUnk_080FEAC8:: @ 080FEAC8
	.incbin "data_080FC8A4/gUnk_080FEAC8.bin"
.ifdef JP
    @ TODO only small differences
	.incbin "data_080FC8A4/gUnk_080FEAC8_1_JP.bin"
.else
.ifdef DEMO_JP
    @ TODO only small differences
	.incbin "data_080FC8A4/gUnk_080FEAC8_2_DEMO_JP.bin"
.else
.ifdef EU
	.incbin "data_080FC8A4/gUnk_080FEAC8_3_EU.bin"
.else
	.incbin "data_080FC8A4/gUnk_080FEAC8_4_USA-DEMO_USA.bin"
.endif
.endif
.endif

.ifdef JP
    @ TODO only small differences
	.incbin "data_080FC8A4/gUnk_080FEAC8_5_JP.bin"
.else
.ifdef DEMO_JP
    @ TODO only small differences
	.incbin "data_080FC8A4/gUnk_080FEAC8_6_DEMO_JP.bin"
.else
.ifdef EU
	.incbin "data_080FC8A4/gUnk_080FEAC8_7_EU.bin"
.else
	.incbin "data_080FC8A4/gUnk_080FEAC8_8_USA-DEMO_USA.bin"
.endif
.endif
.endif

	.incbin "data_080FC8A4/gUnk_080FEAC8_9.bin"

@ kinstone.c
gUnk_080FEBE8:: @ 080FEBE8
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FEBE8.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FEBE8.bin"
.endif

gUnk_080FEC28:: @ 080FEC28
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FEC28.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FEC28.bin"
.endif

gUnk_080FECC8:: @ 080FECC8
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FECC8.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FECC8.bin"
.endif

gUnk_080FED18:: @ 080FED18
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FED18.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FED18.bin"
.endif

gUnk_080FED58:: @ 080FED58
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FED58.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FED58.bin"

@ kinstone.c
.endif
gLilypadRails:: @ 080FED98
	ptr gUnk_080FEDA4
	ptr gUnk_080FEDDA
	ptr gUnk_080FEDF8

gUnk_080FEDA4:: @ 080FEDA4
	.incbin "data_080FC8A4/gUnk_080FEDA4.bin"

gUnk_080FEDDA:: @ 080FEDDA
	.incbin "data_080FC8A4/gUnk_080FEDDA.bin"

gUnk_080FEDF8:: @ 080FEDF8
	.incbin "data_080FC8A4/gUnk_080FEDF8.bin"

gUnk_080FEE18:: @ 080FEE18
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FEE18.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FEE18.bin"
.endif

gUnk_080FEE38:: @ 080FEE38
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FEE38.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FEE38.bin"
.endif

gUnk_080FEE48:: @ 080FEE48
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FEE48.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FEE48.bin"

@ kinstone.c
.endif
gUnk_080FEE58:: @ 080FEE58
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FEE58.inc"
.else
.ifdef JP
    @ TODO only small differences
	.incbin "data_080FC8A4/gUnk_080FEE58_JP.bin"
.else
.ifdef DEMO_JP
    @ TODO only small differences
	.incbin "data_080FC8A4/gUnk_080FEE58_1_DEMO_JP.bin"
.else
.ifdef EU
	.incbin "data_080FC8A4/gUnk_080FEE58_2_EU.bin"
.else
	.incbin "data_080FC8A4/gUnk_080FEE58_3_USA-DEMO_USA.bin"
.endif
.endif
.endif

@ kinstone.c
.endif
gUnk_080FEE78:: @ 080FEE78
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_080FEE78.inc"
.else
	.incbin "data_080FC8A4/gUnk_080FEE78.bin"
.endif
