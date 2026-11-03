
	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata

gSpriteAnimations_SpearMoblin_0:: @ 080CC7E0
	.include "animations/gSpriteAnimations_SpearMoblin_0.s"

gSpriteAnimations_SpearMoblin_1:: @ 080CC7E9
	.include "animations/gSpriteAnimations_SpearMoblin_1.s"

gSpriteAnimations_SpearMoblin_2:: @ 080CC7F2
	.include "animations/gSpriteAnimations_SpearMoblin_2.s"

gSpriteAnimations_SpearMoblin_3:: @ 080CC7FB
	.include "animations/gSpriteAnimations_SpearMoblin_3.s"

gSpriteAnimations_SpearMoblin_4:: @ 080CC804
	.include "animations/gSpriteAnimations_SpearMoblin_4.s"

gSpriteAnimations_SpearMoblin_5:: @ 080CC815
	.include "animations/gSpriteAnimations_SpearMoblin_5.s"

gSpriteAnimations_SpearMoblin_6:: @ 080CC826
	.include "animations/gSpriteAnimations_SpearMoblin_6.s"

gSpriteAnimations_SpearMoblin_7:: @ 080CC837
	.include "animations/gSpriteAnimations_SpearMoblin_7.s"

gSpriteAnimations_SpearMoblin_8:: @ 080CC848
	.include "animations/gSpriteAnimations_SpearMoblin_8.s"

gSpriteAnimations_SpearMoblin_9:: @ 080CC859
	.include "animations/gSpriteAnimations_SpearMoblin_9.s"

gSpriteAnimations_SpearMoblin_10:: @ 080CC86A
	.include "animations/gSpriteAnimations_SpearMoblin_10.s"

gSpriteAnimations_SpearMoblin_11:: @ 080CC87B
	.include "animations/gSpriteAnimations_SpearMoblin_11.s"

gSpriteAnimations_SpearMoblin_12:: @ 080CC88C
	.include "animations/gSpriteAnimations_SpearMoblin_12.s"

gSpriteAnimations_SpearMoblin_13:: @ 080CC8A1
	.include "animations/gSpriteAnimations_SpearMoblin_13.s"

gSpriteAnimations_SpearMoblin_14:: @ 080CC8B6
	.include "animations/gSpriteAnimations_SpearMoblin_14.s"

gSpriteAnimations_SpearMoblin_15:: @ 080CC8CB
	.include "animations/gSpriteAnimations_SpearMoblin_15.s"

gSpriteAnimations_SpearMoblin:: @ 080CC8E0
	ptr gSpriteAnimations_SpearMoblin_0
	ptr gSpriteAnimations_SpearMoblin_1
	ptr gSpriteAnimations_SpearMoblin_2
	ptr gSpriteAnimations_SpearMoblin_3
	ptr gSpriteAnimations_SpearMoblin_4
	ptr gSpriteAnimations_SpearMoblin_5
	ptr gSpriteAnimations_SpearMoblin_6
	ptr gSpriteAnimations_SpearMoblin_7
	ptr gSpriteAnimations_SpearMoblin_8
	ptr gSpriteAnimations_SpearMoblin_9
	ptr gSpriteAnimations_SpearMoblin_10
	ptr gSpriteAnimations_SpearMoblin_11
	ptr gSpriteAnimations_SpearMoblin_12
	ptr gSpriteAnimations_SpearMoblin_13
	ptr gSpriteAnimations_SpearMoblin_14
	ptr gSpriteAnimations_SpearMoblin_15
	ptr 00000000

gUnk_080CC924:: @ 080CC924
	.incbin "spearMoblin/gUnk_080CC924.bin"

gUnk_080CC92C:: @ 080CC92C
	.incbin "spearMoblin/gUnk_080CC92C.bin"

gUnk_080CC934:: @ 080CC934
	.incbin "spearMoblin/gUnk_080CC934.bin"

gUnk_080CC93C:: @ 080CC93C
	.incbin "spearMoblin/gUnk_080CC93C.bin"

gUnk_080CC944:: @ 080CC944
	ptr gUnk_080CC924
	ptr gUnk_080CC92C
	ptr gUnk_080CC934
	ptr gUnk_080CC93C
