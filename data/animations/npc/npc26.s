	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_Npc26_0:: @ 081106E8
	.include "animations/gSpriteAnimations_Npc26_0.s"

gSpriteAnimations_Npc26_4:: @ 081106ED
	.include "animations/gSpriteAnimations_Npc26_4.s"

gSpriteAnimations_Npc26_6:: @ 081106FE
	.include "animations/gSpriteAnimations_Npc26_6.s"

gSpriteAnimations_Npc26:: @ 08110710
	ptr gSpriteAnimations_Npc26_0
	ptr gSpriteAnimations_Npc26_0
	ptr gSpriteAnimations_Npc26_0
	ptr gSpriteAnimations_Npc26_0
	ptr gSpriteAnimations_Npc26_4
	ptr gSpriteAnimations_Npc26_4
	ptr gSpriteAnimations_Npc26_6
	ptr gSpriteAnimations_Npc26_6
	ptr 00000000
