#pragma once


#include <vector>
using namespace std;

#include "Main.h"
#include "Key.h"
#include "MyDirectXSystem.h"
#include "MyDirectXDraw.h"

#define INSTRUCTION_NUM 256

#define CPU_FREQ 4194304//CPUの周波数(hz)

class GBX
{
private:

	bool FATAL_ERROR_FLAG = false;//ロードなどで続行不能なエラーが発生したか



	Key* key;


	bool tmp_CPU_HALT_Flag = false;


	uint64_t hblank_process_stage_counter = 0;
	uint64_t vblank_process_stage_counter = 0;


#define GB_PALETTE_0 0xFF9bbc0f
#define GB_PALETTE_1 0xFF8bac0f
#define GB_PALETTE_2 0xFF306230
#define GB_PALETTE_3 0xFF0f380f
	const DWORD GB_PALETTE_LIST[4] = {
			GB_PALETTE_0,
			GB_PALETTE_1,
			GB_PALETTE_2,
			GB_PALETTE_3,
	};

	DWORD get_bg_window_palette(uint8_t _2bit_color_no) {
		uint8_t color_data;

		if (_2bit_color_no == 0) {
			color_data = (gbx_ram.RAM[0xFF47] & 0b00000011);
		}
		else if (_2bit_color_no == 1) {
			color_data = ((gbx_ram.RAM[0xFF47] >> 2) & 0b00000011);
		}
		else if (_2bit_color_no == 2) {
			color_data = ((gbx_ram.RAM[0xFF47] >> 4) & 0b00000011);
		}
		else {
			color_data = ((gbx_ram.RAM[0xFF47] >> 6) & 0b00000011);
		}

		return GB_PALETTE_LIST[color_data];
	}

	DWORD get_sprite_palette(uint8_t _2bit_color_no, bool palette_OBP1_flag) {
		uint8_t valid_palette_data;
		if (palette_OBP1_flag == false) {
			valid_palette_data = gbx_ram.RAM[0xFF48];
		}
		else {
			valid_palette_data = gbx_ram.RAM[0xFF49];
		}

		uint8_t color_data;

		if (_2bit_color_no == 0) {
			//color_data = (valid_palette_data & 0b00000011);

			return 0x00000000;//色番号が下位2bitのときは透明を返す
		}
		else if (_2bit_color_no == 1) {
			color_data = ((valid_palette_data >> 2) & 0b00000011);
		}
		else if (_2bit_color_no == 2) {
			color_data = ((valid_palette_data >> 4) & 0b00000011);
		}
		else {
			color_data = ((valid_palette_data >> 6) & 0b00000011);
		}

		return GB_PALETTE_LIST[color_data];
	}


	uint8_t _8bit_bg_screen_data_160x144[GBX_WIDTH * GBX_HEIGHT];
	uint8_t _8bit_window_screen_data_160x144[GBX_WIDTH * GBX_HEIGHT];

	uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype0[256 * 256];
	uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype1[256 * 256];
	uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype0[256 * 256];
	uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype1[256 * 256];
	
	uint8_t _8bit_sprite_screen_data_160x144[GBX_WIDTH * GBX_HEIGHT];


	void (GBX::*cpu_instruction_table[INSTRUCTION_NUM])() = {
			&GBX::cpu_fnc__NOP, //0x00
			&GBX::cpu_fnc__LD_BC_n16, //0x01
			&GBX::cpu_fnc__LD_addrBC_A, //0x02
			&GBX::cpu_fnc__INC_BC, //0x03
			&GBX::cpu_fnc__INC_B, //0x04
			&GBX::cpu_fnc__DEC_B, //0x05
			&GBX::cpu_fnc__LD_B_n8, //0x06
			&GBX::cpu_fnc__RLCA, //0x07
			&GBX::cpu_fnc__LD_addrn16_SP, //0x08
			&GBX::cpu_fnc__ADD_HL_BC, //0x09
			&GBX::cpu_fnc__LD_A_addrBC, //0x0a
			&GBX::cpu_fnc__DEC_BC, //0x0b
			&GBX::cpu_fnc__INC_C, //0x0c
			&GBX::cpu_fnc__DEC_C, //0x0d
			&GBX::cpu_fnc__LD_C_n8, //0x0e
			&GBX::cpu_fnc__RRCA, //0x0f
			&GBX::cpu_fnc__STOP, //0x10
			&GBX::cpu_fnc__LD_DE_n16, //0x11
			&GBX::cpu_fnc__LD_addrDE_A, //0x12
			&GBX::cpu_fnc__INC_DE, //0x13
			&GBX::cpu_fnc__INC_D, //0x14
			&GBX::cpu_fnc__DEC_D, //0x15
			&GBX::cpu_fnc__LD_D_n8, //0x16
			&GBX::cpu_fnc__RLA, //0x17
			&GBX::cpu_fnc__JR_e8, //0x18
			&GBX::cpu_fnc__ADD_HL_DE, //0x19
			&GBX::cpu_fnc__LD_A_addrDE, //0x1a
			&GBX::cpu_fnc__DEC_DE, //0x1b
			&GBX::cpu_fnc__INC_E, //0x1c
			&GBX::cpu_fnc__DEC_E, //0x1d
			&GBX::cpu_fnc__LD_E_n8, //0x1e
			&GBX::cpu_fnc__RRA, //0x1f
			&GBX::cpu_fnc__JR_FNZ_e8, //0x20
			&GBX::cpu_fnc__LD_HL_n16, //0x21
			&GBX::cpu_fnc__LD_addrHLI_A, //0x22
			&GBX::cpu_fnc__INC_HL, //0x23
			&GBX::cpu_fnc__INC_H, //0x24
			&GBX::cpu_fnc__DEC_H, //0x25
			&GBX::cpu_fnc__LD_H_n8, //0x26
			&GBX::cpu_fnc__DAA, //0x27
			&GBX::cpu_fnc__JR_FZ_e8, //0x28
			&GBX::cpu_fnc__ADD_HL_HL, //0x29
			&GBX::cpu_fnc__LD_A_addrHLI, //0x2a
			&GBX::cpu_fnc__DEC_HL, //0x2b
			&GBX::cpu_fnc__INC_L, //0x2c
			&GBX::cpu_fnc__DEC_L, //0x2d
			&GBX::cpu_fnc__LD_L_n8, //0x2e
			&GBX::cpu_fnc__CPL, //0x2f
			&GBX::cpu_fnc__JR_FNC_e8, //0x30
			&GBX::cpu_fnc__LD_SP_n16, //0x31
			&GBX::cpu_fnc__LD_addrHLD_A, //0x32
			&GBX::cpu_fnc__INC_SP, //0x33
			&GBX::cpu_fnc__INC_addrHL, //0x34
			&GBX::cpu_fnc__DEC_addrHL, //0x35
			&GBX::cpu_fnc__LD_addrHL_n8, //0x36
			&GBX::cpu_fnc__SCF, //0x37
			&GBX::cpu_fnc__JR_FC_e8, //0x38
			&GBX::cpu_fnc__ADD_HL_SP, //0x39
			&GBX::cpu_fnc__LD_A_addrHLD, //0x3a
			&GBX::cpu_fnc__DEC_SP, //0x3b
			&GBX::cpu_fnc__INC_A, //0x3c
			&GBX::cpu_fnc__DEC_A, //0x3d
			&GBX::cpu_fnc__LD_A_n8, //0x3e
			&GBX::cpu_fnc__CCF, //0x3f
			&GBX::cpu_fnc__LD_B_B, //0x40
			&GBX::cpu_fnc__LD_B_C, //0x41
			&GBX::cpu_fnc__LD_B_D, //0x42
			&GBX::cpu_fnc__LD_B_E, //0x43
			&GBX::cpu_fnc__LD_B_H, //0x44
			&GBX::cpu_fnc__LD_B_L, //0x45
			&GBX::cpu_fnc__LD_B_addrHL, //0x46
			&GBX::cpu_fnc__LD_B_A, //0x47
			&GBX::cpu_fnc__LD_C_B, //0x48
			&GBX::cpu_fnc__LD_C_C, //0x49
			&GBX::cpu_fnc__LD_C_D, //0x4a
			&GBX::cpu_fnc__LD_C_E, //0x4b
			&GBX::cpu_fnc__LD_C_H, //0x4c
			&GBX::cpu_fnc__LD_C_L, //0x4d
			&GBX::cpu_fnc__LD_C_addrHL, //0x4e
			&GBX::cpu_fnc__LD_C_A, //0x4f
			&GBX::cpu_fnc__LD_D_B, //0x50
			&GBX::cpu_fnc__LD_D_C, //0x51
			&GBX::cpu_fnc__LD_D_D, //0x52
			&GBX::cpu_fnc__LD_D_E, //0x53
			&GBX::cpu_fnc__LD_D_H, //0x54
			&GBX::cpu_fnc__LD_D_L, //0x55
			&GBX::cpu_fnc__LD_D_addrHL, //0x56
			&GBX::cpu_fnc__LD_D_A, //0x57
			&GBX::cpu_fnc__LD_E_B, //0x58
			&GBX::cpu_fnc__LD_E_C, //0x59
			&GBX::cpu_fnc__LD_E_D, //0x5a
			&GBX::cpu_fnc__LD_E_E, //0x5b
			&GBX::cpu_fnc__LD_E_H, //0x5c
			&GBX::cpu_fnc__LD_E_L, //0x5d
			&GBX::cpu_fnc__LD_E_addrHL, //0x5e
			&GBX::cpu_fnc__LD_E_A, //0x5f
			&GBX::cpu_fnc__LD_H_B, //0x60
			&GBX::cpu_fnc__LD_H_C, //0x61
			&GBX::cpu_fnc__LD_H_D, //0x62
			&GBX::cpu_fnc__LD_H_E, //0x63
			&GBX::cpu_fnc__LD_H_H, //0x64
			&GBX::cpu_fnc__LD_H_L, //0x65
			&GBX::cpu_fnc__LD_H_addrHL, //0x66
			&GBX::cpu_fnc__LD_H_A, //0x67
			&GBX::cpu_fnc__LD_L_B, //0x68
			&GBX::cpu_fnc__LD_L_C, //0x69
			&GBX::cpu_fnc__LD_L_D, //0x6a
			&GBX::cpu_fnc__LD_L_E, //0x6b
			&GBX::cpu_fnc__LD_L_H, //0x6c
			&GBX::cpu_fnc__LD_L_L, //0x6d
			&GBX::cpu_fnc__LD_L_addrHL, //0x6e
			&GBX::cpu_fnc__LD_L_A, //0x6f
			&GBX::cpu_fnc__LD_addrHL_B, //0x70
			&GBX::cpu_fnc__LD_addrHL_C, //0x71
			&GBX::cpu_fnc__LD_addrHL_D, //0x72
			&GBX::cpu_fnc__LD_addrHL_E, //0x73
			&GBX::cpu_fnc__LD_addrHL_H, //0x74
			&GBX::cpu_fnc__LD_addrHL_L, //0x75
			&GBX::cpu_fnc__HALT, //0x76
			&GBX::cpu_fnc__LD_addrHL_A, //0x77
			&GBX::cpu_fnc__LD_A_B, //0x78
			&GBX::cpu_fnc__LD_A_C, //0x79
			&GBX::cpu_fnc__LD_A_D, //0x7a
			&GBX::cpu_fnc__LD_A_E, //0x7b
			&GBX::cpu_fnc__LD_A_H, //0x7c
			&GBX::cpu_fnc__LD_A_L, //0x7d
			&GBX::cpu_fnc__LD_A_addrHL, //0x7e
			&GBX::cpu_fnc__LD_A_A, //0x7f
			&GBX::cpu_fnc__ADD_A_B, //0x80
			&GBX::cpu_fnc__ADD_A_C, //0x81
			&GBX::cpu_fnc__ADD_A_D, //0x82
			&GBX::cpu_fnc__ADD_A_E, //0x83
			&GBX::cpu_fnc__ADD_A_H, //0x84
			&GBX::cpu_fnc__ADD_A_L, //0x85
			&GBX::cpu_fnc__ADD_A_addrHL, //0x86
			&GBX::cpu_fnc__ADD_A_A, //0x87
			&GBX::cpu_fnc__ADC_A_B, //0x88
			&GBX::cpu_fnc__ADC_A_C, //0x89
			&GBX::cpu_fnc__ADC_A_D, //0x8a
			&GBX::cpu_fnc__ADC_A_E, //0x8b
			&GBX::cpu_fnc__ADC_A_H, //0x8c
			&GBX::cpu_fnc__ADC_A_L, //0x8d
			&GBX::cpu_fnc__ADC_A_addrHL, //0x8e
			&GBX::cpu_fnc__ADC_A_A, //0x8f
			&GBX::cpu_fnc__SUB_A_B, //0x90
			&GBX::cpu_fnc__SUB_A_C, //0x91
			&GBX::cpu_fnc__SUB_A_D, //0x92
			&GBX::cpu_fnc__SUB_A_E, //0x93
			&GBX::cpu_fnc__SUB_A_H, //0x94
			&GBX::cpu_fnc__SUB_A_L, //0x95
			&GBX::cpu_fnc__SUB_A_addrHL, //0x96
			&GBX::cpu_fnc__SUB_A_A, //0x97
			&GBX::cpu_fnc__SBC_A_B, //0x98
			&GBX::cpu_fnc__SBC_A_C, //0x99
			&GBX::cpu_fnc__SBC_A_D, //0x9a
			&GBX::cpu_fnc__SBC_A_E, //0x9b
			&GBX::cpu_fnc__SBC_A_H, //0x9c
			&GBX::cpu_fnc__SBC_A_L, //0x9d
			&GBX::cpu_fnc__SBC_A_addrHL, //0x9e
			&GBX::cpu_fnc__SBC_A_A, //0x9f
			&GBX::cpu_fnc__AND_A_B, //0xa0
			&GBX::cpu_fnc__AND_A_C, //0xa1
			&GBX::cpu_fnc__AND_A_D, //0xa2
			&GBX::cpu_fnc__AND_A_E, //0xa3
			&GBX::cpu_fnc__AND_A_H, //0xa4
			&GBX::cpu_fnc__AND_A_L, //0xa5
			&GBX::cpu_fnc__AND_A_addrHL, //0xa6
			&GBX::cpu_fnc__AND_A_A, //0xa7
			&GBX::cpu_fnc__XOR_A_B, //0xa8
			&GBX::cpu_fnc__XOR_A_C, //0xa9
			&GBX::cpu_fnc__XOR_A_D, //0xaa
			&GBX::cpu_fnc__XOR_A_E, //0xab
			&GBX::cpu_fnc__XOR_A_H, //0xac
			&GBX::cpu_fnc__XOR_A_L, //0xad
			&GBX::cpu_fnc__XOR_A_addrHL, //0xae
			&GBX::cpu_fnc__XOR_A_A, //0xaf
			&GBX::cpu_fnc__OR_A_B, //0xb0
			&GBX::cpu_fnc__OR_A_C, //0xb1
			&GBX::cpu_fnc__OR_A_D, //0xb2
			&GBX::cpu_fnc__OR_A_E, //0xb3
			&GBX::cpu_fnc__OR_A_H, //0xb4
			&GBX::cpu_fnc__OR_A_L, //0xb5
			&GBX::cpu_fnc__OR_A_addrHL, //0xb6
			&GBX::cpu_fnc__OR_A_A, //0xb7
			&GBX::cpu_fnc__CP_A_B, //0xb8
			&GBX::cpu_fnc__CP_A_C, //0xb9
			&GBX::cpu_fnc__CP_A_D, //0xba
			&GBX::cpu_fnc__CP_A_E, //0xbb
			&GBX::cpu_fnc__CP_A_H, //0xbc
			&GBX::cpu_fnc__CP_A_L, //0xbd
			&GBX::cpu_fnc__CP_A_addrHL, //0xbe
			&GBX::cpu_fnc__CP_A_A, //0xbf
			&GBX::cpu_fnc__RET_FNZ, //0xc0
			&GBX::cpu_fnc__POP_BC, //0xc1
			&GBX::cpu_fnc__JP_FNZ_n16, //0xc2
			&GBX::cpu_fnc__JP_n16, //0xc3
			&GBX::cpu_fnc__CALL_FNZ_n16, //0xc4
			&GBX::cpu_fnc__PUSH_BC, //0xc5
			&GBX::cpu_fnc__ADD_A_n8, //0xc6
			&GBX::cpu_fnc__RST_00h, //0xc7
			&GBX::cpu_fnc__RET_FZ, //0xc8
			&GBX::cpu_fnc__RET, //0xc9
			&GBX::cpu_fnc__JP_FZ_n16, //0xca
			&GBX::cpu_fnc__PREFIX, //0xcb
			&GBX::cpu_fnc__CALL_FZ_n16, //0xcc
			&GBX::cpu_fnc__CALL_n16, //0xcd
			&GBX::cpu_fnc__ADC_A_n8, //0xce
			&GBX::cpu_fnc__RST_08h, //0xcf
			&GBX::cpu_fnc__RET_FNC, //0xd0
			&GBX::cpu_fnc__POP_DE, //0xd1
			&GBX::cpu_fnc__JP_FNC_n16, //0xd2
			&GBX::cpu_fnc__GARBAGE, //0xd3
			&GBX::cpu_fnc__CALL_FNC_n16, //0xd4
			&GBX::cpu_fnc__PUSH_DE, //0xd5
			&GBX::cpu_fnc__SUB_A_n8, //0xd6
			&GBX::cpu_fnc__RST_10h, //0xd7
			&GBX::cpu_fnc__RET_FC, //0xd8
			&GBX::cpu_fnc__RETI, //0xd9
			&GBX::cpu_fnc__JP_FC_n16, //0xda
			&GBX::cpu_fnc__GARBAGE, //0xdb
			&GBX::cpu_fnc__CALL_FC_n16, //0xdc
			&GBX::cpu_fnc__GARBAGE, //0xdd
			&GBX::cpu_fnc__SBC_A_n8, //0xde
			&GBX::cpu_fnc__RST_18h, //0xdf
			&GBX::cpu_fnc__LDH_addrn16_A, //0xe0
			&GBX::cpu_fnc__POP_HL, //0xe1
			&GBX::cpu_fnc__LDH_addrC_A, //0xe2
			&GBX::cpu_fnc__GARBAGE, //0xe3
			&GBX::cpu_fnc__GARBAGE, //0xe4
			&GBX::cpu_fnc__PUSH_HL, //0xe5
			&GBX::cpu_fnc__AND_A_n8, //0xe6
			&GBX::cpu_fnc__RST_20h, //0xe7
			&GBX::cpu_fnc__ADD_SP_e8, //0xe8
			&GBX::cpu_fnc__JP_HL, //0xe9
			&GBX::cpu_fnc__LD_addrn16_A, //0xea
			&GBX::cpu_fnc__GARBAGE, //0xeb
			&GBX::cpu_fnc__GARBAGE, //0xec
			&GBX::cpu_fnc__GARBAGE, //0xed
			&GBX::cpu_fnc__XOR_A_n8, //0xee
			&GBX::cpu_fnc__RST_28h, //0xef
			&GBX::cpu_fnc__LDH_A_addrn16, //0xf0
			&GBX::cpu_fnc__POP_AF, //0xf1
			&GBX::cpu_fnc__LDH_A_addrC, //0xf2
			&GBX::cpu_fnc__DI, //0xf3
			&GBX::cpu_fnc__GARBAGE, //0xf4
			&GBX::cpu_fnc__PUSH_AF, //0xf5
			&GBX::cpu_fnc__OR_A_n8, //0xf6
			&GBX::cpu_fnc__RST_30h, //0xf7
			&GBX::cpu_fnc__LD_HL_SPpe8, //0xf8
			&GBX::cpu_fnc__LD_SP_HL, //0xf9
			&GBX::cpu_fnc__LD_A_addrn16, //0xfa
			&GBX::cpu_fnc__EI, //0xfb
			&GBX::cpu_fnc__GARBAGE, //0xfc
			&GBX::cpu_fnc__GARBAGE, //0xfd
			&GBX::cpu_fnc__CP_A_n8, //0xfe
			&GBX::cpu_fnc__RST_38h, //0xff
	};

