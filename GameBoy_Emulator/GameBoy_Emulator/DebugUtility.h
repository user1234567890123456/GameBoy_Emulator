#pragma once

#include <stdio.h>


#include "SystemDefine.h"

#ifdef GAMEBOY_EMULATOR_DEBUG
#define M_debug_printf(...) printf(__VA_ARGS__)
#else
#define M_debug_printf(...)
#endif


class DebugUtility
{
public:
private:
	DebugUtility();
	~DebugUtility();
};
