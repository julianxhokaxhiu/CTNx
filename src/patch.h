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

#pragma once

#include <stdint.h>
#include <windows.h>

uint32_t get_relative_call(uint32_t base, uint32_t offset);
uint32_t get_absolute_value(uint32_t base, uint32_t offset);
void patch_code_char(uint32_t offset, char r);
void patch_code_byte(uint32_t offset, unsigned char r);
void patch_code_word(uint32_t offset, WORD r);
void patch_code_short(uint32_t offset, short r);
void patch_code_dword(uint32_t offset, DWORD r);
void patch_code_int(uint32_t offset, int r);
void patch_code_uint(uint32_t offset, uint32_t r);
void patch_code_float(uint32_t offset, float r);
void patch_code_double(uint32_t offset, double r);

template<typename T>
void patch_multiply_code(uint32_t offset, int multiplier)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(T), PAGE_EXECUTE_READWRITE, &dummy);

	*(T *)offset = (*(T *)offset) * (T)multiplier;

    // TODO Add assertion
}

template<typename T>
void patch_divide_code(uint32_t offset, int multiplier)
{
	DWORD dummy;

	VirtualProtect((void *)offset, sizeof(T), PAGE_EXECUTE_READWRITE, &dummy);

	*(T *)offset = (*(T *)offset) / (T)multiplier;

    // TODO Add assertion
}

void memcpy_code(uint32_t offset, void *data, uint32_t size);
void memset_code(uint32_t offset, uint32_t val, uint32_t size);

void* member_func_to_ptr(char i, ...);