	bool get_timer_enable_flag() {
		return (((gbx_ram.RAM[0xFF07] & 0b00000100) != 0) ? true : false);
	}

	uint64_t get_timer_count_freq() {
		uint8_t div_type = (gbx_ram.RAM[0xFF07] & 0b00000011);
		if (div_type == 0b00) {
			return 1024;
		}
		else if (div_type == 0b01) {
			return 16;
		}
		else if (div_type == 0b10) {
			return 64;
		}
		else {//div_type == 0b11
			return  256;
		}
	}

	uint64_t cpu_machine_cycle = 0;
	uint64_t total_cpu_machine_cycle__div = 0;
	uint64_t total_cpu_machine_cycle__tima = 0;
	//マシンサイクル(クロックサイクルではない)
	uint32_t instruction_machine_cycle_table[INSTRUCTION_NUM] = {
		1, //0x00
		3, //0x01
		2, //0x02
		2, //0x03
		1, //0x04
		1, //0x05
		2, //0x06
		1, //0x07
		5, //0x08
		2, //0x09
		2, //0x0a
		2, //0x0b
		1, //0x0c
		1, //0x0d
		2, //0x0e
		1, //0x0f
		1, //0x10
		3, //0x11
		2, //0x12
		2, //0x13
		1, //0x14
		1, //0x15
		2, //0x16
		1, //0x17
		3, //0x18
		2, //0x19
		2, //0x1a
		2, //0x1b
		1, //0x1c
		1, //0x1d
		2, //0x1e
		1, //0x1f
		2, //0x20(分岐するとき+1)
		3, //0x21
		2, //0x22
		2, //0x23
		1, //0x24
		1, //0x25
		2, //0x26
		1, //0x27
		2, //0x28(分岐するとき+1)
		2, //0x29
		2, //0x2a
		2, //0x2b
		1, //0x2c
		1, //0x2d
		2, //0x2e
		1, //0x2f
		2, //0x30(分岐するとき+1)
		3, //0x31
		2, //0x32
		2, //0x33
		3, //0x34
		3, //0x35
		3, //0x36
		1, //0x37
		2, //0x38(分岐するとき+1)
		2, //0x39
		2, //0x3a
		2, //0x3b
		1, //0x3c
		1, //0x3d
		2, //0x3e
		1, //0x3f
		1, //0x40
		1, //0x41
		1, //0x42
		1, //0x43
		1, //0x44
		1, //0x45
		2, //0x46
		1, //0x47
		1, //0x48
		1, //0x49
		1, //0x4a
		1, //0x4b
		1, //0x4c
		1, //0x4d
		2, //0x4e
		1, //0x4f
		1, //0x50
		1, //0x51
		1, //0x52
		1, //0x53
		1, //0x54
		1, //0x55
		2, //0x56
		1, //0x57
		1, //0x58
		1, //0x59
		1, //0x5a
		1, //0x5b
		1, //0x5c
		1, //0x5d
		2, //0x5e
		1, //0x5f
		1, //0x60
		1, //0x61
		1, //0x62
		1, //0x63
		1, //0x64
		1, //0x65
		2, //0x66
		1, //0x67
		1, //0x68
		1, //0x69
		1, //0x6a
		1, //0x6b
		1, //0x6c
		1, //0x6d
		2, //0x6e
		1, //0x6f
		2, //0x70
		2, //0x71
		2, //0x72
		2, //0x73
		2, //0x74
		2, //0x75
		1, //0x76
		2, //0x77
		1, //0x78
		1, //0x79
		1, //0x7a
		1, //0x7b
		1, //0x7c
		1, //0x7d
		2, //0x7e
		1, //0x7f
		1, //0x80
		1, //0x81
		1, //0x82
		1, //0x83
		1, //0x84
		1, //0x85
		2, //0x86
		1, //0x87
		1, //0x88
		1, //0x89
		1, //0x8a
		1, //0x8b
		1, //0x8c
		1, //0x8d
		2, //0x8e
		1, //0x8f
		1, //0x90
		1, //0x91
		1, //0x92
		1, //0x93
		1, //0x94
		1, //0x95
		2, //0x96
		1, //0x97
		1, //0x98
		1, //0x99
		1, //0x9a
		1, //0x9b
		1, //0x9c
		1, //0x9d
		2, //0x9e
		1, //0x9f
		1, //0xa0
		1, //0xa1
		1, //0xa2
		1, //0xa3
		1, //0xa4
		1, //0xa5
		2, //0xa6
		1, //0xa7
		1, //0xa8
		1, //0xa9
		1, //0xaa
		1, //0xab
		1, //0xac
		1, //0xad
		2, //0xae
		1, //0xaf
		1, //0xb0
		1, //0xb1
		1, //0xb2
		1, //0xb3
		1, //0xb4
		1, //0xb5
		2, //0xb6
		1, //0xb7
		1, //0xb8
		1, //0xb9
		1, //0xba
		1, //0xbb
		1, //0xbc
		1, //0xbd
		2, //0xbe
		1, //0xbf
		2, //0xc0(分岐するとき+3)
		3, //0xc1
		3, //0xc2(分岐するとき+1)
		4, //0xc3
		3, //0xc4(分岐するとき+3)
		4, //0xc5
		2, //0xc6
		4, //0xc7
		2, //0xc8(分岐するとき+3)
		4, //0xc9
		3, //0xca(分岐するとき+1)
		1, //0xcb
		3, //0xcc(分岐するとき+3)
		6, //0xcd
		2, //0xce
		4, //0xcf
		2, //0xd0(分岐するとき+3)
		3, //0xd1
		3, //0xd2(分岐するとき+1)
		0xDEADBEEF, //0xd3
		3, //0xd4(分岐するとき+3)
		4, //0xd5
		2, //0xd6
		4, //0xd7
		2, //0xd8(分岐するとき+3)
		4, //0xd9
		3, //0xda(分岐するとき+1)
		0xDEADBEEF, //0xdb
		3, //0xdc(分岐するとき+3)
		0xDEADBEEF, //0xdd
		2, //0xde
		4, //0xdf
		3, //0xe0
		3, //0xe1
		2, //0xe2
		0xDEADBEEF, //0xe3
		0xDEADBEEF, //0xe4
		4, //0xe5
		2, //0xe6
		4, //0xe7
		4, //0xe8
		1, //0xe9
		4, //0xea
		0xDEADBEEF, //0xeb
		0xDEADBEEF, //0xec
		0xDEADBEEF, //0xed
		2, //0xee
		4, //0xef
		3, //0xf0
		3, //0xf1
		2, //0xf2
		1, //0xf3
		0xDEADBEEF, //0xf4
		4, //0xf5
		2, //0xf6
		4, //0xf7
		3, //0xf8
		2, //0xf9
		4, //0xfa
		1, //0xfb
		0xDEADBEEF, //0xfc
		0xDEADBEEF, //0xfd
		2, //0xfe
		4, //0xff
	};

	struct ROM_INFO {
		uint8_t CGB_Flag;
		uint8_t SGB_Flag;
		uint8_t Cartridge_Type;
		uint8_t ROM_Size;
		uint8_t RAM_Size;
	};
	ROM_INFO rom_info;

	struct GBX_REGISTER{
		union {
			uint16_t AF;
			struct {
				union {
					uint8_t Flags;
					struct {
						uint8_t F_unused : 4;
						uint8_t F_C : 1;
						uint8_t F_H : 1;
						uint8_t F_N : 1;
						uint8_t F_Z : 1;
					};
				};
				uint8_t A;
			};
		};

		union {
			uint16_t BC;
			struct {
				uint8_t C;
				uint8_t B;
			};
		};

		union {
			uint16_t DE;
			struct {
				uint8_t E;
				uint8_t D;
			};
		};

		union {
			uint16_t HL;
			struct {
				uint8_t L;
				uint8_t H;
			};
		};

		uint16_t SP;
		uint16_t PC;
	};
	GBX_REGISTER gbx_register;

#define RAM_SIZE 0x10000
	union GBX_RAM{
		uint8_t RAM[RAM_SIZE];

		struct {
			uint8_t _16kib_ROM_bank_00[0x4000];
			uint8_t _16kib_ROM_bank_XX[0x4000];
			uint8_t _8kib_VRAM[0x2000];
			uint8_t _8kib_External_RAM[0x2000];
			uint8_t _4kib_WRAM_1[0x1000];
			uint8_t _4kib_WRAM_2[0x1000];
			uint8_t _4kib_Mirror___C000_DDFF[0x1E00];
			uint8_t sprite_attribute_table[0x00A0];
			uint8_t unused[0x0060];
			uint8_t IO_register[0x0080];
			uint8_t HRAM[0x007F];
			uint8_t Interrupt_enable_register[0x0001];
		};
	};
	GBX_RAM gbx_ram;

	uint8_t* ROM_bank_data_ptr;//ROMが32KB以上のときに使うやつ

	bool IME_Flag = false;

	uint8_t bank_no = 0;
	bool RAM_Enable_Flag = false;

	uint8_t* get_read_ROM_address() {
		if (bank_no <= 1) {
			return &(gbx_ram.RAM[0x4000]);
		}
		else {
			return (uint8_t*)(ROM_bank_data_ptr + ((bank_no - 2) * 0x4000));
		}
	}

	uint8_t read_RAM_8bit(uint16_t read_address) {
		uint8_t read_value = 0x00;

		if (read_address <= 0x3FFF) {//ROMバンク00
			read_value = gbx_ram.RAM[read_address];
		}
		else if (read_address <= 0x7FFF) {//ROMバンク01-7F
			//read_value = gbx_ram.RAM[read_address];

			uint8_t* read_ROM_address = get_read_ROM_address();
			read_value = read_ROM_address[read_address - 0x4000];
		}
		else if (0xA000 <= read_address && read_address <= 0xBFFF) {//RAMバンク00-03（存在する場合）
			read_value = gbx_ram.RAM[read_address];
		}
		else if (read_address == 0xFF00) {//ジョイパッド
			if ((gbx_ram.RAM[0xFF00] & 0b00010000) == 0) {//方向キー

				uint8_t b_down = (key->get_input_state__GBX__(INPUT_MY_ID_DOWN) != 0) ? 1 : 0;
				uint8_t b_up = (key->get_input_state__GBX__(INPUT_MY_ID_UP) != 0) ? 1 : 0;
				uint8_t b_left = (key->get_input_state__GBX__(INPUT_MY_ID_LEFT) != 0) ? 1 : 0;
				uint8_t b_right = (key->get_input_state__GBX__(INPUT_MY_ID_RIGHT) != 0) ? 1 : 0;

				read_value = (gbx_ram.RAM[0xFF00] & 0b00110000);
				read_value |= ((~((b_down << 3) | (b_up << 2) | (b_left << 1) | b_right)) & 0b00001111);
			}
			else if ((gbx_ram.RAM[0xFF00] & 0b00100000) == 0) {//アクションキー
				uint8_t b_start = (key->get_input_state__GBX__(INPUT_MY_ID_START) != 0) ? 1 : 0;
				uint8_t b_select = (key->get_input_state__GBX__(INPUT_MY_ID_SELECT) != 0) ? 1 : 0;
				uint8_t b_b = (key->get_input_state__GBX__(INPUT_MY_ID_B) != 0) ? 1 : 0;
				uint8_t b_a = (key->get_input_state__GBX__(INPUT_MY_ID_A) != 0) ? 1 : 0;

				read_value = (gbx_ram.RAM[0xFF00] & 0b00110000);
				read_value |= ((~((b_start << 3) | (b_select << 2) | (b_b << 1) | b_a)) & 0b00001111);
			}
			else {
				read_value = gbx_ram.RAM[0xFF00] & 0b00110000;
				read_value |= 0b00001111;
			}
		}
		else {//通常読み取り
			read_value = gbx_ram.RAM[read_address];
		}

		return read_value;
	}

