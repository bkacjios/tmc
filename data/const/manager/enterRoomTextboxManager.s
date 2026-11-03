	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

@ gUnk_08108DE8:: @ 08108DE8
@ 	.incbin "enterRoomTextboxManager/gUnk_08108DE8.bin"

@ EnterRoomTextboxManager_Actions:: @ 08108E28 
@ 	.4byte sub_0805E140
@ 	.4byte sub_0805E18C

gUnk_08108E30:: @ 08108E30
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_08108E30.inc"
.else
	.incbin "enterRoomTextboxManager/gUnk_08108E30.bin"
.endif

gUnk_08108E48:: @ 08108E48
.ifdef PLATFORM_PC64
	.include "data/pc64/gUnk_08108E48.inc"
.else
	.incbin "enterRoomTextboxManager/gUnk_08108E48.bin"
.endif

gUnk_08108E60:: @ 08108E60
	.incbin "enterRoomTextboxManager/gUnk_08108E60.bin"
