	.include "asm/macros.inc"
	.include "constants/constants.inc"

	.section .rodata
    .align 2

@ staffroll rodata

@ staffroll.c
gUnk_081272F0:: @ 081272F0
	.incbin "data_08127280/gUnk_081272F0.bin"

gUnk_08127644:: @ 08127644
	.incbin "data_08127280/gUnk_08127644.bin"

gUnk_08127998:: @ 08127998
	.incbin "data_08127280/gUnk_08127998.bin"

gUnk_08127CEC:: @ 08127CEC
	.incbin "data_08127280/gUnk_08127CEC.bin"

gUnk_08127D00:: @ 08127D00
	ptr StaffrollTask_State0
	ptr StaffrollTask_State1
	ptr StaffrollTask_State2
	ptr StaffrollTask_State3

gUnk_08127D10:: @ 08127D10
	ptr StaffrollTask_State1MenuType0
	ptr StaffrollTask_State1MenuType1
	ptr StaffrollTask_State1MenuType2
	ptr StaffrollTask_State1MenuType3
	ptr StaffrollTask_State1MenuType4
	ptr StaffrollTask_State1MenuType5
	ptr StaffrollTask_State1MenuType6
	ptr StaffrollTask_State1MenuType7

@ End of staffroll
