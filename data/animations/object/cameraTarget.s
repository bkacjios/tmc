	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

gSpriteAnimations_CameraTarget_0:: @ 0811F760
	.include "animations/gSpriteAnimations_CameraTarget_0.s"

gSpriteAnimations_CameraTarget_1:: @ 0811F795
	.include "animations/gSpriteAnimations_CameraTarget_1.s"

gSpriteAnimations_CameraTarget:: @ 0811F7CC
	ptr gSpriteAnimations_CameraTarget_0
	ptr gSpriteAnimations_CameraTarget_1
	ptr 00000000