	uint16_t read_RAM_16bit(uint16_t read_address) {
		return (gbx_ram.RAM[read_address] | (gbx_ram.RAM[read_address + 1] << 8));
	}

	void write_RAM_8bit(uint16_t write_address, uint8_t value) {
		if (write_address <= 0x1FFF) {//RAMの有効化
			if ((value & 0b00001111) == 0x0A) {
				RAM_Enable_Flag = true;
			}
			else {
				RAM_Enable_Flag = false;
			}
		}
		else if (write_address <= 0x3FFF) {//ROMバンク番号
			//M_debug_printf("write_address <= 0x3FFF [value = 0x%02x]\n", value);
			//system("pause");

			bank_no = (value & 0b00011111);
			if (bank_no == 0) {
				bank_no = 1;
			}
		}
		else if (write_address <= 0x5FFF) {//RAMバンク番号-または-ROMバンク番号の上位ビット
			//M_debug_printf("write_address <= 0x5FFF [value = 0x%02x]\n", value);
			//system("pause");
		}
		else if (write_address <= 0x7FFF) {//ROM / RAMモード選択
			//M_debug_printf("write_address <= 0x7FFF [value = 0x%02x]\n", value);
			//system("pause");
		}
		else if (0xA000 <= write_address && write_address <= 0xBFFF) {//RAMバンク00-03（存在する場合）
			gbx_ram.RAM[write_address] = value;
		}
		else if (write_address == 0xFF00) {//ジョイパッド
			gbx_ram.RAM[0xFF00] = (value & 0b00110000);
		}
		else if (write_address == 0xFF04) {
			gbx_ram.RAM[write_address] = 0x00;
		}
		else if (write_address == 0xFF46) {//DMA
			uint16_t src_address = value << 8;
			memcpy((void*)(&(gbx_ram.RAM[0xFE00])), (void*)(&(gbx_ram.RAM[src_address])), 40*4);

			//サイクル数はすすめない //cpu_machine_cycle += 160;//160 M-cycle かかる
		}
		else {//通常書き込み
			gbx_ram.RAM[write_address] = value;
		}
	}

	void write_RAM_16bit(uint16_t write_address, uint16_t value) {
		//gbx_ram.RAM[write_address] = (uint8_t)(value & 0b0000000011111111);
		//gbx_ram.RAM[write_address + 1] = (uint8_t)(value >> 8);

		write_RAM_8bit(write_address, (uint8_t)(value & 0b0000000011111111));
		write_RAM_8bit(write_address + 1, (uint8_t)(value >> 8));
	}

	void push_8bit(uint8_t value) {
		gbx_register.SP--;
		gbx_ram.RAM[gbx_register.SP] = value;
	}

	void push_16bit(uint16_t value) {
		gbx_register.SP--;
		gbx_ram.RAM[gbx_register.SP] = (uint8_t)(value >> 8);
		gbx_register.SP--;
		gbx_ram.RAM[gbx_register.SP] = (uint8_t)(value & 0b11111111);
	}

	uint8_t pop_8bit() {
		uint8_t value = gbx_ram.RAM[gbx_register.SP];
		gbx_register.SP++;

		return value;
	}

	uint16_t pop_16bit() {
		uint8_t low_value = gbx_ram.RAM[gbx_register.SP];
		gbx_register.SP++;
		uint8_t high_value = gbx_ram.RAM[gbx_register.SP];
		gbx_register.SP++;

		return (uint16_t)(low_value | (high_value << 8));
	}

	//=============================================================================
	//=============================================================================
	
	void calc_Z_Flag__8bit(uint8_t result) {
		if (result == 0) {
			gbx_register.F_Z = 1;
		}
		else {
			gbx_register.F_Z = 0;
		}
	}

	//op1 + op2
	void calc_H_Flag__8bit_add(uint8_t op1, uint8_t op2) {
		if ((op1 & 0b00001111) + (op2 & 0b00001111) > 0b00001111) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 - op2
	void calc_H_Flag__8bit_sub(uint8_t op1, uint8_t op2) {
		//uint8_t result = op1 - op2;
		//if ((((uint8_t)(op1 ^ op2 ^ result)) & 0b00010000) != 0) {
		if ((op1 & 0b00001111) < (op2 & 0b00001111)) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 + op2 + (CFLAG)
	void calc_H_Flag__8bit_adc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((op1 & 0b00001111) + (op2 & 0b00001111) + C_Flag > 0b00001111) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 - (op2 + (CFLAG))
	void calc_H_Flag__8bit_sbc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((((op1 & 0b00001111) - ((op2 & 0b00001111) + C_Flag)) & 0b10000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 + op2
	void calc_C_Flag__8bit_add(uint8_t op1, uint8_t op2) {
		if ((uint16_t)op1 + (uint16_t)op2 > 0b11111111) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 - op2
	void calc_C_Flag__8bit_sub(uint8_t op1, uint8_t op2) {
		if (op2 > op1) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 + op2 + (C_FLAG)
	void calc_C_Flag__8bit_adc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((uint16_t)op1 + (uint16_t)op2 + (uint16_t)C_Flag > 0b11111111) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 - (op2 + (C_FLAG))
	void calc_C_Flag__8bit_sbc(uint8_t op1, uint8_t op2, uint8_t C_Flag) {
		if ((((uint16_t)op1 - (uint16_t)((uint16_t)op2 + (uint16_t)C_Flag)) & 0b100000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//op1 + op2 (16bit計算)
	void calc_H_Flag__16bit_add(uint16_t op1, uint16_t op2) {
		if ((op1 & 0b0000111111111111) + (op2 & 0b0000111111111111) > 0b0000111111111111) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
	}

	//op1 + op2 (16bit計算)
	void calc_C_Flag__16bit_add(uint16_t op1, uint16_t op2) {
		if ((uint32_t)op1 + (uint32_t)op2 > 0b1111111111111111) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}
	}

	//=============================================================================


	//未定義のCPUの命令
	void cpu_fnc__GARBAGE() {
		M_debug_printf("ERROR cpu_fnc__GARBAGE()...\n");
		MessageBox(NULL, _T("未定義の命令を実行しました"), _T("情報"), MB_ICONERROR);
	}


	//=============================================================================
	
	//void cpu_fnc__ADC_A_r8() {
	//}

	//0x8F
	void cpu_fnc__ADC_A_A() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.A, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.A, bef_C_Flag);

		gbx_register.A += gbx_register.A;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x88
	void cpu_fnc__ADC_A_B() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.B, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.B, bef_C_Flag);

		gbx_register.A += gbx_register.B;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x89
	void cpu_fnc__ADC_A_C() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.C, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.C, bef_C_Flag);

		gbx_register.A += gbx_register.C;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8A
	void cpu_fnc__ADC_A_D() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.D, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.D, bef_C_Flag);

		gbx_register.A += gbx_register.D;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8B
	void cpu_fnc__ADC_A_E() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.E, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.E, bef_C_Flag);

		gbx_register.A += gbx_register.E;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8C
	void cpu_fnc__ADC_A_H() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.H, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.H, bef_C_Flag);

		gbx_register.A += gbx_register.H;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8D
	void cpu_fnc__ADC_A_L() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_adc(gbx_register.A, gbx_register.L, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, gbx_register.L, bef_C_Flag);

		gbx_register.A += gbx_register.L;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x8E
	void cpu_fnc__ADC_A_addrHL() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);

		gbx_register.A += value_8bit;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0xCE
	void cpu_fnc__ADC_A_n8() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);
		calc_C_Flag__8bit_adc(gbx_register.A, value_8bit, bef_C_Flag);

		//if ((((gbx_register.A & 0b1111) + (value_8bit & 0b1111) + bef_C_Flag) & 0b10000) != 0) {
		//	gbx_register.F_H = 1;
		//}
		//else {
		//	gbx_register.F_H = 0;
		//}

		//if (((gbx_register.A + value_8bit + bef_C_Flag) & 0b100000000) != 0) {
		//	gbx_register.F_C = 1;
		//}
		//else {
		//	gbx_register.F_C = 0;
		//}


		gbx_register.A += value_8bit;
		gbx_register.A += bef_C_Flag;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;

