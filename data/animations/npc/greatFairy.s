	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
	.align 2

gUnk_0812079C:: @ 0812079C
	.incbin "greatFairy/gUnk_0812079C.bin"

gUnk_081207A4:: @ 081207A4
	ptr sub_080872AC
	ptr sub_080872F8

gUnk_081207AC:: @ 081207AC
	.incbin "greatFairy/gUnk_081207AC.bin"

gSpriteAnimations_GreatFairy_0:: @ 081207B0
	.include "animations/gSpriteAnimations_GreatFairy_0.s"

gSpriteAnimations_GreatFairy_2:: @ 081207C1
	.include "animations/gSpriteAnimations_GreatFairy_2.s"

gSpriteAnimations_GreatFairy_1:: @ 081207D2
	.include "animations/gSpriteAnimations_GreatFairy_1.s"

gSpriteAnimations_GreatFairy_9:: @ 081207D7
	.include "animations/gSpriteAnimations_GreatFairy_9.s"

gSpriteAnimations_GreatFairy_5:: @ 081207F4
	.include "animations/gSpriteAnimations_GreatFairy_5.s"

gSpriteAnimations_GreatFairy_6:: @ 08120819
	.include "animations/gSpriteAnimations_GreatFairy_6.s"

gSpriteAnimations_GreatFairy_7:: @ 08120826
	.include "animations/gSpriteAnimations_GreatFairy_7.s"

gSpriteAnimations_GreatFairy_3:: @ 0812082F
	.include "animations/gSpriteAnimations_GreatFairy_3.s"

gSpriteAnimations_GreatFairy_4:: @ 08120850
	.include "animations/gSpriteAnimations_GreatFairy_4.s"

gSpriteAnimations_GreatFairy_8:: @ 08120855
	.include "animations/gSpriteAnimations_GreatFairy_8.s"

gSpriteAnimations_GreatFairy:: @ 08120878
	ptr gSpriteAnimations_GreatFairy_0
	ptr gSpriteAnimations_GreatFairy_1
	ptr gSpriteAnimations_GreatFairy_2
	ptr gSpriteAnimations_GreatFairy_3
	ptr gSpriteAnimations_GreatFairy_4
	ptr gSpriteAnimations_GreatFairy_5
	ptr gSpriteAnimations_GreatFairy_6
	ptr gSpriteAnimations_GreatFairy_7
	ptr gSpriteAnimations_GreatFairy_8
	ptr gSpriteAnimations_GreatFairy_9
