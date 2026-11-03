	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

@ subtask2.c
gUnk_08128C00:: @ 08128C00
	.incbin "data_08127280/gUnk_08128C00.bin"

gUnk_08128C04:: @ 08128C04
	.incbin "data_08127280/gUnk_08128C04.bin"

gUnk_08128C14:: @ 08128C14
	.incbin "data_08127280/gUnk_08128C14.bin"

gUnk_08128C94:: @ 08128C94
	.incbin "data_08127280/gUnk_08128C94.bin"

gUnk_08128D14:: @ 08128D14
	ptr sub_080A59AC
	ptr sub_080A59C8
	ptr sub_080A5A54
	ptr sub_080A5A90

gUnk_08128D24:: @ 08128D24
	ptr sub_080A5AF4
	ptr sub_080A5B34
	ptr sub_080A5BB8

gUnk_08128D30:: @ 08128D30
	ptr sub_080A5C44
	ptr sub_080A5C9C

gUnk_08128D38:: @ 08128D38
	.incbin "data_08127280/gUnk_08128D38.bin"

gUnk_08128D3C:: @ 08128D3C
	.incbin "data_08127280/gUnk_08128D3C.bin"

gUnk_08128D43:: @ 08128D43
	.incbin "data_08127280/gUnk_08128D43.bin"

gUnk_08128D51:: @ 08128D51
	.incbin "data_08127280/gUnk_08128D51.bin"

gUnk_08128D58:: @ 08128D58
	ptr sub_080A6024
	ptr sub_080A6044

gUnk_08128D60:: @ 08128D60
	.incbin "data_08127280/gUnk_08128D60.bin"

gUnk_08128D70:: @ 08128D70
	.incbin "data_08127280/gUnk_08128D70.bin"

gUnk_08128DB0:: @ 08128DB0
	ptr sub_080A6108
	ptr sub_080A612C

gUnk_08128DB8:: @ 08128DB8
	.incbin "data_08127280/gUnk_08128DB8.bin"

gUnk_08128DBC:: @ 08128DBC
	.incbin "data_08127280/gUnk_08128DBC.bin"

gUnk_08128DCC:: @ 08128DCC
	ptr sub_080A6290
	ptr sub_080A62E0

gUnk_08128DD4:: @ 08128DD4
	.incbin "data_08127280/gUnk_08128DD4.bin"

gUnk_08128DD8:: @ 08128DD8
	.incbin "data_08127280/gUnk_08128DD8.bin"

gUnk_08128DE8:: @ 08128DE8
	.incbin "data_08127280/gUnk_08128DE8.bin"

Subtask_MapHint_Functions:: @ 08128E70
	ptr Subtask_MapHint_0
	ptr Subtask_MapHint_1

gUnk_08128E78:: @ 08128E78
	ptr sub_080A6650
	ptr sub_080A667C

gUnk_08128E80:: @ 08128E80
	.incbin "data_08127280/gUnk_08128E80.bin"

gUnk_08128E84:: @ 08128E84
	.incbin "data_08127280/gUnk_08128E84.bin"

gUnk_08128E94:: @ 08128E94
	.incbin "data_08127280/gUnk_08128E94.bin"

gUnk_08128F1C:: @ 08128F1C
	ptr sub_080A6B04
	ptr sub_080A6C1C

Subtask_FastTravel_Functions:: @ 08128F24
	ptr Subtask_FastTravel_0
	ptr Subtask_FastTravel_1
	ptr Subtask_FastTravel_2
	ptr Subtask_FastTravel_3
	ptr Subtask_FastTravel_4

gUnk_08128F38:: @ 08128F38
	.incbin "data_08127280/gUnk_08128F38.bin"

gUnk_08128F4C:: @ 08128F4C
	.incbin "data_08127280/gUnk_08128F4C.bin"

gUnk_08128F58:: @ 08128F58
	.incbin "data_08127280/gUnk_08128F58.bin"

gUnk_08128FA8:: @ 08128FA8
	.incbin "data_08127280/gUnk_08128FA8.bin"

gUnk_08128FC0:: @ 08128FC0
	.incbin "data_08127280/gUnk_08128FC0.bin"

gUnk_08128FD8:: @ 08128FD8
	.incbin "data_08127280/gUnk_08128FD8.bin"

gDungeonNames:: @ 08128FF0
	.incbin "data_08127280/gDungeonNames.bin"

gUnk_08129004:: @ 08129004
	.incbin "data_08127280/gUnk_08129004.bin"

gUnk_0812901C:: @ 0812901C
	ptr Subtask_FadeIn
	ptr Subtask_Init
	ptr Subtask_Update
	ptr Subtask_FadeOut
	ptr Subtask_Die

@ subtask2.c
gSubtasks:: @ 08129030
	ptr Subtask_Exit
	ptr Subtask_PauseMenu
	ptr Subtask_Exit
	ptr Subtask_MapHint
	ptr Subtask_KinstoneMenu
	ptr Subtask_AuxCutscene
	ptr Subtask_PortalCutscene
	ptr Subtask_FigurineMenu
	ptr Subtask_WorldEvent
	ptr Subtask_FastTravel
	ptr Subtask_LocalMapHint
