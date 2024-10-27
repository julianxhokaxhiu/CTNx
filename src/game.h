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
#include <string>

#define GAME_LANGUAGE_JA 0
#define GAME_LANGUAGE_EN 1
#define GAME_LANGUAGE_DE 2
#define GAME_LANGUAGE_IT 3
#define GAME_LANGUAGE_ES 4
#define GAME_LANGUAGE_FR 5
#define GAME_LANGUAGE_ZH_HANS 6
#define GAME_LANGUAGE_ZH_HANT 7
#define GAME_LANGUAGE_KO 8

struct MsgWindow {
  BYTE gap0[0x2C0];
  int currentLineNoMaybe;
  int pageStartLineNoMaybe;
};

struct field_object {
  DWORD dword0;
  DWORD dword4;
  DWORD dword8;
  DWORD dwordC;
  DWORD dword10;
  DWORD dword14;
  DWORD dword18;
  DWORD dword1C;
  BYTE byte20;
  BYTE gap0[6375];
  std::string defaultCharacterNames[7];
  BYTE gap1[62932];
  BYTE byte10F84;
  BYTE gap2[1];
  WORD word10F86;
  BYTE gap3[72];
  DWORD dword10FD0;
  BYTE gap4[56];
  DWORD dword1100C;
  BYTE gap5[56];
  DWORD dword11048;
  BYTE gap6[100];
  DWORD dword110B0;
  BYTE gap7[4108];
  int currentFieldId;
  BYTE gap8[4488];
  int activePartyMemberIds[3];
  int inactivePartyMemberIds[4];
  BYTE gap9[20];
  DWORD dword1327C;
  BYTE gapA[728];
  DWORD dword13558;
  BYTE gap[38828];
};

struct game_externals
{
  // Core exe address
  HMODULE baseAddress;
  HMODULE libcocosAddress;
  // Game WinMain
  int (WINAPI *WinMain)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd);
  LRESULT (CALLBACK *WindowProc)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  int (__thiscall *application_run_102535A2)();
  void (__thiscall *application_setAnimationInterval)(float);
  // Game Functions
  LONGLONG (*get_time_sub_30FFA0)();
  LONGLONG (*get_time_sub_30FFF0)();
  std::string* (__thiscall *get_game_string)(void*, void*, int, int);
  std::string* (__thiscall *sub_1B92D0)(void*, void*, int, int);
  void (__thiscall *sub_msgwindow_nextpage)(MsgWindow*);
  void (__thiscall *sub_195C70)(MsgWindow*, int);
  int (*log_line_sub_6F2690)(int, char*, ...);
  char (*engine_init_game_object_sub_69D120)();
  // Game Data
  LARGE_INTEGER* PerformanceCount;
  LARGE_INTEGER* Frequency;
  field_object* field_object;
  DWORD* language_id;
  DWORD* field_scene_object;
  float* field_scene_interval;
  DWORD global_latency;
};

uint32_t calc_game_address_offset(uint32_t offset);
void game_data_init();
void game_hook_init();
void game_hook_term();
