	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
	.align 2

gSpritePtrs:: @ 080029B4
	ptr 00000000
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_1
	ptr gSpriteFrames_1
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_1
	ptr gSpriteFrames_2
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_1
	ptr gSpriteFrames_3
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_4
	ptr gSpriteFrames_4
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_5
	ptr gSpriteFrames_5
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_6
	ptr gSpriteFrames_6
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_7
	ptr gSpriteFrames_7
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_1
	ptr gSpriteFrames_8
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_4
	ptr gSpriteFrames_9
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MinishEmoticon
	ptr gSpriteFrames_MinishEmoticon
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_EzloCap
	ptr gSpriteFrames_EzloCap
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_12
	ptr gSpriteFrames_12
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_13
	ptr gSprite_Link
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_14
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_15
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_16
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_17
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_18
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_19
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_20
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_21
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_22
	ptr gSprite_081C8C74
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ItemGustJar
	ptr gSpriteFrames_23
	ptr gSprite_081D7674
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Gentari
	ptr gSpriteFrames_24
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Festari
	ptr gSpriteFrames_25
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Melari
	ptr gSpriteFrames_26
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MountainMinish
	ptr gSpriteFrames_27
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_28
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_TownMinish
	ptr gSpriteFrames_29
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Librari
	ptr gSpriteFrames_30
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ForestMinish
	ptr gSpriteFrames_31
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Vaati
	ptr gSpriteFrames_32
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_9
	ptr gSpriteFrames_33
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_0
	ptr gSpriteFrames_CutsceneMiscObject_0
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_1
	ptr gSpriteFrames_CutsceneMiscObject_1
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_2
	ptr gSpriteFrames_CutsceneMiscObject_2
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MinishEzlo
	ptr gSpriteFrames_MinishEzlo
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_3
	ptr gSpriteFrames_CutsceneMiscObject_3
	ptr gSprite_081DF834
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Din
	ptr gSpriteFrames_Din
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Nayru
	ptr gSpriteFrames_Nayru
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Farore
	ptr gSpriteFrames_Farore
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Npc5
	ptr gSpriteFrames_Npc5
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GuardWithSpear
	ptr gSpriteFrames_GuardWithSpear
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_KingDaltus
	ptr gSpriteFrames_KingDaltus
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MinisterPotho
	ptr gSpriteFrames_MinisterPotho
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Npc26
	ptr gSpriteFrames_Npc26
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Maid
	ptr gSpriteFrames_Maid
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_TingleSiblings
	ptr gSpriteFrames_TingleSiblings
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Beedle
	ptr gSpriteFrames_Beedle
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Brocco
	ptr gSpriteFrames_Brocco
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Pina
	ptr gSpriteFrames_Pina
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Postman
	ptr gSpriteFrames_Postman
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Malon
	ptr gSpriteFrames_Malon
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Epona
	ptr gSpriteFrames_Epona
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MilkCart
	ptr gSpriteFrames_MilkCart
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Smith
	ptr gSpriteFrames_Smith
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Townsperson
	ptr gSpriteFrames_Townsperson
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_58
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Kid
	ptr gSpriteFrames_Kid
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Mailbox
	ptr gSpriteFrames_Mailbox
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SittingPerson
	ptr gSpriteFrames_SittingPerson
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Mutoh
	ptr gSpriteFrames_Mutoh
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Carpenter
	ptr gSpriteFrames_Carpenter
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Gorman
	ptr gSpriteFrames_Gorman
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Cat
	ptr gSpriteFrames_Cat
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Cat
	ptr gSpriteFrames_66
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Dog
	ptr gSpriteFrames_Dog
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Dog
	ptr gSpriteFrames_68
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Dog
	ptr gSpriteFrames_69
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Syrup
	ptr gSpriteFrames_Syrup
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Percy
	ptr gSpriteFrames_Percy
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Farmers_0
	ptr gSpriteFrames_Farmers_0
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Farmers_1
	ptr gSpriteFrames_Farmers_1
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Dampe
	ptr gSpriteFrames_Dampe
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_KingGustaf
	ptr gSpriteFrames_KingGustaf
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Gina
	ptr gSpriteFrames_Gina
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Simon
	ptr gSpriteFrames_Simon
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Mama
	ptr gSpriteFrames_Mama
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Teachers
	ptr gSpriteFrames_Teachers
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Teachers
	ptr gSpriteFrames_80
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WindTribespeople_0
	ptr gSpriteFrames_WindTribespeople_0
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WindTribespeople_1
	ptr gSpriteFrames_WindTribespeople_1
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WindTribespeople_2
	ptr gSpriteFrames_WindTribespeople_2
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WindTribespeople_3
	ptr gSpriteFrames_WindTribespeople_3
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WindTribespeople_4
	ptr gSpriteFrames_WindTribespeople_4
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WindTribespeople_5
	ptr gSpriteFrames_WindTribespeople_5
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MayorHagen
	ptr gSpriteFrames_MayorHagen
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GhostBrothers
	ptr gSpriteFrames_GhostBrothers
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_HurdyGurdyMan
	ptr gSpriteFrames_HurdyGurdyMan
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Cow
	ptr gSpriteFrames_Cow
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GoronMerchant
	ptr gSpriteFrames_GoronMerchant
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GoronMerchant
	ptr gSpriteFrames_92
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Goron
	ptr gSpriteFrames_Goron
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CastorWildsStatue
	ptr gSpriteFrames_CastorWildsStatue
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Npc4F
	ptr gSpriteFrames_Npc4F
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_4
	ptr gSpriteFrames_CutsceneMiscObject_4
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr gSpriteFrames_97
	ptr gSprite_081FF1B4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DarkNut_0
	ptr gSpriteFrames_DarkNut_0
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DarkNut_1
	ptr gSpriteFrames_DarkNut_1
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ChuchuBoss_0
	ptr gSpriteFrames_ChuchuBoss_0
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DeathFx_0
	ptr gSpriteFrames_DeathFx_0
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Madderpillar_0
	ptr gSpriteFrames_Madderpillar_0
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Gleerok_0
	ptr gSpriteFrames_Gleerok_0
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Keaton
	ptr gSpriteFrames_Keaton
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MazaalHand
	ptr gSpriteFrames_MazaalHand
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ScissorsBeetle
	ptr gSpriteFrames_ScissorsBeetle
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MandiblesProjectile
	ptr gSpriteFrames_MandiblesProjectile
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Stalfos
	ptr gSpriteFrames_Stalfos
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Gibdo
	ptr gSpriteFrames_Gibdo
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Lakitu
	ptr gSpriteFrames_Lakitu
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_LakituCloudProjectile
	ptr gSpriteFrames_LakituCloudProjectile
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiRebornEnemy_0
	ptr gSpriteFrames_VaatiRebornEnemy_0
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiRebornEnemy_1
	ptr gSpriteFrames_VaatiRebornEnemy_1
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiRebornEnemy_2
	ptr gSpriteFrames_VaatiRebornEnemy_2
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiRebornEnemy_3
	ptr gSpriteFrames_VaatiRebornEnemy_3
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiBall
	ptr gSpriteFrames_VaatiBall
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiTransfigured_0
	ptr gSpriteFrames_VaatiTransfigured_0
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiTransfigured_1
	ptr gSpriteFrames_VaatiTransfigured_1
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiTransfigured_2
	ptr gSpriteFrames_VaatiTransfigured_2
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiTransfigured_3
	ptr gSpriteFrames_VaatiTransfigured_3
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiArm
	ptr gSpriteFrames_VaatiArm
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiWrath
	ptr gSpriteFrames_VaatiWrath
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiReborn
	ptr gSpriteFrames_VaatiReborn
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MoblinLady_1
	ptr gSpriteFrames_MoblinLady_1
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BallChainSoldier
	ptr gSpriteFrames_BallChainSoldier
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GyorgMale
	ptr gSpriteFrames_GyorgMale
	ptr gSprite_082474D4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_JarPortal
	ptr gSpriteFrames_JarPortal
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Beanstalk_0
	ptr gSpriteFrames_Beanstalk_0
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Minecart
	ptr gSpriteFrames_Minecart
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_LavaPlatform_1
	ptr gSpriteFrames_LavaPlatform_1
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Whirlwind
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BigVortex
	ptr gSpriteFrames_BigVortex
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MacroBook
	ptr gSpriteFrames_MacroBook
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GentariCurtains
	ptr gSpriteFrames_GentariCurtains
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpiderWeb
	ptr gSpriteFrames_SpiderWeb
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ShrinkingHieroglyphs
	ptr gSpriteFrames_ShrinkingHieroglyphs
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_OctorokBoss_0
	ptr gSpriteFrames_OctorokBoss_0
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Bell
	ptr gSpriteFrames_Bell
	ptr gSprite_082D4894
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Talon
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Sturgeon
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Librarians_0
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Librarians_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Stamp
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Marcy
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Stockwell
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Wheaton
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Pita
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CuccoAggr
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BladeBrothers_0
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BladeBrothers_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Rem
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Carlov_0
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Carlov_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DrLeft
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Anju
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Emma
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Gregal
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr 00000000
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CastorWildsStatue
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Chuchu
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpecialFx
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpecialFx
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Thunderbolt
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Beetle
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_166
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Pot
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DeathFx_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Peahat
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Railtrack
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Rollobite
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_UnusedSkull
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Flame
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Octorok
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SwordParticle
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_RockProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Keese
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DoorMimic
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DoorMimic
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_RockChuchu
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpinyChuchu
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PressurePlate
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MoleMittsParticle
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ChestSpawner
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BigBarrel
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PushableStatue
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Moldworm
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Sluggula
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Sluggula
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Sluggula
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Pesto
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Puffstool
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PushableRock
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Beanstalk_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_KinstoneSpark
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PullableMushroom
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Bollard
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WarpPoint
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_AmbientClouds
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ChuchuBoss_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ChuchuBoss_2
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CameraTarget
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BossDoor
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_LampParticle
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_LikeLike
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpearMoblin
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BowMoblin
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BusinessScrub
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_RupeeLike
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MinishSizedEntrance
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PullableLever
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ThoughtBubble
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Madderpillar_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_LavaPlatform
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WaterDrop
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WallMaster
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_DirtBallProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CrenelBeanSprout
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BombPeahat
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Spark
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Chaser
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpikedBeetle
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Helmasaur
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_FallingBoulder
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Bobomb
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Gleerok_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GreatFairy
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Tektite
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WizzrobeWind
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WizzrobeFire
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WizzrobeIce
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WindProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_FireProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_IceProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Armos
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Leever
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Rope
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Eyegore
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Pesto
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_AcroBandits
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Crow
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Mulldozer
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PushableLever
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BigPushableLever
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Bombarossa
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Wisp
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpinyBeetle
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MazaalHead
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MazaalBracelet
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MazaalEnergyBeam
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MazaalEnergyBeam
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MazaalMacro
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_EyeSwitch
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_OctorokBoss_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PicoBloom
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CarlovObject
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BigIceBlock
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BigIceBlock
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SmallIceBlock
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Trapdoor
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_EvilSpirit
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CloudPiranha
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_FlyingSkull
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BoneProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BakerOven
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Lamp_0
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_LakituLightning
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_5
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Bird_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Bird
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_TorchTrapProjectile_0
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_TorchTrapProjectile_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GuruguruBar
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Lamp_1
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_FigurineDevice
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_PullableLever
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_RotatingTrapdoor
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Ghini
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiArm
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiWrath
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Vaati3Arm
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_WrathEye
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_V3HandProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiWrath
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiEyesMacro
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_JapaneseSubtitle
.ifndef EU
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_JapaneseSubtitle
.endif
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Fan
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_AngryStatue
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CannonballProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_V1EyeLaser
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_V1DarkMagicProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Slime
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Curtain
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Vaati2Particle
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Enemy5A
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiTransfigured_4
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_VaatiTransfigured_5
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SanctuaryStoneTablet
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_FireballGuy
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpikedRollers
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_6
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_MoblinLady
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Vaati1Portal
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GyorgMaleEnergyProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ClothesRack
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_7
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_JailBars
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_FanWind
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GyorgMaleEye
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GyorgFemaleEye
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GyorgChild
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SmallTownMinish
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_GyorgFemaleMouth
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_V2Projectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_8
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Pinwheel
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_SpearMoblin
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_BowMoblin
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ArrowProjectile
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

gMoreSpritePtrs:: @ 08003DD4
	ptr gSpriteAnimations_322
	ptr gSpriteFrames_322
	ptr gSprite_082E68F4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_322
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_322
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_FileScreenObjects
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_ObjectA2
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_CutsceneMiscObject_10
	ptr 00000000
	ptr 00000000
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	ptr gSpriteAnimations_Vaati_1
	ptr gSpriteFrames_Vaati_1
	ptr gSprite_082F27F4
	.4byte 00000000
	.ifdef PLATFORM_PC64
	.4byte 0 @ SpritePtr is 32 bytes
	.endif

	.ifdef PLATFORM_PC64
	@ 329 entries; the game indexes gSpritePtrs across gMoreSpritePtrs.
	.if . - gSpritePtrs != 329 * 32
	.error "gSpritePtrs entries must be 32 bytes in the 64-bit build"
	.endif
	.endif

gUnk_08003E44:: @ 08003E44
	.incbin "data_080029B4/gUnk_08003E44.bin"
