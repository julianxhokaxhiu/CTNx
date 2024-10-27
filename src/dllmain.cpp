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

#include "audio.h"
#include "crashdump.h"
#include "exports.h"
#include "hext.h"
#include "game.h"
#include "gamehacks.h"
#include "utils.h"
#include "voice.h"

#include <windowsx.h>
#include <dwmapi.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <psapi.h>
#include <mmsystem.h>
#include <malloc.h>
#include <ddraw.h>
#include <filesystem>

void on_game_hook()
{
  // install crash handler
  SetUnhandledExceptionFilter(ExceptionHandler);

  CTNx_log_current_pc_specs();

  QueryPerformanceFrequency(&system_frequency);

  // Disable vsync
  glfwSwapInterval(enable_vsync);

  // start hooking
  game_hook_init();
  voice_init();

  // gamehacks
  gamehacks.init();

  // apply hext patching
  hextPatcher.applyAll();
}

void on_game_close()
{
  voice_term();
  game_hook_term();

  // Cleanup the audio device
  nxAudioEngine.cleanup();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  if (DetourIsHelperProcess()) return TRUE;

  if (fdwReason == DLL_PROCESS_ATTACH)
  {
    // Push the limit of how many files we can open at the same time to the maximum available on Windows
    // See https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/setmaxstdio?view=msvc-160#remarks
    _setmaxstdio(8192);

    SetProcessDPIAware();

    GetCurrentDirectoryA(BASEDIR_LENGTH, basedir);

    // Setup logging layer
    remove(APP_RELEASE_NAME ".log");
    plog::init<plog::CTNxFormatter>(plog::verbose, APP_RELEASE_NAME ".log");

    // prevent screensavers
    SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED);

    PLOGI.printf("CTNx driver version " APP_RELEASE_VERSION "\n");

    // init game data
    game_externals.baseAddress = GetModuleHandle(nullptr);
    game_externals.libcocosAddress = GetModuleHandle("libcocos2d");
    game_data_init();

    // print detected language
    switch(*game_externals.language_id)
    {
      case GAME_LANGUAGE_JA:
        PLOGI.printf("Detected language: ja\n");
        break;
      case GAME_LANGUAGE_EN:
        PLOGI.printf("Detected language: en\n");
        break;
      case GAME_LANGUAGE_DE:
        PLOGI.printf("Detected language: de\n");
        break;
      case GAME_LANGUAGE_IT:
        PLOGI.printf("Detected language: it\n");
        break;
      case GAME_LANGUAGE_FR:
        PLOGI.printf("Detected language: fr\n");
        break;
      case GAME_LANGUAGE_ZH_HANS:
        PLOGI.printf("Detected language: zh-hans\n");
        break;
      case GAME_LANGUAGE_ZH_HANT:
        PLOGI.printf("Detected language: zh-hant\n");
        break;
      case GAME_LANGUAGE_KO:
        PLOGI.printf("Detected language: ko\n");
        break;
    }

    read_cfg();

    // Hook WinMain and allow us to run some custom init code before the game starts
    static decltype(game_externals.WinMain) detour = [](HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) -> int
    {
      DetourTransactionBegin();
      DetourUpdateThread(GetCurrentThread());
      // ------------------------------------
      DetourDetach((void**)&game_externals.WinMain, detour);
      // ------------------------------------
      DetourTransactionCommit();

      on_game_hook();

      // continue execution
      return game_externals.WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    };

    DisableThreadLibraryCalls(hinstDLL);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    // ------------------------------------
    DetourAttach((void**)&game_externals.WinMain, detour);
    // ------------------------------------
    DetourTransactionCommit();
  }
  else if (fdwReason == DLL_PROCESS_DETACH)
  {
    on_game_close();
  }

  return TRUE;
}
