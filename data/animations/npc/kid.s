	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Kid_0:: @ 0810C0C4
	.include "animations/gSpriteAnimations_Kid_0.s"

gSpriteAnimations_Kid_1:: @ 0810C0C9
	.include "animations/gSpriteAnimations_Kid_1.s"

gSpriteAnimations_Kid_2:: @ 0810C0CE
	.include "animations/gSpriteAnimations_Kid_2.s"

gSpriteAnimations_Kid_3:: @ 0810C0D3
	.include "animations/gSpriteAnimations_Kid_3.s"

gSpriteAnimations_Kid_4:: @ 0810C0D8
	.include "animations/gSpriteAnimations_Kid_4.s"

gSpriteAnimations_Kid_5:: @ 0810C0E9
	.include "animations/gSpriteAnimations_Kid_5.s"

gSpriteAnimations_Kid_6:: @ 0810C0FA
	.include "animations/gSpriteAnimations_Kid_6.s"

gSpriteAnimations_Kid_7:: @ 0810C10B
	.include "animations/gSpriteAnimations_Kid_7.s"

gSpriteAnimations_Kid_20:: @ 0810C11C
	.include "animations/gSpriteAnimations_Kid_20.s"

gSpriteAnimations_Kid_21:: @ 0810C135
	.include "animations/gSpriteAnimations_Kid_21.s"

gSpriteAnimations_Kid_22:: @ 0810C14E
	.include "animations/gSpriteAnimations_Kid_22.s"

gSpriteAnimations_Kid_23:: @ 0810C167
	.include "animations/gSpriteAnimations_Kid_23.s"

gSpriteAnimations_Kid_8:: @ 0810C180
	.include "animations/gSpriteAnimations_Kid_8.s"

gSpriteAnimations_Kid_9:: @ 0810C199
	.include "animations/gSpriteAnimations_Kid_9.s"

gSpriteAnimations_Kid_11:: @ 0810C1A6
	.include "animations/gSpriteAnimations_Kid_11.s"

gSpriteAnimations_Kid_12:: @ 0810C1D7
	.include "animations/gSpriteAnimations_Kid_12.s"

gSpriteAnimations_Kid_13:: @ 0810C1DC
	.include "animations/gSpriteAnimations_Kid_13.s"

gSpriteAnimations_Kid:: @ 0810C1E4
	ptr gSpriteAnimations_Kid_0
	ptr gSpriteAnimations_Kid_1
	ptr gSpriteAnimations_Kid_2
	ptr gSpriteAnimations_Kid_3
	ptr gSpriteAnimations_Kid_4
	ptr gSpriteAnimations_Kid_5
	ptr gSpriteAnimations_Kid_6
	ptr gSpriteAnimations_Kid_7
	ptr gSpriteAnimations_Kid_8
	ptr gSpriteAnimations_Kid_9
	ptr gSpriteAnimations_Kid_9
	ptr gSpriteAnimations_Kid_11
	ptr gSpriteAnimations_Kid_12
	ptr gSpriteAnimations_Kid_13
	ptr gSpriteAnimations_Kid_0
	ptr gSpriteAnimations_Kid_0
	ptr gSpriteAnimations_Kid_0
	ptr gSpriteAnimations_Kid_1
	ptr gSpriteAnimations_Kid_2
	ptr gSpriteAnimations_Kid_3
	ptr gSpriteAnimations_Kid_20
	ptr gSpriteAnimations_Kid_21
	ptr gSpriteAnimations_Kid_22
	ptr gSpriteAnimations_Kid_23
	ptr gSpriteAnimations_Kid_8
	ptr gSpriteAnimations_Kid_9
	ptr gSpriteAnimations_Kid_9
	ptr gSpriteAnimations_Kid_11
	ptr gSpriteAnimations_Kid_12
	ptr gSpriteAnimations_Kid_13
	ptr gSpriteAnimations_Kid_0
	ptr gSpriteAnimations_Kid_0
	ptr 00000000

@ TODO data of an unused entity?
@ maybe the unused original guard npc?
gUnk_0810C268:: @ 0810C268
	ptr sub_08062AD0
	ptr sub_08062AD4

gUnk_0810C270:: @ 0810C270
	.incbin "kid/gUnk_0810C270.bin"

gUnk_0810C275:: @ 0810C275
	.incbin "kid/gUnk_0810C275.bin"

gUnk_0810C27A:: @ 0810C27A
	.incbin "kid/gUnk_0810C27A.bin"

gUnk_0810C280:: @ 0810C280
	ptr gUnk_0810C270
	ptr gUnk_0810C275
	ptr gUnk_0810C27A
	ptr 00000000
