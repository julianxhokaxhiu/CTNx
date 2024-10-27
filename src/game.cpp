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

#include "globals.h"
#include "game.h"
#include "cocos2d.h"
#include "gamehacks.h"
#include "log.h"
#include "patch.h"

bool has_game_init = false;
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
  game_externals.application_run_102535A2 = (decltype(game_externals.application_run_102535A2))(calc_libcocos2d_address_offset(0x002535A2));
  game_externals.application_setAnimationInterval = (decltype(game_externals.application_setAnimationInterval))(calc_libcocos2d_address_offset(0x002536E7));

  game_externals.Frequency = (decltype(game_externals.PerformanceCount))(calc_game_address_offset(0x41A388));
  game_externals.PerformanceCount = (decltype(game_externals.PerformanceCount))(calc_game_address_offset(0x41A390));
  game_externals.get_time_sub_30FFA0 = (decltype(game_externals.get_time_sub_30FFA0))(calc_game_address_offset(0x30FFA0));
  game_externals.get_time_sub_30FFF0 = (decltype(game_externals.get_time_sub_30FFF0))(calc_game_address_offset(0x30FFF0));

  game_externals.get_game_string = (decltype(game_externals.get_game_string))(calc_game_address_offset(0x1B9060));
  game_externals.sub_1B92D0 = (decltype(game_externals.sub_1B92D0))(calc_game_address_offset(0x1B92D0));
  game_externals.sub_msgwindow_nextpage = (decltype(game_externals.sub_msgwindow_nextpage))(calc_game_address_offset(0x195E30));
  game_externals.sub_195C70 = (decltype(game_externals.sub_195C70))(calc_game_address_offset(0x195C70));
  game_externals.language_id = (decltype(game_externals.language_id))(calc_game_address_offset(0x3FA168));

  game_externals.log_line_sub_6F2690 = (decltype(game_externals.log_line_sub_6F2690))(calc_game_address_offset(0x2F2690));

  game_externals.engine_init_game_object_sub_69D120 = (decltype(game_externals.engine_init_game_object_sub_69D120))(calc_game_address_offset(0x29D120));
  game_externals.field_scene_object = (decltype(game_externals.field_scene_object))(calc_game_address_offset(0x41B4BC));
  game_externals.global_latency = (decltype(game_externals.global_latency))(calc_game_address_offset(0x3BA404));
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  gamehacks.processKeyboardInput(uMsg, wParam, lParam);

  Director::getInstance()->setDisplayStats(true);
  Director::getInstance()->resume();

  return game_externals.WindowProc(hwnd, uMsg, wParam, lParam);
}

BOOL qpc_custom(LARGE_INTEGER *lpPerformanceCount)
{
  BOOL ret = QueryPerformanceCounter(lpPerformanceCount);

  lpPerformanceCount->QuadPart *= gamehacks.getCurrentSpeedhack();

  return ret;
}

BOOL qpf_custom(LARGE_INTEGER *lpFrequency)
{
  BOOL ret = QueryPerformanceFrequency(lpFrequency);

  lpFrequency->QuadPart *= gamehacks.getCurrentSpeedhack();

  if (has_game_init)
  {
    float newInterval = 1.0f / (60.0f * gamehacks.getCurrentSpeedhack());
    *game_externals.field_scene_interval = newInterval;
    patch_code_float(game_externals.global_latency, newInterval);
  }

  return ret;
}

char engine_init_game_object_sub_69D120()
{
  char ret = game_externals.engine_init_game_object_sub_69D120();

  game_externals.field_scene_interval = (decltype(game_externals.field_scene_interval))(*game_externals.field_scene_object + 0x310);
  has_game_init = true;

  return ret;
}

void game_hook_init()
{
	DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  // ------------------------------------
  DetourAttach((void**)&game_externals.WindowProc, WindowProc);
  DetourAttach((void**)&game_externals.engine_init_game_object_sub_69D120, engine_init_game_object_sub_69D120);
  if (show_applog) DetourAttach((void**)&game_externals.log_line_sub_6F2690, external_debug_print);
  // ------------------------------------
  DetourTransactionCommit();

  // Custom one-shot replacement
  replace_ds_function(uint32_t(game_externals.application_run_102535A2) + 0x1A, qpc_custom);
  replace_ds_function(uint32_t(game_externals.application_run_102535A2) + 0x5E, qpc_custom);
  replace_ds_function(uint32_t(game_externals.application_setAnimationInterval) + 0xC, qpf_custom);
  replace_ds_function(uint32_t(game_externals.get_time_sub_30FFA0) + 0x14, qpc_custom);
  replace_ds_function(uint32_t(game_externals.get_time_sub_30FFF0) + 0x14, qpc_custom);
}

void game_hook_term()
{
  DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  // ------------------------------------
  DetourDetach((void**)&game_externals.WindowProc, WindowProc);
  DetourDetach((void**)&game_externals.engine_init_game_object_sub_69D120, engine_init_game_object_sub_69D120);
  if (show_applog) DetourDetach((void**)&game_externals.log_line_sub_6F2690, external_debug_print);
  // ------------------------------------
  DetourTransactionCommit();
}
