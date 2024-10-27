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

#include <toml++/toml.h>

#include "cfg.h"
#include "globals.h"

#define CTNx_CFG_FILE "CTNx.toml"

// configuration variables with their default values
std::string external_voice_path;
std::vector<std::string> external_voice_ext;
std::string external_ambient_path;
std::vector<std::string> external_ambient_ext;
bool enable_voice_music_fade;
long external_voice_music_fade_volume;
bool enable_voice_auto_text;
bool trace_all;
bool trace_voice;
bool trace_ambient;
bool create_crash_dump;
std::string hext_patching_path;
double speedhack_step;
double speedhack_max;
double speedhack_min;
long external_audio_number_of_channels;
long external_audio_sample_rate;
long external_voice_volume;
long external_ambient_volume;

std::vector<std::string> get_string_or_array_of_strings(const toml::node_view<toml::node> &node)
{
	if (node.is_array()) {
		toml::array* a = node.as_array();
		if (a && a->is_homogeneous(toml::node_type::string)) {
			std::vector<std::string> ret;
			ret.reserve(a->size());
			for (toml::node &elem: *a) {
				ret.push_back(elem.value_or(""));
			}
			return ret;
		}
	}

	return std::vector<std::string>(1, node.value_or(""));
}

void read_cfg()
{
	toml::parse_result config;

	try
	{
		config = toml::parse_file(CTNx_CFG_FILE);
	}
	catch (const toml::parse_error &err)
	{
		PLOGW.printf("Parse error while opening the file " CTNx_CFG_FILE ". Will continue with the default settings.\n");
		PLOGW.printf("%s (Line %u Column %u)\n", err.what(), err.source().begin.line, err.source().begin.column);

		char tmp[1024]{0};
		sprintf(tmp, "%s (Line %u Column %u)\n\nWill continue with safe default settings.", err.what(), err.source().begin.line, err.source().begin.column);
		MessageBoxA(NULL, tmp, "Configuration issue detected!", MB_ICONWARNING | MB_OK);

		config = toml::parse("");
	}

	// Read config values
	external_voice_path = config["external_voice_path"].value_or("");
	external_voice_ext = get_string_or_array_of_strings(config["external_voice_ext"]);
	enable_voice_music_fade = config["enable_voice_music_fade"].value_or(false);
	external_voice_music_fade_volume = config["external_voice_music_fade_volume"].value_or(25);
	enable_voice_auto_text = config["enable_voice_auto_text"].value_or(true);
	external_ambient_path = config["external_ambient_path"].value_or("");
	external_ambient_ext = get_string_or_array_of_strings(config["external_ambient_ext"]);
	trace_all = config["trace_all"].value_or(false);
	trace_voice = config["trace_voice"].value_or(false);
	trace_ambient = config["trace_ambient"].value_or(false);
	create_crash_dump = config["create_crash_dump"].value_or(false);
	hext_patching_path = config["hext_patching_path"].value_or("");
  speedhack_step = config["speedhack_step"].value_or(0.5);
	speedhack_max = config["speedhack_max"].value_or(8.0);
	speedhack_min = config["speedhack_min"].value_or(1.0);
	external_audio_number_of_channels = config["external_audio_number_of_channels"].value_or(2);
	external_audio_sample_rate = config["external_audio_sample_rate"].value_or(44100);
	external_voice_volume = config["external_voice_volume"].value_or(-1);
	external_ambient_volume = config["external_ambient_volume"].value_or(-1);

	// Normalize voice music fade volume
	if (external_voice_music_fade_volume < 0) external_voice_music_fade_volume = 0;
	if (external_voice_music_fade_volume > 100) external_voice_music_fade_volume = 100;

	// #############
	// SAFE DEFAULTS
	// #############

	if (hext_patching_path.empty())
	{
		hext_patching_path = "hext";
	}

	switch(*game_externals.language_id)
	{
		case GAME_LANGUAGE_JA:
			hext_patching_path += "/ja";
			break;
		case GAME_LANGUAGE_EN:
			hext_patching_path += "/en";
			break;
		case GAME_LANGUAGE_DE:
			hext_patching_path += "/de";
			break;
		case GAME_LANGUAGE_IT:
			hext_patching_path += "/it";
			break;
		case GAME_LANGUAGE_FR:
			hext_patching_path += "/fr";
			break;
		case GAME_LANGUAGE_ZH_HANS:
			hext_patching_path += "/zh-hans";
			break;
		case GAME_LANGUAGE_ZH_HANT:
			hext_patching_path += "/zh-hant";
			break;
		case GAME_LANGUAGE_KO:
			hext_patching_path += "/ko";
			break;
	}

	// EXTERNAL VOICE PATH
	if (external_voice_path.empty())
		external_voice_path = "voice";

	// EXTERNAL VOICE EXTENSION
	if (external_voice_ext.empty() || external_voice_ext.front().empty())
		external_voice_ext = std::vector<std::string>(1, "ogg");

	// EXTERNAL AMBIENT PATH
	if (external_ambient_path.empty())
		external_ambient_path = "ambient";

	// EXTERNAL AMBIENT EXTENSION
	if (external_ambient_ext.empty() || external_ambient_ext.front().empty())
		external_ambient_ext = std::vector<std::string>(1, "ogg");

	// AUDIO NUMBER OF CHANNELS
	if (external_audio_number_of_channels < 0)
		external_audio_number_of_channels = 0;
	else if (external_audio_number_of_channels % 2 != 0)
		// Round to the previous even number
		external_audio_number_of_channels--;

	// AUDIO SAMPLE RATE
	if (external_audio_sample_rate < 0)
		external_audio_sample_rate = 0;

	// VOLUME
	if (external_voice_volume < 0) external_voice_volume = 0;
	if (external_ambient_volume < 0) external_ambient_volume = 0;
	if (external_voice_volume > 100) external_voice_volume = 100;
	if (external_ambient_volume > 100) external_ambient_volume = 100;
}
