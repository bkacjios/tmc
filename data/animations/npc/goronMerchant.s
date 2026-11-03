	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
@    .align 2

gSpriteAnimations_GoronMerchant_1:: @ 08111BBE
	.include "animations/gSpriteAnimations_GoronMerchant_1.s"

gSpriteAnimations_GoronMerchant_0:: @ 08111BC3
	.include "animations/gSpriteAnimations_GoronMerchant_0.s"

gSpriteAnimations_GoronMerchant_3:: @ 08111BC8
	.include "animations/gSpriteAnimations_GoronMerchant_3.s"

gSpriteAnimations_GoronMerchant_5:: @ 08111BCD
	.include "animations/gSpriteAnimations_GoronMerchant_5.s"

gSpriteAnimations_GoronMerchant_7:: @ 08111BF2
	.include "animations/gSpriteAnimations_GoronMerchant_7.s"

gSpriteAnimations_GoronMerchant:: @ 08111C18
	ptr gSpriteAnimations_GoronMerchant_0
	ptr gSpriteAnimations_GoronMerchant_1
	ptr gSpriteAnimations_GoronMerchant_0
	ptr gSpriteAnimations_GoronMerchant_3
	ptr gSpriteAnimations_GoronMerchant_0
	ptr gSpriteAnimations_GoronMerchant_5
	ptr gSpriteAnimations_GoronMerchant_0
	ptr gSpriteAnimations_GoronMerchant_7
	ptr 00000000
