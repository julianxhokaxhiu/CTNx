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

#include <string>
#include <vector>

#define RENDERER_BACKEND_AUTO 0
#define RENDERER_BACKEND_OPENGL 1
// Slot 2 used to be used for DIRECT3D9 but is not more officially supported by bgfx.
// Preserve the slot numbers as they are to ensure compatibility with existing installations and tooling.
#define RENDERER_BACKEND_DIRECT3D11 3
#define RENDERER_BACKEND_DIRECT3D12 4
#define RENDERER_BACKEND_VULKAN 5

#define FPS_LIMITER_ORIGINAL 0
#define FPS_LIMITER_DEFAULT 1
#define FPS_LIMITER_30FPS 2
#define FPS_LIMITER_60FPS 3

#define GAME_LIGHTING_ORIGINAL 0
#define GAME_LIGHTING_PER_VERTEX 1
#define GAME_LIGHTING_PER_PIXEL 2

extern std::string external_voice_path;
extern std::vector<std::string> external_voice_ext;
extern std::string external_ambient_path;
extern std::vector<std::string> external_ambient_ext;
extern bool enable_voice_music_fade;
extern long external_voice_music_fade_volume;
extern bool enable_voice_auto_text;
extern bool trace_all;
extern bool trace_voice;
extern bool trace_ambient;
extern bool create_crash_dump;
extern std::string hext_patching_path;
extern double speedhack_step;
extern double speedhack_max;
extern double speedhack_min;
extern long external_audio_number_of_channels;
extern long external_audio_sample_rate;
extern long external_voice_volume;
extern long external_ambient_volume;

void read_cfg();
