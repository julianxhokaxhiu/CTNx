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

#include <mimalloc-new-delete.h>
#include <windows.h>
#include <toml++/toml.h>

#include <detours/detours.h>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>
#include "plog.formatter.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#if defined(__cplusplus)
}
#endif

#include "cfg.h"
#include "game.h"

#define BASEDIR_LENGTH 512
extern char basedir[BASEDIR_LENGTH];

extern HMODULE game_baseAddress;
extern struct game_externals game_externals;

uint32_t noop();
void on_game_close();
void ffmpeg_log_callback(void* ptr, int level, const char* fmt, va_list vl);
void CTNx_log_current_pc_specs();