		gbx_register.PC++;
	}

	//void cpu_fnc__ADD_A_r8() {
	//}

	//0x87
	void cpu_fnc__ADD_A_A() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.A);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.A);

		gbx_register.A += gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x80
	void cpu_fnc__ADD_A_B() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.B);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.B);

		gbx_register.A += gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x81
	void cpu_fnc__ADD_A_C() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.C);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.C);

		gbx_register.A += gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x82
	void cpu_fnc__ADD_A_D() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.D);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.D);

		gbx_register.A += gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x83
	void cpu_fnc__ADD_A_E() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.E);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.E);

		gbx_register.A += gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x84
	void cpu_fnc__ADD_A_H() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.H);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.H);

		gbx_register.A += gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x85
	void cpu_fnc__ADD_A_L() {
		calc_H_Flag__8bit_add(gbx_register.A, gbx_register.L);
		calc_C_Flag__8bit_add(gbx_register.A, gbx_register.L);

		gbx_register.A += gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0x86
	void cpu_fnc__ADD_A_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_add(gbx_register.A, value_8bit);
		calc_C_Flag__8bit_add(gbx_register.A, value_8bit);

		gbx_register.A += value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;
	}

	//0xC6
	void cpu_fnc__ADD_A_n8() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_add(gbx_register.A, value_8bit);
		calc_C_Flag__8bit_add(gbx_register.A, value_8bit);

		gbx_register.A += value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 0;

		gbx_register.PC++;
	}

	//void cpu_fnc__AND_A_r8() {
	//}

	//0xA7
	void cpu_fnc__AND_A_A() {
		gbx_register.A &= gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA0
	void cpu_fnc__AND_A_B() {
		gbx_register.A &= gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA1
	void cpu_fnc__AND_A_C() {
		gbx_register.A &= gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA2
	void cpu_fnc__AND_A_D() {
		gbx_register.A &= gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA3
	void cpu_fnc__AND_A_E() {
		gbx_register.A &= gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA4
	void cpu_fnc__AND_A_H() {
		gbx_register.A &= gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA5
	void cpu_fnc__AND_A_L() {
		gbx_register.A &= gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xA6
	void cpu_fnc__AND_A_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A &= value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;
	}

	//0xE6
	void cpu_fnc__AND_A_n8() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		gbx_register.A &= value_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
		gbx_register.F_C = 0;

		gbx_register.PC++;
	}

	//void cpu_fnc__CP_A_r8() {
	//}

	//0xBF
	void cpu_fnc__CP_A_A() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.A);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.A);

		tmp_8bit -= gbx_register.A;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xB8
	void cpu_fnc__CP_A_B() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.B);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.B);

		tmp_8bit -= gbx_register.B;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xB9
	void cpu_fnc__CP_A_C() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.C);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.C);

		tmp_8bit -= gbx_register.C;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBA
	void cpu_fnc__CP_A_D() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.D);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.D);

		tmp_8bit -= gbx_register.D;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBB
	void cpu_fnc__CP_A_E() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.E);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.E);

		tmp_8bit -= gbx_register.E;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBC
	void cpu_fnc__CP_A_H() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.H);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.H);

		tmp_8bit -= gbx_register.H;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBD
	void cpu_fnc__CP_A_L() {
		uint8_t tmp_8bit = gbx_register.A;

		calc_C_Flag__8bit_sub(tmp_8bit, gbx_register.L);
		calc_H_Flag__8bit_sub(tmp_8bit, gbx_register.L);

		tmp_8bit -= gbx_register.L;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xBE
	void cpu_fnc__CP_A_addrHL() {
		uint8_t tmp_8bit = gbx_register.A;
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_C_Flag__8bit_sub(tmp_8bit, value_1_8bit);
		calc_H_Flag__8bit_sub(tmp_8bit, value_1_8bit);

		tmp_8bit -= value_1_8bit;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;
	}

	//0xFE
	void cpu_fnc__CP_A_n8() {
		uint8_t tmp_8bit = gbx_register.A;
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		calc_C_Flag__8bit_sub(tmp_8bit, value_1_8bit);
		calc_H_Flag__8bit_sub(tmp_8bit, value_1_8bit);

		tmp_8bit -= value_1_8bit;

		calc_Z_Flag__8bit(tmp_8bit);
		gbx_register.F_N = 1;

		gbx_register.PC++;
	}

	//void cpu_fnc__DEC_r8() {
	//}

	//0x3D
	void cpu_fnc__DEC_A() {
		calc_H_Flag__8bit_sub(gbx_register.A, 1);

		gbx_register.A--;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 1;
	}

	//0x05
	void cpu_fnc__DEC_B() {
		calc_H_Flag__8bit_sub(gbx_register.B, 1);

		gbx_register.B--;

		calc_Z_Flag__8bit(gbx_register.B);
		gbx_register.F_N = 1;
	}

	//0x0D
	void cpu_fnc__DEC_C() {
		calc_H_Flag__8bit_sub(gbx_register.C, 1);

		gbx_register.C--;

		calc_Z_Flag__8bit(gbx_register.C);
		gbx_register.F_N = 1;
	}

	//0x15
	void cpu_fnc__DEC_D() {
		calc_H_Flag__8bit_sub(gbx_register.D, 1);

		gbx_register.D--;

		calc_Z_Flag__8bit(gbx_register.D);
		gbx_register.F_N = 1;
	}

	//0x1D
	void cpu_fnc__DEC_E() {
		calc_H_Flag__8bit_sub(gbx_register.E, 1);

		gbx_register.E--;

		calc_Z_Flag__8bit(gbx_register.E);
		gbx_register.F_N = 1;
	}

	//0x25
	void cpu_fnc__DEC_H() {
		calc_H_Flag__8bit_sub(gbx_register.H, 1);

		gbx_register.H--;

		calc_Z_Flag__8bit(gbx_register.H);
		gbx_register.F_N = 1;
	}

	//0x2D
	void cpu_fnc__DEC_L() {
		calc_H_Flag__8bit_sub(gbx_register.L, 1);

		gbx_register.L--;

		calc_Z_Flag__8bit(gbx_register.L);
		gbx_register.F_N = 1;
	}

	//0x35
	void cpu_fnc__DEC_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_sub(value_1_8bit, 1);

		value_1_8bit--;

		calc_Z_Flag__8bit(value_1_8bit);

		write_RAM_8bit(gbx_register.HL, value_1_8bit);

		gbx_register.F_N = 1;
	}

	//void cpu_fnc__INC_r8() {
	//}

	//0x3C
	void cpu_fnc__INC_A() {
		calc_H_Flag__8bit_add(gbx_register.A, 1);

		gbx_register.A++;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
	}

	//0x04
	void cpu_fnc__INC_B() {
		calc_H_Flag__8bit_add(gbx_register.B, 1);

		gbx_register.B++;

		calc_Z_Flag__8bit(gbx_register.B);
		gbx_register.F_N = 0;
	}

	//0x0C
	void cpu_fnc__INC_C() {
		calc_H_Flag__8bit_add(gbx_register.C, 1);

		gbx_register.C++;

		calc_Z_Flag__8bit(gbx_register.C);
		gbx_register.F_N = 0;
	}

	//0x14
	void cpu_fnc__INC_D() {
		calc_H_Flag__8bit_add(gbx_register.D, 1);

		gbx_register.D++;

		calc_Z_Flag__8bit(gbx_register.D);
		gbx_register.F_N = 0;
	}

	//0x1C
	void cpu_fnc__INC_E() {
		calc_H_Flag__8bit_add(gbx_register.E, 1);

		gbx_register.E++;

		calc_Z_Flag__8bit(gbx_register.E);
		gbx_register.F_N = 0;
	}

	//0x24
	void cpu_fnc__INC_H() {
		calc_H_Flag__8bit_add(gbx_register.H, 1);

		gbx_register.H++;

		calc_Z_Flag__8bit(gbx_register.H);
		gbx_register.F_N = 0;
	}

	//0x2C
	void cpu_fnc__INC_L() {
		calc_H_Flag__8bit_add(gbx_register.L, 1);

		gbx_register.L++;

		calc_Z_Flag__8bit(gbx_register.L);
		gbx_register.F_N = 0;
	}

	//0x34
	void cpu_fnc__INC_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_add(value_1_8bit, 1);

		value_1_8bit++;

		calc_Z_Flag__8bit(value_1_8bit);

		write_RAM_8bit(gbx_register.HL, value_1_8bit);

		gbx_register.F_N = 0;
	}

	//void cpu_fnc__OR_A_r8() {
	//}

	//0xB7
	void cpu_fnc__OR_A_A() {
		gbx_register.A |= gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB0
	void cpu_fnc__OR_A_B() {
		gbx_register.A |= gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB1
	void cpu_fnc__OR_A_C() {
		gbx_register.A |= gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB2
	void cpu_fnc__OR_A_D() {
		gbx_register.A |= gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB3
	void cpu_fnc__OR_A_E() {
		gbx_register.A |= gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB4
	void cpu_fnc__OR_A_H() {
		gbx_register.A |= gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB5
	void cpu_fnc__OR_A_L() {
		gbx_register.A |= gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xB6
	void cpu_fnc__OR_A_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A |= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xF6
	void cpu_fnc__OR_A_n8() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		gbx_register.A |= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;

		gbx_register.PC++;
	}

	//void cpu_fnc__SBC_A_r8() {
	//}

	//0x9F
	void cpu_fnc__SBC_A_A() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.A, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.A, bef_C_Flag);

		gbx_register.A -= (gbx_register.A + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x98
	void cpu_fnc__SBC_A_B() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.B, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.B, bef_C_Flag);

		gbx_register.A -= (gbx_register.B + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x99
	void cpu_fnc__SBC_A_C() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.C, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.C, bef_C_Flag);

		gbx_register.A -= (gbx_register.C + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9A
	void cpu_fnc__SBC_A_D() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.D, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.D, bef_C_Flag);

		gbx_register.A -= (gbx_register.D + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9B
	void cpu_fnc__SBC_A_E() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.E, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.E, bef_C_Flag);

		gbx_register.A -= (gbx_register.E + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9C
	void cpu_fnc__SBC_A_H() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.H, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.H, bef_C_Flag);

		gbx_register.A -= (gbx_register.H + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9D
	void cpu_fnc__SBC_A_L() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		calc_H_Flag__8bit_sbc(gbx_register.A, gbx_register.L, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, gbx_register.L, bef_C_Flag);

		gbx_register.A -= (gbx_register.L + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x9E
	void cpu_fnc__SBC_A_addrHL() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);

		gbx_register.A -= (value_1_8bit + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0xDE
	void cpu_fnc__SBC_A_n8() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);
		calc_C_Flag__8bit_sbc(gbx_register.A, value_1_8bit, bef_C_Flag);

		gbx_register.A -= (value_1_8bit + bef_C_Flag);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;

		gbx_register.PC++;
	}

	//void cpu_fnc__SUB_A_r8() {
	//}

	//0x97
	void cpu_fnc__SUB_A_A() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.A);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.A);

		gbx_register.A -= (gbx_register.A);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x90
	void cpu_fnc__SUB_A_B() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.B);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.B);

		gbx_register.A -= (gbx_register.B);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x91
	void cpu_fnc__SUB_A_C() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.C);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.C);

		gbx_register.A -= (gbx_register.C);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x92
	void cpu_fnc__SUB_A_D() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.D);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.D);

		gbx_register.A -= (gbx_register.D);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x93
	void cpu_fnc__SUB_A_E() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.E);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.E);

		gbx_register.A -= (gbx_register.E);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x94
	void cpu_fnc__SUB_A_H() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.H);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.H);

		gbx_register.A -= (gbx_register.H);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x95
	void cpu_fnc__SUB_A_L() {
		calc_H_Flag__8bit_sub(gbx_register.A, gbx_register.L);
		calc_C_Flag__8bit_sub(gbx_register.A, gbx_register.L);

		gbx_register.A -= (gbx_register.L);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0x96
	void cpu_fnc__SUB_A_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		calc_H_Flag__8bit_sub(gbx_register.A, value_1_8bit);
		calc_C_Flag__8bit_sub(gbx_register.A, value_1_8bit);

		gbx_register.A -= (value_1_8bit);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;
	}

	//0xD6
	void cpu_fnc__SUB_A_n8() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		calc_H_Flag__8bit_sub(gbx_register.A, value_1_8bit);
		calc_C_Flag__8bit_sub(gbx_register.A, value_1_8bit);

		gbx_register.A -= (value_1_8bit);

		calc_Z_Flag__8bit(gbx_register.A);

		gbx_register.F_N = 1;

		gbx_register.PC++;
	}

	//void cpu_fnc__XOR_A_r8() {
	//}

	//0xAF
	void cpu_fnc__XOR_A_A() {
		gbx_register.A ^= gbx_register.A;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xA8
	void cpu_fnc__XOR_A_B() {
		gbx_register.A ^= gbx_register.B;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xA9
	void cpu_fnc__XOR_A_C() {
		gbx_register.A ^= gbx_register.C;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAA
	void cpu_fnc__XOR_A_D() {
		gbx_register.A ^= gbx_register.D;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAB
	void cpu_fnc__XOR_A_E() {
		gbx_register.A ^= gbx_register.E;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAC
	void cpu_fnc__XOR_A_H() {
		gbx_register.A ^= gbx_register.H;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAD
	void cpu_fnc__XOR_A_L() {
		gbx_register.A ^= gbx_register.L;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xAE
	void cpu_fnc__XOR_A_addrHL() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A ^= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;
	}

	//0xEE
	void cpu_fnc__XOR_A_n8() {
		uint8_t value_1_8bit = read_RAM_8bit(gbx_register.PC);

		gbx_register.A ^= value_1_8bit;

		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
		gbx_register.F_C = 0;

		gbx_register.PC++;
	}

	//=============================================================================

	//void cpu_fnc__ADD_HL_r16() {
	//}

	//0x09
	void cpu_fnc__ADD_HL_BC() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.BC);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.BC);

		gbx_register.HL += gbx_register.BC;

		gbx_register.F_N = 0;
	}

	//0x19
	void cpu_fnc__ADD_HL_DE() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.DE);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.DE);

		gbx_register.HL += gbx_register.DE;

		gbx_register.F_N = 0;
	}

	//0x29
	void cpu_fnc__ADD_HL_HL() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.HL);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.HL);

		gbx_register.HL += gbx_register.HL;

		gbx_register.F_N = 0;
	}

	//void cpu_fnc__INC_r16() {
	//}

	//0x03
	void cpu_fnc__INC_BC() {
		gbx_register.BC++;
	}

	//0x13
	void cpu_fnc__INC_DE() {
		gbx_register.DE++;
	}

	//0x23
	void cpu_fnc__INC_HL() {
		gbx_register.HL++;
	}

	//void cpu_fnc__DEC_r16() {
	//}

	//0x0B
	void cpu_fnc__DEC_BC() {
		gbx_register.BC--;
	}

	//0x1B
	void cpu_fnc__DEC_DE() {
		gbx_register.DE--;
	}

	//0x2B
	void cpu_fnc__DEC_HL() {
		gbx_register.HL--;
	}

	//=============================================================================

	////void cpu_fnc__BIT_u3_r8() {
	////
	////}
	////
	////void cpu_fnc__BIT_u3_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SET_u3_r8() {
	////
	////}
	////
	////void cpu_fnc__SET_u3_addrHL() {
	////
	////}
	////
	////void cpu_fnc__RES_u3_r8() {
	////
	////}
	////
	////void cpu_fnc__RES_u3_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SWAP_r8() {
	////
	////}
	////
	////void cpu_fnc__SWAP_addrHL() {
	////
	////}

	//=============================================================================

	////void cpu_fnc__RL_r8() {
	////
	////}
	////
	////void cpu_fnc__RL_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x17
	void cpu_fnc__RLA() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		if ((gbx_register.A & 0b10000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A <<= 1;
		gbx_register.A |= bef_C_Flag;

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__RLC_r8() {
	////
	////}
	////
	////void cpu_fnc__RLC_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x07
	void cpu_fnc__RLCA() {
		if ((gbx_register.A & 0b10000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A <<= 1;
		gbx_register.A |= gbx_register.F_C;

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__RR_r8() {
	////
	////}
	////
	////void cpu_fnc__RR_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x1F
	void cpu_fnc__RRA() {
		uint8_t bef_C_Flag = gbx_register.F_C;

		if ((gbx_register.A & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A >>= 1;
		gbx_register.A |= (bef_C_Flag << 7);

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__RRC_r8() {
	////
	////}
	////
	////void cpu_fnc__RRC_addrHL() {
	////
	////}


	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	//0x0F
	void cpu_fnc__RRCA() {
		if ((gbx_register.A & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.A >>= 1;
		gbx_register.A |= (gbx_register.F_C << 7);

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


	////void cpu_fnc__SLA_r8() {
	////
	////}
	////
	////void cpu_fnc__SLA_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SRA_r8() {
	////
	////}
	////
	////void cpu_fnc__SRA_addrHL() {
	////
	////}
	////
	////void cpu_fnc__SRL_r8() {
	////
	////}
	////
	////void cpu_fnc__SRL_addrHL() {
	////
	////}

	//=============================================================================

	//void cpu_fnc__LD_r8_r8() {
	//
	//}

	//0x7F
	void cpu_fnc__LD_A_A() {
		gbx_register.A = gbx_register.A;
	}

	//0x78
	void cpu_fnc__LD_A_B() {
		gbx_register.A = gbx_register.B;
	}

	//0x79
	void cpu_fnc__LD_A_C() {
		gbx_register.A = gbx_register.C;
	}

	//0x7A
	void cpu_fnc__LD_A_D() {
		gbx_register.A = gbx_register.D;
	}

	//0x7B
	void cpu_fnc__LD_A_E() {
		gbx_register.A = gbx_register.E;
	}

	//0x7C
	void cpu_fnc__LD_A_H() {
		gbx_register.A = gbx_register.H;
	}

	//0x7D
	void cpu_fnc__LD_A_L() {
		gbx_register.A = gbx_register.L;
	}

	//###

	//0x47
	void cpu_fnc__LD_B_A() {
		gbx_register.B = gbx_register.A;
	}

	//0x40
	void cpu_fnc__LD_B_B() {
		gbx_register.B = gbx_register.B;
	}

	//0x41
	void cpu_fnc__LD_B_C() {
		gbx_register.B = gbx_register.C;
	}

	//0x42
	void cpu_fnc__LD_B_D() {
		gbx_register.B = gbx_register.D;
	}

	//0x43
	void cpu_fnc__LD_B_E() {
		gbx_register.B = gbx_register.E;
	}

	//0x44
	void cpu_fnc__LD_B_H() {
		gbx_register.B = gbx_register.H;
	}

	//0x45
	void cpu_fnc__LD_B_L() {
		gbx_register.B = gbx_register.L;
	}

	//###

	//0x4F
	void cpu_fnc__LD_C_A() {
		gbx_register.C = gbx_register.A;
	}

	//0x48
	void cpu_fnc__LD_C_B() {
		gbx_register.C = gbx_register.B;
	}

	//0x49
	void cpu_fnc__LD_C_C() {
		gbx_register.C = gbx_register.C;
	}

	//0x4A
	void cpu_fnc__LD_C_D() {
		gbx_register.C = gbx_register.D;
	}

	//0x4B
	void cpu_fnc__LD_C_E() {
		gbx_register.C = gbx_register.E;
	}

	//0x4C
	void cpu_fnc__LD_C_H() {
		gbx_register.C = gbx_register.H;
	}

	//0x4D
	void cpu_fnc__LD_C_L() {
		gbx_register.C = gbx_register.L;
	}

	//###

	//0x57
	void cpu_fnc__LD_D_A() {
		gbx_register.D = gbx_register.A;
	}

	//0x50
	void cpu_fnc__LD_D_B() {
		gbx_register.D = gbx_register.B;
	}

	//0x51
	void cpu_fnc__LD_D_C() {
		gbx_register.D = gbx_register.C;
	}

	//0x52
	void cpu_fnc__LD_D_D() {
		gbx_register.D = gbx_register.D;
	}

	//0x53
	void cpu_fnc__LD_D_E() {
		gbx_register.D = gbx_register.E;
	}

	//0x54
	void cpu_fnc__LD_D_H() {
		gbx_register.D = gbx_register.H;
	}

	//0x55
	void cpu_fnc__LD_D_L() {
		gbx_register.D = gbx_register.L;
	}

	//###

	//0x5F
	void cpu_fnc__LD_E_A() {
		gbx_register.E = gbx_register.A;
	}

	//0x58
	void cpu_fnc__LD_E_B() {
		gbx_register.E = gbx_register.B;
	}

	//0x59
	void cpu_fnc__LD_E_C() {
		gbx_register.E = gbx_register.C;
	}

	//0x5A
	void cpu_fnc__LD_E_D() {
		gbx_register.E = gbx_register.D;
	}

	//0x5B
	void cpu_fnc__LD_E_E() {
		gbx_register.E = gbx_register.E;
	}

	//0x5C
	void cpu_fnc__LD_E_H() {
		gbx_register.E = gbx_register.H;
	}

	//0x5D
	void cpu_fnc__LD_E_L() {
		gbx_register.E = gbx_register.L;
	}

	//###

	//0x67
	void cpu_fnc__LD_H_A() {
		gbx_register.H = gbx_register.A;
	}

	//0x60
	void cpu_fnc__LD_H_B() {
		gbx_register.H = gbx_register.B;
	}

	//0x61
	void cpu_fnc__LD_H_C() {
		gbx_register.H = gbx_register.C;
	}

	//0x62
	void cpu_fnc__LD_H_D() {
		gbx_register.H = gbx_register.D;
	}

	//0x63
	void cpu_fnc__LD_H_E() {
		gbx_register.H = gbx_register.E;
	}

	//0x64
	void cpu_fnc__LD_H_H() {
		gbx_register.H = gbx_register.H;
	}

	//0x65
	void cpu_fnc__LD_H_L() {
		gbx_register.H = gbx_register.L;
	}

	//###

	//0x6F
	void cpu_fnc__LD_L_A() {
		gbx_register.L = gbx_register.A;
	}

	//0x68
	void cpu_fnc__LD_L_B() {
		gbx_register.L = gbx_register.B;
	}

	//0x69
	void cpu_fnc__LD_L_C() {
		gbx_register.L = gbx_register.C;
	}

	//0x6A
	void cpu_fnc__LD_L_D() {
		gbx_register.L = gbx_register.D;
	}

	//0x6B
	void cpu_fnc__LD_L_E() {
		gbx_register.L = gbx_register.E;
	}

	//0x6C
	void cpu_fnc__LD_L_H() {
		gbx_register.L = gbx_register.H;
	}

	//0x6D
	void cpu_fnc__LD_L_L() {
		gbx_register.L = gbx_register.L;
	}

	//###

	//void cpu_fnc__LD_r8_n8() {
	//}

	//0x3E
	void cpu_fnc__LD_A_n8() {
		gbx_register.A = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x06
	void cpu_fnc__LD_B_n8() {
		gbx_register.B = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x0E
	void cpu_fnc__LD_C_n8() {
		gbx_register.C = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x16
	void cpu_fnc__LD_D_n8() {
		gbx_register.D = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x1E
	void cpu_fnc__LD_E_n8() {
		gbx_register.E = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x26
	void cpu_fnc__LD_H_n8() {
		gbx_register.H = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//0x2E
	void cpu_fnc__LD_L_n8() {
		gbx_register.L = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;
	}

	//void cpu_fnc__LD_r16_n16() {
	//}

	//0x01
	void cpu_fnc__LD_BC_n16() {
		uint16_t value_16bit = read_RAM_16bit(gbx_register.PC);
		gbx_register.BC = value_16bit;

		gbx_register.PC += 2;
	}

	//0x11
	void cpu_fnc__LD_DE_n16() {
		uint16_t value_16bit = read_RAM_16bit(gbx_register.PC);
		gbx_register.DE = value_16bit;

		gbx_register.PC += 2;
	}

	//0x21
	void cpu_fnc__LD_HL_n16() {
		uint16_t value_16bit = read_RAM_16bit(gbx_register.PC);
		gbx_register.HL = value_16bit;

		gbx_register.PC += 2;
	}


	//void cpu_fnc__LD_addrHL_r8() {
	//}

	//0x77
	void cpu_fnc__LD_addrHL_A() {
		write_RAM_8bit(gbx_register.HL, gbx_register.A);
	}

	//0x70
	void cpu_fnc__LD_addrHL_B() {
		write_RAM_8bit(gbx_register.HL, gbx_register.B);
	}

	//0x71
	void cpu_fnc__LD_addrHL_C() {
		write_RAM_8bit(gbx_register.HL, gbx_register.C);
	}

	//0x72
	void cpu_fnc__LD_addrHL_D() {
		write_RAM_8bit(gbx_register.HL, gbx_register.D);
	}

	//0x73
	void cpu_fnc__LD_addrHL_E() {
		write_RAM_8bit(gbx_register.HL, gbx_register.E);
	}

	//0x74
	void cpu_fnc__LD_addrHL_H() {
		write_RAM_8bit(gbx_register.HL, gbx_register.H);
	}

	//0x75
	void cpu_fnc__LD_addrHL_L() {
		write_RAM_8bit(gbx_register.HL, gbx_register.L);
	}


	//0x36
	void cpu_fnc__LD_addrHL_n8() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.PC);

		write_RAM_8bit(gbx_register.HL, value_8bit);

		gbx_register.PC++;
	}


	//void cpu_fnc__LD_r8_addrHL() {
	//}
	
	//0x7E
	void cpu_fnc__LD_A_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.A = value_8bit;
	}

	//0x46
	void cpu_fnc__LD_B_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.B = value_8bit;
	}

	//0x4E
	void cpu_fnc__LD_C_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.C = value_8bit;
	}

	//0x56
	void cpu_fnc__LD_D_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.D = value_8bit;
	}

	//0x5E
	void cpu_fnc__LD_E_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.E = value_8bit;
	}

	//0x66
	void cpu_fnc__LD_H_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.H = value_8bit;
	}

	//0x6E
	void cpu_fnc__LD_L_addrHL() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.HL);

		gbx_register.L = value_8bit;
	}


	//void cpu_fnc__LD_addrr16_A() {
	//}

	//0x02
	void cpu_fnc__LD_addrBC_A() {
		write_RAM_8bit(gbx_register.BC, gbx_register.A);
	}

	//0x12
	void cpu_fnc__LD_addrDE_A() {
		write_RAM_8bit(gbx_register.DE, gbx_register.A);
	}

	//void cpu_fnc__LD_addrHL_A()//実装済み

	//0xEA
	void cpu_fnc__LD_addrn16_A() {
		uint16_t addr_16bit = read_RAM_16bit(gbx_register.PC);

		write_RAM_8bit(addr_16bit, gbx_register.A);

		gbx_register.PC += 2;
	}

	//0xE0
	void cpu_fnc__LDH_addrn16_A() {
		uint8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		write_RAM_8bit(0xFF00 + relative_addr, gbx_register.A);

		gbx_register.PC++;
	}

	//0xE2
	void cpu_fnc__LDH_addrC_A() {
		write_RAM_8bit(0xFF00 + gbx_register.C, gbx_register.A);
	}


	//void cpu_fnc__LD_A_addrr16() {
	//}

	//0x0A
	void cpu_fnc__LD_A_addrBC() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.BC);

		gbx_register.A = value_8bit;
	}

	//0x1A
	void cpu_fnc__LD_A_addrDE() {
		uint8_t value_8bit = read_RAM_8bit(gbx_register.DE);

		//printf("check!\n");

		gbx_register.A = value_8bit;
	}

	//void cpu_fnc__LD_A_addrHL()//実装済み


	//0xFA
	void cpu_fnc__LD_A_addrn16() {
		uint16_t addr_16bit = read_RAM_16bit(gbx_register.PC);
		uint8_t value_8bit = read_RAM_8bit(addr_16bit);

		gbx_register.A = value_8bit;

		gbx_register.PC += 2;
	}

	//0xF0
	void cpu_fnc__LDH_A_addrn16() {
		uint8_t relative_addr = read_RAM_8bit(gbx_register.PC);
		gbx_register.A = read_RAM_8bit(0xFF00 + relative_addr);

		gbx_register.PC++;
	}

	//0xF2
	void cpu_fnc__LDH_A_addrC() {
		gbx_register.A = read_RAM_8bit(0xFF00 + gbx_register.C);
	}

	//0x22
	void cpu_fnc__LD_addrHLI_A() {
		write_RAM_8bit(gbx_register.HL, gbx_register.A);

		gbx_register.HL++;
	}

	//0x32
	void cpu_fnc__LD_addrHLD_A() {
		write_RAM_8bit(gbx_register.HL, gbx_register.A);

		gbx_register.HL--;
	}

	//0x2A
	void cpu_fnc__LD_A_addrHLI() {
		gbx_register.A = read_RAM_8bit(gbx_register.HL);

		gbx_register.HL++;
	}

	//0x3A
	void cpu_fnc__LD_A_addrHLD() {
		gbx_register.A = read_RAM_8bit(gbx_register.HL);

		gbx_register.HL--;
	}

	//=============================================================================
	
	//0xC3
	void cpu_fnc__JP_n16() {
		gbx_register.PC = read_RAM_16bit(gbx_register.PC);

		//gbx_register.PC += 2;
	}


	//void cpu_fnc__JP_cc_n16() {
	//}

	//0xCA
	void cpu_fnc__JP_FZ_n16() {//Z
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 1) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}

	//0xC2
	void cpu_fnc__JP_FNZ_n16() {//NZ
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 0) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}

	//0xDA
	void cpu_fnc__JP_FC_n16() {//C
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 1) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}

	//0xD2
	void cpu_fnc__JP_FNC_n16() {//NC
		uint16_t jmp_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 0) {
			gbx_register.PC = jmp_addr;

			cpu_machine_cycle++;
		}
	}


	//0xE9
	void cpu_fnc__JP_HL() {
		gbx_register.PC = gbx_register.HL;
	}


	//0x18
	void cpu_fnc__JR_e8() {
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;

		gbx_register.PC += relative_addr;
	}


	//void cpu_fnc__JR_cc_e8() {
	//}
	
	//0x28
	void cpu_fnc__JR_FZ_e8() {//Z
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_Z == 1) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;
	}
	//0x20
	void cpu_fnc__JR_FNZ_e8() {//NZ
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_Z == 0) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;
	}
	//0x38
	void cpu_fnc__JR_FC_e8() {//C
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_C == 1) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;
	}
	//0x30
	void cpu_fnc__JR_FNC_e8() {//NC
		int8_t relative_addr = read_RAM_8bit(gbx_register.PC);

		if (gbx_register.F_C == 0) {
			gbx_register.PC += relative_addr;

			cpu_machine_cycle++;
		}

		gbx_register.PC++;
	}


	//0xCD
	void cpu_fnc__CALL_n16() {
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		push_16bit(gbx_register.PC);

		gbx_register.PC = call_addr;
	}


	//void cpu_fnc__CALL_cc_n16() {
	//}

	//0xCC
	void cpu_fnc__CALL_FZ_n16() {//Z
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 1) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}

	//0xC4
	void cpu_fnc__CALL_FNZ_n16() {//NZ
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_Z == 0) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}

	//0xDC
	void cpu_fnc__CALL_FC_n16() {//C
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 1) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}

	//0xD4
	void cpu_fnc__CALL_FNC_n16() {//NC
		uint16_t call_addr = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;

		if (gbx_register.F_C == 0) {
			push_16bit(gbx_register.PC);

			gbx_register.PC = call_addr;

			cpu_machine_cycle += 3;
		}
	}


	//0xC9
	void cpu_fnc__RET() {
		gbx_register.PC = pop_16bit();
	}


	//void cpu_fnc__RET_cc() {
	//}

	//0xC8
	void cpu_fnc__RET_FZ() {//Z
		if (gbx_register.F_Z == 1) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}

	//0xC0
	void cpu_fnc__RET_FNZ() {//NZ
		if (gbx_register.F_Z == 0) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}

	//0xD8
	void cpu_fnc__RET_FC() {//C
		if (gbx_register.F_C == 1) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}

	//0xD0
	void cpu_fnc__RET_FNC() {//NC
		if (gbx_register.F_C == 0) {
			gbx_register.PC = pop_16bit();

			cpu_machine_cycle += 3;
		}
	}


	//0xD9
	void cpu_fnc__RETI() {
		gbx_register.PC = pop_16bit();

		IME_Flag = true;
	}


	//void cpu_fnc__RST_vec() {
	//}

	void RST_process(uint8_t vec) {
		uint16_t call_addr = (uint16_t)(0x0000 + vec);

		push_16bit(gbx_register.PC);

		gbx_register.PC = call_addr;
	}

	//0xC7
	void cpu_fnc__RST_00h() {
		RST_process(0x00);
	}

	//0xCF
	void cpu_fnc__RST_08h() {
		RST_process(0x08);
	}

	//0xD7
	void cpu_fnc__RST_10h() {
		RST_process(0x10);
	}

	//0xDF
	void cpu_fnc__RST_18h() {
		RST_process(0x18);
	}

	//0xE7
	void cpu_fnc__RST_20h() {
		RST_process(0x20);
	}

	//0xEF
	void cpu_fnc__RST_28h() {
		RST_process(0x28);
	}

	//0xF7
	void cpu_fnc__RST_30h() {
		RST_process(0x30);
	}

	//0xFF
	void cpu_fnc__RST_38h() {
		RST_process(0x38);
	}


	//=============================================================================
	
	//0x39
	void cpu_fnc__ADD_HL_SP() {
		calc_H_Flag__16bit_add(gbx_register.HL, gbx_register.SP);
		calc_C_Flag__16bit_add(gbx_register.HL, gbx_register.SP);

		gbx_register.HL += gbx_register.SP;

		gbx_register.F_N = 0;
	}


	//0xE8
	void cpu_fnc__ADD_SP_e8() {
		uint8_t value_8bit_unsigned = read_RAM_8bit(gbx_register.PC);
		int8_t value_8bit_signed = (int8_t)value_8bit_unsigned;

		//calc_H_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//calc_C_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//
		//gbx_register.SP = (uint16_t)((int32_t)gbx_register.SP + (int32_t)value_8bit_signed);

		int32_t result = (int32_t)gbx_register.SP + (int32_t)value_8bit_signed;
		uint32_t carry_bit = (uint32_t)gbx_register.SP ^ (uint32_t)value_8bit_signed ^ (uint32_t)result;
		
		gbx_register.SP = (uint16_t)result;

		if ((carry_bit & 0b00010000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}

		if ((carry_bit & 0b100000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;

		gbx_register.PC++;
	}

	//0x33
	void cpu_fnc__INC_SP() {
		gbx_register.SP++;
	}

	//0x3B
	void cpu_fnc__DEC_SP() {
		gbx_register.SP--;
	}

	//0x31
	void cpu_fnc__LD_SP_n16() {
		gbx_register.SP = read_RAM_16bit(gbx_register.PC);

		gbx_register.PC += 2;
	}


	//0x08
	void cpu_fnc__LD_addrn16_SP() {
		uint16_t addr_16bit = read_RAM_16bit(gbx_register.PC);

		write_RAM_8bit(addr_16bit, gbx_register.SP & 0b11111111);
		write_RAM_8bit(addr_16bit + 1, (gbx_register.SP >> 8) & 0b11111111);

		gbx_register.PC += 2;
	}


	//0xF8
	void cpu_fnc__LD_HL_SPpe8() {//LD HL,SP+e8
		uint8_t value_8bit_unsigned = read_RAM_8bit(gbx_register.PC);
		int8_t value_8bit_signed = (int8_t)value_8bit_unsigned;

		//calc_H_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//calc_C_Flag__8bit_add(gbx_register.SP & 0b0000000011111111, value_8bit_unsigned);
		//
		//gbx_register.HL = (uint16_t)((int32_t)gbx_register.SP + (int32_t)value_8bit_signed);

		int32_t result = (int32_t)gbx_register.SP + (int32_t)value_8bit_signed;
		uint32_t carry_bit = (uint32_t)gbx_register.SP ^ (uint32_t)value_8bit_signed ^ (uint32_t)result;

		gbx_register.HL = (uint16_t)result;

		if ((carry_bit & 0b00010000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}

		if ((carry_bit & 0b100000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.F_Z = 0;
		gbx_register.F_N = 0;

		gbx_register.PC++;
	}


	//0xF9
	void cpu_fnc__LD_SP_HL() {
		gbx_register.SP = gbx_register.HL;
	}


	//0xF5
	void cpu_fnc__PUSH_AF() {
		gbx_register.F_unused = 0;//未使用領域は0にしておく

		push_16bit(gbx_register.AF);
	}


	//void cpu_fnc__PUSH_r16() {
	//}

	//0xC5
	void cpu_fnc__PUSH_BC() {
		push_16bit(gbx_register.BC);
	}

	//0xD5
	void cpu_fnc__PUSH_DE() {
		push_16bit(gbx_register.DE);
	}

	//0xE5
	void cpu_fnc__PUSH_HL() {
		push_16bit(gbx_register.HL);
	}


	//0xF1
	void cpu_fnc__POP_AF() {
		gbx_register.AF = pop_16bit();

		if ((gbx_register.AF & 0b0000000010000000) != 0) {
			gbx_register.F_Z = 1;
		}
		else {
			gbx_register.F_Z = 0;
		}
		
		if ((gbx_register.AF & 0b0000000001000000) != 0) {
			gbx_register.F_N = 1;
		}
		else {
			gbx_register.F_N = 0;
		}
		
		if ((gbx_register.AF & 0b0000000000100000) != 0) {
			gbx_register.F_H = 1;
		}
		else {
			gbx_register.F_H = 0;
		}
		
		if ((gbx_register.AF & 0b0000000000010000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		gbx_register.F_unused = 0;//未使用領域は0にしておく
	}


	//void cpu_fnc__POP_r16() {
	//}

	//0xC1
	void cpu_fnc__POP_BC() {
		gbx_register.BC = pop_16bit();
	}

	//0xD1
	void cpu_fnc__POP_DE() {
		gbx_register.DE = pop_16bit();
	}

	//0xE1
	void cpu_fnc__POP_HL() {
		gbx_register.HL = pop_16bit();
	}


	//=============================================================================

	//0x3F
	void cpu_fnc__CCF() {
		gbx_register.F_C = (gbx_register.F_C == 0) ? 1 : 0;

		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//0x2F
	void cpu_fnc__CPL() {
		gbx_register.A = ~gbx_register.A;

		gbx_register.F_N = 1;
		gbx_register.F_H = 1;
	}

	//0x27
	void cpu_fnc__DAA() {
		int32_t tmp = gbx_register.A;
		if (gbx_register.F_N == 0) {
			if (gbx_register.F_C == 1 || tmp > 0x99) {
				tmp += 0x60;
				gbx_register.F_C = 1;
			}
			if (gbx_register.F_H == 1 || (tmp & 0xf) > 0x9) {
				tmp += 0x06;
			}
		}
		else {
			if (gbx_register.F_C == 1) {
				tmp -= 0x60;
			}
			if (gbx_register.F_H == 1) {
				tmp -= 0x06;
			}
		}
		gbx_register.A = (uint8_t)(tmp & 0xff);
		calc_Z_Flag__8bit(gbx_register.A);
		gbx_register.F_H = 0;

	}

	//0xF3
	void cpu_fnc__DI() {
		IME_Flag = false;
	}

	//0xFB
	void cpu_fnc__EI() {
		IME_Flag = true;
	}

	//0x76
	void cpu_fnc__HALT() {
		////M_debug_printf("HALT...\n");
		////
		////gbx_register.PC--;

		//M_debug_printf("HALT...\n");

		uint8_t IE_value = gbx_ram.RAM[0xFFFF];
		uint8_t IF_value = gbx_ram.RAM[0xFF0F];

		if (IME_Flag == true) {
			//if (!(((IE_value & IF_value) & 0b00011111) != 0)) {
				gbx_register.PC--;//PCをすすめずにとどまる

				tmp_CPU_HALT_Flag = true;
			//}
		}
		else {
			if (((IE_value & IF_value) & 0b00011111) != 0) {//割り込みが保留中のとき
				gbx_register.PC++;//ハードウエアのバグで1バイトすすめる
			}
			else {
				gbx_register.PC--;//PCをすすめずにとどまる
			}
		}
	}

	//0x00
	void cpu_fnc__NOP() {
	}

	//0x37
	void cpu_fnc__SCF() {
		gbx_register.F_C = 1;

		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	//0x10
	void cpu_fnc__STOP() {
		/*
		TODO
		詳細な実装をあとでする
		*/

		//M_debug_printf("STOP...\n");

		gbx_register.PC++;//stop命令は1バイトとばす

		//M_debug_printf("PC = 0x%04x\n", gbx_register.PC - 2);
		//system("pause");
	}

	//=============================================================================
	

	//0xCB
	void cpu_fnc__PREFIX() {
		uint8_t instruction_code_2 = read_RAM_8bit(gbx_register.PC);

		gbx_register.PC++;

		PREFIX_process(instruction_code_2);

		if (instruction_code_2 == 0x46 ||
			instruction_code_2 == 0x56 ||
			instruction_code_2 == 0x66 ||
			instruction_code_2 == 0x76 ||

			instruction_code_2 == 0x4E ||
			instruction_code_2 == 0x5E ||
			instruction_code_2 == 0x6E ||
			instruction_code_2 == 0x7E)
		{
			cpu_machine_cycle += 2;
		}
		else if (
			instruction_code_2 == 0x06 ||
			instruction_code_2 == 0x16 ||
			instruction_code_2 == 0x26 ||
			instruction_code_2 == 0x36 ||

			instruction_code_2 == 0x0E ||
			instruction_code_2 == 0x1E ||
			instruction_code_2 == 0x2E ||
			instruction_code_2 == 0x3E ||

			instruction_code_2 == 0x86 ||
			instruction_code_2 == 0x96 ||
			instruction_code_2 == 0xA6 ||
			instruction_code_2 == 0xB6 ||
			instruction_code_2 == 0xC6 ||
			instruction_code_2 == 0xD6 ||
			instruction_code_2 == 0xE6 ||
			instruction_code_2 == 0xF6 ||

			instruction_code_2 == 0x8E ||
			instruction_code_2 == 0x9E ||
			instruction_code_2 == 0xAE ||
			instruction_code_2 == 0xBE ||
			instruction_code_2 == 0xCE ||
			instruction_code_2 == 0xDE ||
			instruction_code_2 == 0xEE ||
			instruction_code_2 == 0xFE)
		{
			cpu_machine_cycle += 3;
		}
		else {
			cpu_machine_cycle++;
		}
	}

	void PREFIX_process(uint8_t instruction_code) {
		uint8_t* target_op_ptr = get_target_op_ptr(instruction_code); //操作する"レジスタかメモリ"のポインタ

		if (0x07 >= instruction_code) {//RLC
			PREFIX_process__RLC(target_op_ptr);
		}
		else if (0x0F >= instruction_code) {//RRC
			PREFIX_process__RRC(target_op_ptr);
		}
		else if (0x17 >= instruction_code) {//RL
			PREFIX_process__RL(target_op_ptr);
		}
		else if (0x1F >= instruction_code) {//RR
			PREFIX_process__RR(target_op_ptr);
		}
		else if (0x27 >= instruction_code) {//SLA
			PREFIX_process__SLA(target_op_ptr);
		}
		else if (0x2F >= instruction_code) {//SRA
			PREFIX_process__SRA(target_op_ptr);
		}
		else if (0x37 >= instruction_code) {//SWAP
			PREFIX_process__SWAP(target_op_ptr);
		}
		else if (0x3F >= instruction_code) {//SRL
			PREFIX_process__SRL(target_op_ptr);
		}
		else if (0x47 >= instruction_code) {//BIT 0
			PREFIX_process__BIT_0(target_op_ptr);
		}
		else if (0x4F >= instruction_code) {//BIT 1
			PREFIX_process__BIT_1(target_op_ptr);
		}
		else if (0x57 >= instruction_code) {//BIT 2
			PREFIX_process__BIT_2(target_op_ptr);
		}
		else if (0x5F >= instruction_code) {//BIT 3
			PREFIX_process__BIT_3(target_op_ptr);
		}
		else if (0x67 >= instruction_code) {//BIT 4
			PREFIX_process__BIT_4(target_op_ptr);
		}
		else if (0x6F >= instruction_code) {//BIT 5
			PREFIX_process__BIT_5(target_op_ptr);
		}
		else if (0x77 >= instruction_code) {//BIT 6
			PREFIX_process__BIT_6(target_op_ptr);
		}
		else if (0x7F >= instruction_code) {//BIT 7
			PREFIX_process__BIT_7(target_op_ptr);
		}
		else if (0x87 >= instruction_code) {//RES 0
			PREFIX_process__RES_0(target_op_ptr);
		}
		else if (0x8F >= instruction_code) {//RES 1
			PREFIX_process__RES_1(target_op_ptr);
		}
		else if (0x97 >= instruction_code) {//RES 2
			PREFIX_process__RES_2(target_op_ptr);
		}
		else if (0x9F >= instruction_code) {//RES 3
			PREFIX_process__RES_3(target_op_ptr);
		}
		else if (0xA7 >= instruction_code) {//RES 4
			PREFIX_process__RES_4(target_op_ptr);
		}
		else if (0xAF >= instruction_code) {//RES 5
			PREFIX_process__RES_5(target_op_ptr);
		}
		else if (0xB7 >= instruction_code) {//RES 6
			PREFIX_process__RES_6(target_op_ptr);
		}
		else if (0xBF >= instruction_code) {//RES 7
			PREFIX_process__RES_7(target_op_ptr);
		}
		else if (0xC7 >= instruction_code) {//SET 0
			PREFIX_process__SET_0(target_op_ptr);
		}
		else if (0xCF >= instruction_code) {//SET 1
			PREFIX_process__SET_1(target_op_ptr);
		}
		else if (0xD7 >= instruction_code) {//SET 2
			PREFIX_process__SET_2(target_op_ptr);
		}
		else if (0xDF >= instruction_code) {//SET 3
			PREFIX_process__SET_3(target_op_ptr);
		}
		else if (0xE7 >= instruction_code) {//SET 4
			PREFIX_process__SET_4(target_op_ptr);
		}
		else if (0xEF >= instruction_code) {//SET 5
			PREFIX_process__SET_5(target_op_ptr);
		}
		else if (0xF7 >= instruction_code) {//SET 6
			PREFIX_process__SET_6(target_op_ptr);
		}
		//else if (0xFF >= instruction_code) {//SET 7
		else {//SET 7
			PREFIX_process__SET_7(target_op_ptr);
		}
	}

	/*

	B		= +0x00
	C		= +0x01
	D		= +0x02
	E		= +0x03
	H		= +0x04
	L		= +0x05
	(HL)	= +0x06
	A		= +0x07

	*/
	uint8_t* get_target_op_ptr(uint8_t instruction_code) {
		uint8_t op_index = (uint8_t)(instruction_code % 8); //= 0～7

		if (op_index == 0) {
			return &(gbx_register.B);
		}
		else if (op_index == 1) {
			return &(gbx_register.C);
		}
		else if (op_index == 2) {
			return &(gbx_register.D);
		}
		else if (op_index == 3) {
			return &(gbx_register.E);
		}
		else if (op_index == 4) {
			return &(gbx_register.H);
		}
		else if (op_index == 5) {
			return &(gbx_register.L);
		}
		else if (op_index == 6) {
			return &(gbx_ram.RAM[gbx_register.HL]);
		}
		//else if (op_index == 7) {
		else {
			return &(gbx_register.A);
		}
	}


	void PREFIX_process__RLC(uint8_t* target_op_ptr) {
		if (((*target_op_ptr) & 0b10000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		(*target_op_ptr) <<= 1;
		(*target_op_ptr) |= gbx_register.F_C;

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	void PREFIX_process__RRC(uint8_t* target_op_ptr) {
		if (((*target_op_ptr) & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		(*target_op_ptr) >>= 1;
		(*target_op_ptr) |= (gbx_register.F_C << 7);

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	void PREFIX_process__RL(uint8_t* target_op_ptr) {
		uint8_t bef_C_Flag = gbx_register.F_C;

		if (((*target_op_ptr) & 0b10000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		(*target_op_ptr) <<= 1;
		(*target_op_ptr) |= bef_C_Flag;

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	void PREFIX_process__RR(uint8_t* target_op_ptr) {
		uint8_t bef_C_Flag = gbx_register.F_C;

		if (((*target_op_ptr) & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		(*target_op_ptr) >>= 1;
		(*target_op_ptr) |= (bef_C_Flag << 7);

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	void PREFIX_process__SLA(uint8_t* target_op_ptr) {
		if (((*target_op_ptr) & 0b10000000) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		(*target_op_ptr) <<= 1;

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	void PREFIX_process__SRA(uint8_t* target_op_ptr) {
		uint8_t bef_sign = (((*target_op_ptr) & 0b10000000) != 0) ? 1 : 0;

		if (((*target_op_ptr) & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		(*target_op_ptr) >>= 1;
		(*target_op_ptr) &= 0b01111111;
		(*target_op_ptr) |= (bef_sign << 7);

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}

	void PREFIX_process__SWAP(uint8_t* target_op_ptr) {
		uint8_t bef_value = (*target_op_ptr);

		(*target_op_ptr) >>= 4;
		(*target_op_ptr) &= 0b00001111;
		(*target_op_ptr) |= ((bef_value << 4) & 0b11110000);

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_C = 0;
		gbx_register.F_H = 0;
		gbx_register.F_N = 0;
	}

	void PREFIX_process__SRL(uint8_t* target_op_ptr) {
		if (((*target_op_ptr) & 0b00000001) != 0) {
			gbx_register.F_C = 1;
		}
		else {
			gbx_register.F_C = 0;
		}

		(*target_op_ptr) >>= 1;

		calc_Z_Flag__8bit(*target_op_ptr);
		gbx_register.F_N = 0;
		gbx_register.F_H = 0;
	}


	void PREFIX_sub_process__BIT(uint8_t check_bit, uint8_t* target_op_ptr) {
		if (((*target_op_ptr) & (0b00000001 << check_bit)) == 0) {
			gbx_register.F_Z = 1;
		}
		else {
			gbx_register.F_Z = 0;
		}

		gbx_register.F_N = 0;
		gbx_register.F_H = 1;
	}
	void PREFIX_process__BIT_0(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(0, target_op_ptr);
	}
	void PREFIX_process__BIT_1(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(1, target_op_ptr);
	}
	void PREFIX_process__BIT_2(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(2, target_op_ptr);
	}
	void PREFIX_process__BIT_3(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(3, target_op_ptr);
	}
	void PREFIX_process__BIT_4(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(4, target_op_ptr);
	}
	void PREFIX_process__BIT_5(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(5, target_op_ptr);
	}
	void PREFIX_process__BIT_6(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(6, target_op_ptr);
	}
	void PREFIX_process__BIT_7(uint8_t* target_op_ptr) {
		PREFIX_sub_process__BIT(7, target_op_ptr);
	}

	void PREFIX_sub_process__RES(uint8_t clear_bit, uint8_t* target_op_ptr) {
		(*target_op_ptr) &= (uint8_t)(~(0b00000001 << clear_bit));
	}
	void PREFIX_process__RES_0(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(0, target_op_ptr);
	}
	void PREFIX_process__RES_1(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(1, target_op_ptr);
	}
	void PREFIX_process__RES_2(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(2, target_op_ptr);
	}
	void PREFIX_process__RES_3(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(3, target_op_ptr);
	}
	void PREFIX_process__RES_4(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(4, target_op_ptr);
	}
	void PREFIX_process__RES_5(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(5, target_op_ptr);
	}
	void PREFIX_process__RES_6(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(6, target_op_ptr);
	}
	void PREFIX_process__RES_7(uint8_t* target_op_ptr) {
		PREFIX_sub_process__RES(7, target_op_ptr);
	}

	void PREFIX_sub_process__SET(uint8_t set_bit, uint8_t* target_op_ptr) {
		(*target_op_ptr) |= (0b00000001 << set_bit);
	}
	void PREFIX_process__SET_0(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(0, target_op_ptr);
	}
	void PREFIX_process__SET_1(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(1, target_op_ptr);
	}
	void PREFIX_process__SET_2(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(2, target_op_ptr);
	}
	void PREFIX_process__SET_3(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(3, target_op_ptr);
	}
	void PREFIX_process__SET_4(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(4, target_op_ptr);
	}
	void PREFIX_process__SET_5(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(5, target_op_ptr);
	}
	void PREFIX_process__SET_6(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(6, target_op_ptr);
	}
	void PREFIX_process__SET_7(uint8_t* target_op_ptr) {
		PREFIX_sub_process__SET(7, target_op_ptr);
	}


	//=============================================================================
	//=============================================================================

	void cpu_init() {
		//gbx_register.AF = 0;
		//gbx_register.BC = 0;
		//gbx_register.DE = 0;
		//gbx_register.HL = 0;
		//gbx_register.SP = 0xFFFE;
		//gbx_register.PC = 0x0100;

		////gbx_register.A = 0x11;
		////gbx_register.Flags = 0x80;
		////gbx_register.B = 0x00;
		////gbx_register.C = 0x00;
		////gbx_register.D = 0xFF;
		////gbx_register.E = 0x56;
		////gbx_register.H = 0x00;
		////gbx_register.L = 0x0D;
		////gbx_register.SP = 0xFFFE;
		////gbx_register.PC = 0x0100;

		gbx_register.A = 0x01;
		gbx_register.Flags = 0xB0;
		gbx_register.B = 0x00;
		gbx_register.C = 0x13;
		gbx_register.D = 0x00;
		gbx_register.E = 0xD8;
		gbx_register.H = 0x01;
		gbx_register.L = 0x4D;
		gbx_register.SP = 0xFFFE;
		gbx_register.PC = 0x0100;
	}

	void ppu_init() {
		memset(_8bit_bg_screen_data_160x144, 0, GBX_WIDTH * GBX_HEIGHT);
		memset(_8bit_window_screen_data_160x144, 0, GBX_WIDTH * GBX_HEIGHT);

		memset(_8bit_backbuffer_data_256x256__mtype0_dtype0, 0, 256 * 256);
		memset(_8bit_backbuffer_data_256x256__mtype0_dtype1, 0, 256 * 256);
		memset(_8bit_backbuffer_data_256x256__mtype1_dtype0, 0, 256 * 256);
		memset(_8bit_backbuffer_data_256x256__mtype1_dtype1, 0, 256 * 256);

		memset(_8bit_sprite_screen_data_160x144, 0, GBX_WIDTH * GBX_HEIGHT);
	}

	void init_util() {
		tmp_CPU_HALT_Flag = false;

		cpu_machine_cycle = 0;
		total_cpu_machine_cycle__div = 0;
		total_cpu_machine_cycle__tima = 0;

		//memset(gbx_ram.RAM, 0, RAM_SIZE);//RAMを初期化してはいけない

		IME_Flag = false;
	}

	int read_rom_file(const char* filename) {
		FILE* rom_fp;
		if (fopen_s(&rom_fp, filename, "rb") != 0) {
			return -1;
		}

		if (fseek(rom_fp, 0x143, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.CGB_Flag), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}

		if (fseek(rom_fp, 0x146, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.SGB_Flag), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}

		if (fseek(rom_fp, 0x147, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.Cartridge_Type), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}
		Main::Cartridge_Type = rom_info.Cartridge_Type;

		if (fseek(rom_fp, 0x148, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.ROM_Size), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}
		Main::PGM_size = rom_info.ROM_Size;

		if (fseek(rom_fp, 0x149, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(rom_info.RAM_Size), sizeof(uint8_t), 1, rom_fp) != 1) {
			goto read_rom_error;
		}
		Main::RAM_type = rom_info.RAM_Size;

		M_debug_printf("################################\n");
		M_debug_printf("CGB_Flag = 0x%02x\n", rom_info.CGB_Flag);
		M_debug_printf("SGB_Flag = 0x%02x\n", rom_info.SGB_Flag);
		M_debug_printf("Cartridge_Type = 0x%02x\n", rom_info.Cartridge_Type);
		M_debug_printf("ROM_Size = 0x%02x\n", rom_info.ROM_Size);
		M_debug_printf("RAM_Size = 0x%02x\n", rom_info.RAM_Size);
		M_debug_printf("################################\n");

		if (fseek(rom_fp, 0x0, SEEK_SET) != 0) {
			goto read_rom_error;
		}
		if (fread(&(gbx_ram.RAM), sizeof(uint8_t), 0x8000, rom_fp) != 0x8000) {
			goto read_rom_error;
		}

		if ((32 << rom_info.ROM_Size) > 32) {//ROMが32KBより大きいときはバンクを使う
			const size_t ROM_BANK_DATA_SIZE = 1024 * ((32 << rom_info.ROM_Size) - 32);

			ROM_bank_data_ptr = (uint8_t*)malloc(ROM_BANK_DATA_SIZE);

			if (fread(ROM_bank_data_ptr, sizeof(uint8_t), ROM_BANK_DATA_SIZE, rom_fp) != ROM_BANK_DATA_SIZE) {
				goto read_rom_error;
			}
		}


		return 0;

	read_rom_error:
		fclose(rom_fp);

		return -1;
	}

	void update_LCD_STAT() {
		if (ppu_line_y >= 144) {
			set_LCD_STAT_mode_flag(1);//1: VBlank
		}
		else if (0 <= ppu_line_x && ppu_line_x <= 80) {
			set_LCD_STAT_mode_flag(2);//2: Searching OAM
		}
		else if (80 < ppu_line_x && ppu_line_x <= 240) {
			set_LCD_STAT_mode_flag(3);//3: Transferring Data to LCD Controller
		}
		else {
			set_LCD_STAT_mode_flag(0);//0: HBlank
		}
	}

	void set_LCD_STAT_mode_flag(uint8_t mode_flag) {
		write_RAM_8bit(0xFF41, (read_RAM_8bit(0xFF41) & 0b11111100) | (mode_flag & 0b00000011));
	}

	uint32_t c_cycle_mod = 0;//余りのC-Cycle
	uint32_t ppu_line_x = 0;
	uint32_t ppu_line_y = 0;

	void execute_ppu_process(uint64_t c_cycle) {
		for (uint64_t i = 0; i < c_cycle; i++) {
			//M_debug_printf("ppu_line_x = %d, ppu_line_y = %d\n", ppu_line_x, ppu_line_y);

			if (ppu_line_x == 0 && ppu_line_y == 0) {//初回は上端にあるスプライトのチェックをする
				check_sprite_upside_16line();
			}

			if (ppu_line_x == 0) {
				hblank_process_stage_counter = 0;

				//===================================
				//LCD_STAT割り込み OAM
				if ((read_RAM_8bit(0xFF41) & 0b00100000) != 0) {//Bit 5 - Mode 2 OAM STAT Interrupt source
					gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
				}
			}

			//描画中
			if ((80 <= ppu_line_x && ppu_line_x < 240) && ppu_line_y < 144) {
				draw_backbuffer_bg_1pixel(ppu_line_x - 80, ppu_line_y);
				draw_backbuffer_window_1pixel(ppu_line_x - 80, ppu_line_y);
			}
			if (((80 - 8) <= ppu_line_x && ppu_line_x < 240) && ppu_line_y < 144) {//X座標左端もチェックする
				create_screen_sprite_data__1pixel(ppu_line_x - 80, ppu_line_y);
			}

			//HBLANK中
			if (240 <= ppu_line_x) {
				if (hblank_process_stage_counter == 0) {//初めてのときはHblankの割り込みを要求する
					//===================================
					//LCD_STAT割り込み HBLANK
					if ((read_RAM_8bit(0xFF41) & 0b00001000) != 0) {//Bit 3 - Mode 0 HBlank STAT Interrupt source
						gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
					}
				}

				hblank_process_stage_counter++;
				if (hblank_process_stage_counter > 1) {
					hblank_process_stage_counter = 1;
				}
			}

			ppu_line_x++;
			if (ppu_line_x >= 456) {
				ppu_line_x = 0;
				ppu_line_y++;
				write_RAM_8bit(0xFF44, ppu_line_y);

				update_window_flag__1line();//ウィンドウの内部情報を1ラインごとに更新する

				//===================================
				//LCD_STAT割り込み LYC=LY
				if ((read_RAM_8bit(0xFF41) & 0b01000000) != 0) {//Bit 6 - LYC=LY STAT Interrupt source
					if (ppu_line_y == read_RAM_8bit(0xFF45)) {
						gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
					}
				}
				//LCD_STATのLYC=LYのフラグを更新する
				if (ppu_line_y == read_RAM_8bit(0xFF45)) {
					write_RAM_8bit(0xFF41, read_RAM_8bit(0xFF41) | 0b00000100);
				}
				else {
					write_RAM_8bit(0xFF41, read_RAM_8bit(0xFF41) & 0b11111011);
				}

				if (ppu_line_y >= 144) {//Vblank開始
					if (vblank_process_stage_counter == 0) {//初めてのときはVblankの割り込みを要求する
						gbx_ram.RAM[0xFF0F] |= 0b00000001;//Vblankの割り込みを要求する

						//===================================
						//LCD_STAT割り込み VBLANK
						if ((read_RAM_8bit(0xFF41) & 0b00010000) != 0) {//Bit 4 - Mode 1 VBlank STAT Interrupt source
							gbx_ram.RAM[0xFF0F] |= 0b00000010;//STAT割り込みを要求する
						}
					}

					vblank_process_stage_counter++;
					if (vblank_process_stage_counter > 1) {
						vblank_process_stage_counter = 1;
					}

				}
				if (ppu_line_y >= 154) {//Vblank終了
					c_cycle_mod = c_cycle - (i + 1);//余りのC-Cycleを計算する

					return;
				}
			}
		}

		c_cycle_mod = 0;//普段は余り0
	}

	uint8_t* get_backbuffer_data_256x256_ptr(bool tilemap_type1_flag, bool tiledata_type1_flag) {
		if (tilemap_type1_flag == false && tiledata_type1_flag == false) {
			return _8bit_backbuffer_data_256x256__mtype0_dtype0;
		}
		else if (tilemap_type1_flag == false && tiledata_type1_flag == true) {
			return _8bit_backbuffer_data_256x256__mtype0_dtype1;
		}
		else if (tilemap_type1_flag == true && tiledata_type1_flag == false) {
			return _8bit_backbuffer_data_256x256__mtype1_dtype0;
		}
		//else if (tilemap_type1_flag == true && tiledata_type1_flag == true) {
		else {
			return _8bit_backbuffer_data_256x256__mtype1_dtype1;
		}
	}

	void draw_backbuffer_bg_1pixel(uint8_t screen_x, uint8_t screen_y) {
		//M_debug_printf("screen_x = %d, screen_y = %d\n", screen_x, screen_y);

		if ((gbx_ram.RAM[0xFF40] & 0b00000001) == 0) {//背景/ウィンドウが無効であるとき
			return;
		}

		bool tilemap_type1_flag;
		bool tiledata_type1_flag;

		if ((gbx_ram.RAM[0xFF40] & 0b00001000) == 0) {
			tilemap_type1_flag = false;
		}
		else {
			tilemap_type1_flag = true;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00010000) == 0) {
			tiledata_type1_flag = false;
		}
		else {
			tiledata_type1_flag = true;
		}

		uint8_t* _8bit_bg_backbuffer_data_256x256__ptr = get_backbuffer_data_256x256_ptr(tilemap_type1_flag, tiledata_type1_flag);

		uint8_t scroll_x = gbx_ram.RAM[0xFF43];
		uint8_t scroll_y = gbx_ram.RAM[0xFF42];

		uint8_t color_2bit = _8bit_bg_backbuffer_data_256x256__ptr[(256 * (uint8_t)(scroll_y + screen_y)) + (uint8_t)(scroll_x + screen_x)];
		if ((color_2bit & 0b00000011) != 0) {//背景色でないとき
			backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = true;
		}
		//else {
		//	backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = false;
		//}
		_8bit_bg_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = color_2bit;
	}


	/*
	1ラインごとにウインドウの内部情報を更新するためのやつ
	*/
	void update_window_flag__1line() {
		if (window_backbuffer_draw_internal_flag_x == true) {
			window_backbuffer_draw_internal_flag_x = false;

			window_backbuffer_draw_internal_counter_y++;
		}
	}
	bool window_backbuffer_draw_internal_flag_x = false;
	uint32_t window_backbuffer_draw_internal_counter_y = 0;
	void draw_backbuffer_window_1pixel(uint8_t screen_x, uint8_t screen_y) {
		if ((gbx_ram.RAM[0xFF40] & 0b00000001) == 0) {//背景/ウィンドウが無効であるとき
			return;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00100000) == 0) {//ウィンドウが無効であるとき
			return;
		}

		bool tilemap_type1_flag;
		bool tiledata_type1_flag;

		if ((gbx_ram.RAM[0xFF40] & 0b01000000) == 0) {
			tilemap_type1_flag = false;
		}
		else {
			tilemap_type1_flag = true;
		}

		if ((gbx_ram.RAM[0xFF40] & 0b00010000) == 0) {
			tiledata_type1_flag = false;
		}
		else {
			tiledata_type1_flag = true;
		}

		uint8_t* _8bit_window_backbuffer_data_256x256__ptr = get_backbuffer_data_256x256_ptr(tilemap_type1_flag, tiledata_type1_flag);

		uint8_t window_x = gbx_ram.RAM[0xFF4B] - 7;
		uint8_t window_y = gbx_ram.RAM[0xFF4A];

		if (screen_x < window_x ||
			screen_y < window_y)
		{
			//透明の部分
			
			//_8bit_window_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = 0xFF;
		
			return;
		}

		uint8_t color_2bit = _8bit_window_backbuffer_data_256x256__ptr[(uint8_t)(screen_x - window_x) + ((uint8_t)window_backbuffer_draw_internal_counter_y/*(screen_y - window_y)*/ * 256)];
		if ((color_2bit & 0b00000011) != 0) {//背景色でないとき
			backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = true;
		}
		//else {
		//	backbuffer_sprite_mask[screen_x + screen_y * GBX_WIDTH] = false;
		//}
		_8bit_window_screen_data_160x144[screen_x + screen_y * GBX_WIDTH] = color_2bit;

		window_backbuffer_draw_internal_flag_x = true;//このラインでウインドウが描画されたフラグをたてる
	}

	void _abstruct__create_256_256_backbuffer(uint8_t* backbuffer_data_ptr, bool tilemap_type1_flag, bool tiledata_type1_flag) {
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				int8_t tile_no;//符号あり
				if (tilemap_type1_flag == false) {
					tile_no = (int8_t)(gbx_ram.RAM[0x9800 + (j + i * 32)]);
				}
				else {
					tile_no = (int8_t)(gbx_ram.RAM[0x9C00 + (j + i * 32)]);
				}

				uint8_t* tile_data_ptr;
				if (tiledata_type1_flag == true) {
					tile_data_ptr = &(gbx_ram.RAM[0x8000]);
				}
				else {
					tile_data_ptr = &(gbx_ram.RAM[0x9000]);
				}

				//M_debug_printf("[index = %d]tile_no = %c[%02x]\n", (j + i * 32), tile_no, tile_no);

				for (int y = 0; y < 8; y++) {
					for (int x = 0; x < 8; x++) {
						uint8_t pixel_color_2bit;

						if (tiledata_type1_flag == true) {//タイル番号の符号なしで計算する
							pixel_color_2bit = (((tile_data_ptr[0x10 * (uint8_t)tile_no + y * 2]) >> (7 - x)) & 0b00000001) |
								((((tile_data_ptr[0x10 * (uint8_t)tile_no + y * 2 + 1]) >> (7 - x)) & 0b00000001) << 1);
						}
						else {//タイル番号の符号ありで計算する
							pixel_color_2bit = (((tile_data_ptr[0x10 * tile_no + y * 2]) >> (7 - x)) & 0b00000001) |
								((((tile_data_ptr[0x10 * tile_no + y * 2 + 1]) >> (7 - x)) & 0b00000001) << 1);
						}

						backbuffer_data_ptr[(8 * 256 * i + 256 * y) + (j * 8 + x)] = pixel_color_2bit;
					}
				}
			}

		}
	}

	void create_all_256x256_backbuffer() {
		/*
		uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype0[256 * 256];
		uint8_t _8bit_backbuffer_data_256x256__mtype0_dtype1[256 * 256];
		uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype0[256 * 256];
		uint8_t _8bit_backbuffer_data_256x256__mtype1_dtype1[256 * 256];
		*/

		_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype0_dtype0, false, false);
		_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype0_dtype1, false, true);
		_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype1_dtype0, true, false);
		_abstruct__create_256_256_backbuffer(_8bit_backbuffer_data_256x256__mtype1_dtype1, true, true);
	}

	void execute_draw_screenbuffer_1sprite_8x8_data(int32_t sprite_x, int32_t sprite_y, uint8_t tile_no, bool sprite_reverse_x_flag, bool sprite_reverse_y_flag, bool palette_OBP1_flag, bool sprite_max_priority_flag) {
		uint8_t* tile_data_ptr = &(gbx_ram.RAM[0x8000]);

		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				int32_t offset_X = sprite_x + x;
				int32_t offset_Y = sprite_y + y;

				if (offset_X < 0 || offset_Y < 0 || GBX_WIDTH <= offset_X || GBX_HEIGHT <= offset_Y) {//画面外は描画しない
					continue;
				}

				uint8_t pixel_color_x_shift_bit;
				if (sprite_reverse_x_flag == true) {
					pixel_color_x_shift_bit = x;
				}
				else {
					pixel_color_x_shift_bit = (7 - x);
				}
				uint8_t pixel_color_y_shift_bit;
				if (sprite_reverse_y_flag == true) {
					pixel_color_y_shift_bit = (7 - y);
				}
				else {
					pixel_color_y_shift_bit = y;
				}
				uint8_t pixel_color_2bit;
				pixel_color_2bit = (((tile_data_ptr[0x10 * tile_no + pixel_color_y_shift_bit * 2]) >> pixel_color_x_shift_bit) & 0b00000001) |
					((((tile_data_ptr[0x10 * tile_no + pixel_color_y_shift_bit * 2 + 1]) >> pixel_color_x_shift_bit) & 0b00000001) << 1);

				if (palette_OBP1_flag == true) {//パレットが1のときは最上位ビットを1にしておく
					pixel_color_2bit |= 0b10000000;
				}

				if (sprite_max_priority_flag == false) {//優先順位が低いときは最上位から２番目のビットを1にしておく
					pixel_color_2bit |= 0b01000000;
				}

				if ((pixel_color_2bit & 0b00111111) == 0) {//透明部分はとばす
					continue;
				}

				_8bit_sprite_screen_data_160x144[offset_X + (offset_Y * GBX_WIDTH)] = pixel_color_2bit;
			}
		}
	}


	struct Sprite_Info {
		uint32_t x = 0;
		uint32_t y = 0;
		uint8_t tile_no = 0;
		bool sprite_max_priority_flag = false;
		bool sprite_reverse_y_flag = false;
		bool sprite_reverse_x_flag = false;
		bool palette_OBP1_flag = false;
		bool size_8x16_flag = false;
	};
	vector<Sprite_Info> sprite_info_list;
	void init_sprite_info_list() {
		sprite_info_list.clear();
		sprite_info_list.shrink_to_fit();
	}
	/*
	画面に表示されない16ラインにスプライトがあるかチェックする
	*/
	void check_sprite_upside_16line() {
		for (int y = -16; y < 0; y++) {
			for (int x = -8; x < GBX_WIDTH; x++) {//X座標左端もチェックする
				create_screen_sprite_data__1pixel(x, y);
			}
		}
	}

	void create_screen_sprite_data__1pixel(uint32_t pixel_x, uint32_t pixel_y) {
		if ((gbx_ram.RAM[0xFF40] & 0b00000010) == 0) {//スプライトが無効なとき
			return;
		}

		for (int i = 0; i < 40; i++) {
			Sprite_Info s_info;

			/*
			s_info.x, s_info.x はスクリーンの座標
			*/
			s_info.y = (gbx_ram.RAM[0xFE00 + (4 * (39 - i))] - 16);
			s_info.x = (gbx_ram.RAM[0xFE00 + (4 * (39 - i)) + 1] - 8);
			if (!(pixel_x == s_info.x && pixel_y == s_info.y)) {
				continue;
			}
			s_info.tile_no = gbx_ram.RAM[0xFE00 + (4 * (39 - i)) + 2];
			uint8_t attribute = gbx_ram.RAM[0xFE00 + (4 * (39 - i)) + 3];

			s_info.sprite_max_priority_flag = ((attribute & 0b10000000) != 0) ? false : true;
			s_info.sprite_reverse_y_flag = ((attribute & 0b01000000) != 0) ? true : false;
			s_info.sprite_reverse_x_flag = ((attribute & 0b00100000) != 0) ? true : false;
			s_info.palette_OBP1_flag = ((attribute & 0b00010000) != 0) ? true : false;

			if ((gbx_ram.RAM[0xFF40] & 0b00000100) == 0) {//8x8のとき
				s_info.size_8x16_flag = false;
			}
			else {//8x16のとき
				s_info.size_8x16_flag = true;
			}

			sprite_info_list.push_back(s_info);

		}
	}

	void draw_screenbuffer_sprite_data() {
		for (int i = 0; i < sprite_info_list.size(); i++) {
			Sprite_Info s_info = sprite_info_list[i];

			if (s_info.size_8x16_flag == false) {//8x8のとき
				execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y, s_info.tile_no, s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
			}
			else {//8x16のとき
				if (s_info.sprite_reverse_y_flag == false) {
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y, (s_info.tile_no & 0b11111110), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y + 8, (uint8_t)(s_info.tile_no | 0b00000001), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
				}
				else {
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y, (uint8_t)(s_info.tile_no | 0b00000001), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
					execute_draw_screenbuffer_1sprite_8x8_data(s_info.x, s_info.y + 8, (s_info.tile_no & 0b11111110), s_info.sprite_reverse_x_flag, s_info.sprite_reverse_y_flag, s_info.palette_OBP1_flag, s_info.sprite_max_priority_flag);
				}
			}
		}
	}


	bool backbuffer_sprite_mask[GBX_WIDTH * GBX_HEIGHT] = { false };//背景色でない場合true

	void draw_screen_bg(MyDirectXSystem* myDirectXSystem) {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t color_no = _8bit_bg_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color = get_bg_window_palette(color_no);

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	void draw_screen_window(MyDirectXSystem* myDirectXSystem) {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t color_no = _8bit_window_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color;
				if (color_no == 0xFF) {//透明部分の時
					color = 0x00000000;
				}
				else {
					color = get_bg_window_palette(color_no);
				}

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

	void draw_screen_sprite(MyDirectXSystem* myDirectXSystem) {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(GBX_WIDTH, GBX_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < GBX_HEIGHT; y++) {
			for (int x = 0; x < GBX_WIDTH; x++) {
				uint8_t color_no = _8bit_sprite_screen_data_160x144[y * GBX_WIDTH + x];
				DWORD color;

				if ((color_no & 0b01000000) != 0) {//優先順位が低いとき
					if (backbuffer_sprite_mask[y * GBX_WIDTH + x] == true || color_no == 0xFF) {//背景色でない場合
						color = 0x00000000;
					}
					else {
						color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
					}
				}
				else {
					if (color_no == 0xFF) {//透明部分の時
						color = 0x00000000;
					}
					else {
						color = get_sprite_palette(color_no & 0b00111111, ((color_no & 0b10000000) != 0) ? true : false);
					}
				}

				pTextureBuffer[y * GBX_WIDTH + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		//MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0);
		MyDirectXDraw::draw_texture_base_leftup_enable_size(myDirectXSystem, pTexture, (float)GBX_WIDTH, (float)GBX_HEIGHT, 0, 0, 4.0);

		pTexture->Release();
	}

#ifdef GBX_EMU_DEBUG
	void _debug_draw_screen_256x256_backbuffer(MyDirectXSystem* myDirectXSystem, uint8_t buffer_type) {
		LPDIRECT3DTEXTURE9 pTexture;
		if (FAILED(myDirectXSystem->get_pDevice3D()->CreateTexture(256, 256, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, NULL))) {
			return;
		}
		D3DLOCKED_RECT lockedRect;
		pTexture->LockRect(0, &lockedRect, NULL, 0);
		DWORD* pTextureBuffer = (DWORD*)lockedRect.pBits;

		for (int y = 0; y < 256; y++) {
			for (int x = 0; x < 256; x++) {
				uint8_t color_no;
				if (buffer_type == 0) {
					color_no = _8bit_backbuffer_data_256x256__mtype0_dtype0[y * 256 + x];
				}
				else if (buffer_type == 1) {
					color_no = _8bit_backbuffer_data_256x256__mtype0_dtype1[y * 256 + x];
				}
				else if (buffer_type == 2) {
					color_no = _8bit_backbuffer_data_256x256__mtype1_dtype0[y * 256 + x];
				}
				else {
					color_no = _8bit_backbuffer_data_256x256__mtype1_dtype1[y * 256 + x];
				}
				DWORD color = get_bg_window_palette(color_no);

				pTextureBuffer[y * 256 + x] = color;
			}
		}

		pTexture->UnlockRect(0);

		MyDirectXDraw::draw_texture_base_leftup(myDirectXSystem, pTexture, (float)256, (float)256, 0, 0);

		pTexture->Release();
	}
#endif

	/*
	割り込みを実行したときはtrueをかえす
	*/
	bool interrupt_process() {
		uint8_t IE_value = gbx_ram.RAM[0xFFFF];
		uint8_t IF_value = gbx_ram.RAM[0xFF0F];

		if ((IE_value & 0b00000001) != 0 && (IF_value & 0b00000001) != 0) {//VBlank
			execute_interrupt(0x40);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00000001));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00000010) != 0 && (IF_value & 0b00000010) != 0) {//LCD STAT
			execute_interrupt(0x48);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00000010));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00000100) != 0 && (IF_value & 0b00000100) != 0) {//Timer
			execute_interrupt(0x50);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00000100));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00001000) != 0 && (IF_value & 0b00001000) != 0) {//Serial
			execute_interrupt(0x58);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00001000));//対応するIFのbitをクリアする

			return true;
		}
		else if ((IE_value & 0b00010000) != 0 && (IF_value & 0b00010000) != 0) {//Joypad
			execute_interrupt(0x60);

			gbx_ram.RAM[0xFF0F] &= (uint8_t)(~(0b00010000));//対応するIFのbitをクリアする

			return true;
		}

		return false;
	}

	void execute_interrupt(uint8_t handler_addr) {
		if (tmp_CPU_HALT_Flag == true) {//HALT命令で割り込みを待っていたとき
			gbx_register.PC++;//PC+1をPUSHする

			tmp_CPU_HALT_Flag = false;
		}

		push_16bit(gbx_register.PC);

		gbx_register.PC = (uint16_t)handler_addr;

		IME_Flag = false;//IMEを無効にする
	}

	void watch_key_interrupt() {
		if ((gbx_ram.RAM[0xFF00] & 0b00010000) == 0) {//方向キー

			uint8_t b_down = (key->get_input_state__GBX__(INPUT_MY_ID_DOWN) != 0) ? 1 : 0;
			uint8_t b_up = (key->get_input_state__GBX__(INPUT_MY_ID_UP) != 0) ? 1 : 0;
			uint8_t b_left = (key->get_input_state__GBX__(INPUT_MY_ID_LEFT) != 0) ? 1 : 0;
			uint8_t b_right = (key->get_input_state__GBX__(INPUT_MY_ID_RIGHT) != 0) ? 1 : 0;

			if ((b_down + b_up + b_left + b_right) != 0) {//ボタンが1つでも押されていたとき
				gbx_ram.RAM[0xFF0F] |= 0b00010000;//JPADの割り込みを要求する
			}
		}
		else if ((gbx_ram.RAM[0xFF00] & 0b00100000) == 0) {//アクションキー
			uint8_t b_start = (key->get_input_state__GBX__(INPUT_MY_ID_START) != 0) ? 1 : 0;
			uint8_t b_select = (key->get_input_state__GBX__(INPUT_MY_ID_SELECT) != 0) ? 1 : 0;
			uint8_t b_b = (key->get_input_state__GBX__(INPUT_MY_ID_B) != 0) ? 1 : 0;
			uint8_t b_a = (key->get_input_state__GBX__(INPUT_MY_ID_A) != 0) ? 1 : 0;

			if ((b_start + b_select + b_b + b_a) != 0) {//ボタンが1つでも押されていたとき
				gbx_ram.RAM[0xFF0F] |= 0b00010000;//JPADの割り込みを要求する
			}
		}
	}

public:
	GBX(const char* rom_filename, Key* key) : key(key) {
		memset(gbx_ram.RAM, 0, RAM_SIZE);

		if (read_rom_file(rom_filename) != 0) {
			goto gbx_init_error;
		}

		cpu_init();
		ppu_init();
		init_util();

		Main::ROM_loaded_flag = true;


		M_debug_printf("################################\n");
		M_debug_printf("GBX::GBX() Succeed!\n");
		M_debug_printf("################################\n");

		return;

	gbx_init_error:
		M_debug_printf("################################\n");
		M_debug_printf("GBX::GBX() Failed......\n");
		M_debug_printf("################################\n");

		MessageBox(NULL, _T("ROMの初期化の際にエラーが発生しました"), _T("ERROR"), MB_OK | MB_ICONERROR);

		FATAL_ERROR_FLAG = true;
	}

	~GBX() {
		free(ROM_bank_data_ptr);
	}

	bool get_FATAL_ERROR_FLAG() {
		return FATAL_ERROR_FLAG;
	}

	void execute_all(MyDirectXSystem* myDirectXSystem) {
		memset(backbuffer_sprite_mask, false, sizeof(bool) * GBX_WIDTH * GBX_HEIGHT);

		init_sprite_info_list();
		memset(_8bit_sprite_screen_data_160x144, 0xFF, GBX_WIDTH * GBX_HEIGHT);//スプライトのバックバッファを全部透明にする

		memset(_8bit_window_screen_data_160x144, 0xFF, GBX_WIDTH * GBX_HEIGHT);//ウインドウのバックバッファを全部透明にする
		window_backbuffer_draw_internal_flag_x = false;
		window_backbuffer_draw_internal_counter_y = 0;

		hblank_process_stage_counter = 0;
		vblank_process_stage_counter = 0;
		cpu_machine_cycle = 0;
		ppu_line_x = 0;
		ppu_line_y = 0;
		write_RAM_8bit(0xFF44, ppu_line_y);

		for (;;) {
			watch_key_interrupt();

			uint8_t instruction_code = read_RAM_8bit(gbx_register.PC);

			//=========================================================
			// 
			//M_debug_printf("=========================================================\n");
			//M_debug_printf("PC:0x%04x [命令:0x%02x] A:0x%02x, BC:0x%04x, DE:0x%04x, HL:0x%04x, Flags:0x%02x, SP:0x%04x\n",
			//	gbx_register.PC, instruction_code, gbx_register.A, gbx_register.BC, gbx_register.DE, gbx_register.HL, gbx_register.Flags, gbx_register.SP);
			//M_debug_printf("TIMA(0xFF05) = 0x%02x\n", gbx_ram.RAM[0xFF05]);
			//system("pause");
			// 
			//=========================================================

			gbx_register.PC++;

			(this->*(cpu_instruction_table[instruction_code]))();

			uint32_t instruction_machine_cycle = instruction_machine_cycle_table[instruction_code];
			if (instruction_machine_cycle == 0xDEADBEEF) {
				M_debug_printf("ERROR 未定義の命令を実行しました[PC:0x%04x, コード:0x%02x]\n", gbx_register.PC, instruction_code);
				MessageBox(NULL, _T("未定義の命令を実行しました"), _T("情報"), MB_ICONERROR);
			}

			cpu_machine_cycle += instruction_machine_cycle;//割り込みした場合はそのサイクル数も加算してある

			total_cpu_machine_cycle__div += cpu_machine_cycle;
			while (total_cpu_machine_cycle__div >= ((CPU_FREQ / 16384) / 4)) {
				((uint8_t)(gbx_ram.RAM[0xFF04]))++;//DIVレジスタを加算する

				total_cpu_machine_cycle__div -= ((CPU_FREQ / 16384) / 4);
			}

			if (get_timer_enable_flag() == true) {
				const uint64_t timer_count_freq = get_timer_count_freq();
				total_cpu_machine_cycle__tima += cpu_machine_cycle;
				while (total_cpu_machine_cycle__tima >= (timer_count_freq / 4)) {
					((uint8_t)(gbx_ram.RAM[0xFF05]))++;//Timerカウンタレジスタを加算する
					if (gbx_ram.RAM[0xFF05] == 0x00) {//Timerカウンタレジスタがオーバーフローしたとき
						gbx_ram.RAM[0xFF05] = gbx_ram.RAM[0xFF06];

						gbx_ram.RAM[0xFF0F] |= 0b00000100;
					}

					total_cpu_machine_cycle__tima -= (timer_count_freq / 4);
				}
			}

			execute_ppu_process(cpu_machine_cycle * 4 + c_cycle_mod);
			update_LCD_STAT();

			cpu_machine_cycle = 0;

			if (IME_Flag == true) {
				if (interrupt_process() == true) {
					cpu_machine_cycle += 5;//割り込みに5Mサイクルかかる
				}
			}

			if (ppu_line_y >= 154) {//Vblank終了
				create_all_256x256_backbuffer();
				draw_screenbuffer_sprite_data();

				break;
			}
		}

		if ((gbx_ram.RAM[0xFF40] & 0b10000000) != 0) {//LCD有効フラグが有効のとき
			draw_screen_bg(myDirectXSystem);//bgをwindowよりも先に呼び出すこと
			draw_screen_window(myDirectXSystem);
			draw_screen_sprite(myDirectXSystem);
		}

#ifdef GBX_EMU_DEBUG
		//_debug_draw_screen_256x256_backbuffer(myDirectXSystem, 0);
		//_debug_draw_screen_256x256_backbuffer(myDirectXSystem, 1);
		//_debug_draw_screen_256x256_backbuffer(myDirectXSystem, 2);
		//_debug_draw_screen_256x256_backbuffer(myDirectXSystem, 3);
#endif

		//M_debug_printf("End 1 frame...\n");
		
		//system("pause");
	}
};

