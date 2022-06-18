#pragma once

#include <stdint.h>
#include <windows.h>

class Main
{
private:
	Main() {}
	~Main() {}
public:
	static bool ready_read_ROM_flag;
	static TCHAR ROMFilePath[MAX_PATH];

	static bool ROM_loaded_flag;
	static uint8_t Cartridge_Type;
	static uint8_t PGM_size;
	static uint8_t RAM_type;

	static bool Show_FPS_Flag;
};
