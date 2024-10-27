/****************************************************************************/
//    Copyright (C) 2024 Julian Xhokaxhiu                                   //
//                                                                          //
//    This file is part of CTNx                                             //
//                                                                          //
//    CTNx is free software: you can redistribute it and/or modify          //
//    it under the terms of the GNU General Public License as published by  //
//    the Free Software Foundation, either version 3 of the License         //
//                                                                          //
//    CTNx is distributed in the hope that it will be useful,               //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of        //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         //
//    GNU General Public License for more details.                          //
/****************************************************************************/

#include "patch.h"

#include <windows.h>
#include <stdint.h>

#include "crashdump.h"

uint8_t check_is_call(const char *name, uint32_t base, uint32_t offset, uint16_t instruction)
{
	if ((instruction & 0xFF) != 0xE8 && (instruction & 0xFF) != 0xE9 && instruction != 0x15FF)
	{
		// Warning to diagnose errors faster
		PLOGW.printf("%s: Unrecognized call/jmp instruction at 0x%X + 0x%X (0x%X): 0x%X\n", name, base, offset, base + offset, instruction);
	}

	return instruction == 0x15FF ? 2 : 1;
}

void replace_ds_function(uint32_t offset, void* func)
{
	DWORD dummy;

	VirtualProtect((void*)offset, 6, PAGE_EXECUTE_READWRITE, &dummy);

	*(unsigned char*)offset = 0x90;
	*(unsigned char*)(offset + 1) = 0xE8;
	*(uint32_t*)(offset + 2) = ((uint32_t)func - offset) - 6;
}

uint32_t get_relative_call(uint32_t base, uint32_t offset)
{
	uint16_t instruction = *((uint16_t *)(base + offset));

	uint8_t size = check_is_call(__func__, base, offset, instruction);

	uint32_t ret = base + *((uint32_t *)(base + offset + size)) + offset + 4 + size;

	return ret;
}

uint32_t get_absolute_value(uint32_t base, uint32_t offset)
{
	return *((uint32_t *)(base + offset));
}

void patch_code_byte(uint32_t offset, unsigned char r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(unsigned char *)offset = r;
}

void patch_code_char(uint32_t offset, char r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(char *)offset = r;
}

void patch_code_word(uint32_t offset, WORD r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(WORD *)offset = r;
}

void patch_code_short(uint32_t offset, short r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(short *)offset = r;
}

void patch_code_dword(uint32_t offset, DWORD r)
{
	DWORD dummy;

	VirtualProtect((void*)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(DWORD*)offset = r;
}

void patch_code_int(uint32_t offset, int r)
{
	DWORD dummy;

	VirtualProtect((void*)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(int*)offset = r;
}

void patch_code_uint(uint32_t offset, uint32_t r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(uint32_t *)offset = r;
}

void patch_code_float(uint32_t offset, float r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(float *)offset = r;
}

void patch_code_double(uint32_t offset, double r)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(r), PAGE_EXECUTE_READWRITE, &dummy);

	*(double *)offset = r;
}

void memcpy_code(uint32_t offset, void *data, uint32_t size)
{
	DWORD dummy;

	VirtualProtect((void *)offset, size, PAGE_EXECUTE_READWRITE, &dummy);

	memcpy((void *)offset, data, size);
}

void memset_code(uint32_t offset, uint32_t val, uint32_t size)
{
	DWORD dummy;

	VirtualProtect((void *)offset, size, PAGE_EXECUTE_READWRITE, &dummy);

	memset((void *)offset, val, size);
}

// From https://stackoverflow.com/a/21636483
void* member_func_to_ptr(char i, ...)
{
    va_list v;
    va_start(v,i);
    void* ret = va_arg(v, void*);
    va_end(v);
    return ret;
}
