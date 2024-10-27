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

#include "game.h"
#include "gamehacks.h"
#include "globals.h"

struct game_externals game_externals;

uint32_t calc_game_address_offset(uint32_t offset)
{
	return uint32_t(game_externals.baseAddress) + offset;
}

uint32_t calc_libcocos2d_address_offset(uint32_t offset)
{
	return uint32_t(game_externals.libcocosAddress) + offset;
}

void game_data_init()
{
  game_externals.WinMain = (decltype(game_externals.WinMain))(calc_game_address_offset(0x2D8830));
  game_externals.WindowProc = (decltype(game_externals.WindowProc))(calc_libcocos2d_address_offset(0x000C2170));

  game_externals.Frequency = (decltype(game_externals.PerformanceCount))(calc_game_address_offset(0x41A388));
  game_externals.PerformanceCount = (decltype(game_externals.PerformanceCount))(calc_game_address_offset(0x41A390));
  game_externals.get_time_sub_30FFA0 = (decltype(game_externals.get_time_sub_30FFA0))(calc_game_address_offset(0x30FFA0));
  game_externals.get_time_sub_30FFF0 = (decltype(game_externals.get_time_sub_30FFF0))(calc_game_address_offset(0x30FFF0));

  game_externals.get_game_string = (decltype(game_externals.get_game_string))(calc_game_address_offset(0x1B9060));
  game_externals.sub_1B92D0 = (decltype(game_externals.sub_1B92D0))(calc_game_address_offset(0x1B92D0));
  game_externals.sub_msgwindow_nextpage = (decltype(game_externals.sub_msgwindow_nextpage))(calc_game_address_offset(0x195E30));
  game_externals.sub_195C70 = (decltype(game_externals.sub_195C70))(calc_game_address_offset(0x195C70));
  game_externals.language_id = (decltype(game_externals.language_id))(calc_game_address_offset(0x3FA168));
}

LONGLONG get_time_sub_30FFA0()
{
  LARGE_INTEGER PerformanceCount;

  QueryPerformanceCounter(&PerformanceCount);
  LONGLONG ret = PerformanceCount.QuadPart - game_externals.PerformanceCount->QuadPart;

  ret *= gamehacks.getCurrentSpeedhack();

  return ret;
}

LONGLONG get_time_sub_30FFF0()
{
  LARGE_INTEGER PerformanceCount;

  QueryPerformanceCounter(&PerformanceCount);
  LONGLONG ret = 1000000 * (PerformanceCount.QuadPart - game_externals.PerformanceCount->QuadPart) / game_externals.Frequency->QuadPart;

  ret *= gamehacks.getCurrentSpeedhack();

  return ret;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  gamehacks.processKeyboardInput(uMsg, wParam, lParam);

  return game_externals.WindowProc(hwnd, uMsg, wParam, lParam);
}

void game_hook_init()
{
	DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  // ------------------------------------
  DetourAttach((void**)&game_externals.WindowProc, WindowProc);
  DetourAttach((void**)&game_externals.get_time_sub_30FFA0, get_time_sub_30FFA0);
  DetourAttach((void**)&game_externals.get_time_sub_30FFF0, get_time_sub_30FFF0);
  // ------------------------------------
  DetourTransactionCommit();
}

void game_hook_term()
{
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  // ------------------------------------
  DetourDetach((void**)&game_externals.WindowProc, WindowProc);
  DetourDetach((void**)&game_externals.get_time_sub_30FFA0, get_time_sub_30FFA0);
  DetourDetach((void**)&game_externals.get_time_sub_30FFF0, get_time_sub_30FFF0);
  // ------------------------------------
  DetourTransactionCommit();
}
