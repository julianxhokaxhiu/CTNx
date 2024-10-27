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

#include <stack>
#include <string>
#include <vector>
#include <unordered_map>
#include <soloud.h>
#include "audio/vgmstream/vgmstream.h"
#include "globals.h"

#define NXAUDIOENGINE_INVALID_HANDLE 0xfffff000

static void NxAudioEngineVgmstreamCallback(int level, const char* str)
{
  PLOGI.printf("VGMStream [L%d]: %s\n", level, str);
}

class NxAudioEngine
{
public:
	struct NxAudioEngineVoice
	{
		NxAudioEngineVoice() :
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			stream(nullptr),
			volume(1.0f) {}
		SoLoud::handle handle;
		SoLoud::VGMStream* stream;
		float volume;
	};

	struct NxAudioEngineAmbient
	{
		NxAudioEngineAmbient() :
			handle(NXAUDIOENGINE_INVALID_HANDLE),
			stream(nullptr),
			volume(1.0f),
			fade_in(0.0f),
			fade_out(0.0f) {}
		SoLoud::handle handle;
		SoLoud::VGMStream* stream;
		float volume;
		double fade_in;
		double fade_out;
	};

private:
	enum NxAudioEngineLayer
	{
		NXAUDIOENGINE_VOICE,
		NXAUDIOENGINE_AMBIENT
	};

	bool _engineInitialized = false;
	SoLoud::Soloud _engine;

	// VOICE
	short _voiceMaxSlots = 0;
	float _voiceMasterVolume = -1.0f;
	std::map<int, NxAudioEngineVoice> _currentVoice;
	std::map<std::string, int> _voiceSequentialIndexes;

	// AMBIENT
	float _ambientMasterVolume = -1.0f;
	std::map<std::string, int> _ambientSequentialIndexes;
	NxAudioEngineAmbient _currentAmbient;

	// MISC
	// Returns false if the file does not exist
	bool getFilenameFullPath(char *_out, const char* _key, NxAudioEngineLayer _type);

	bool fileExists(const char* filename);

	// CFG
	std::unordered_map<NxAudioEngineLayer,toml::parse_result> nxAudioEngineConfig;

	void loadConfig();

public:

	bool init();
	void flush();
	void cleanup();

	// Voice
	bool canPlayVoice(const char* name);
	bool playVoice(const char* name, int slot = 0, float volume = 1.0f);
	void stopVoice(int slot = 0, double time = 0);
	void pauseVoice(int slot = 0, double time = 0);
	void resumeVoice(int slot = 0, double time = 0);
	bool isVoicePlaying(int slot = 0);
	void setVoiceMaxSlots(int slot);
	float getVoiceMasterVolume();
	void setVoiceMasterVolume(float volume, double time = 0);

	// Ambient
	bool canPlayAmbient(const char* name);
	bool playAmbient(const char* name, float volume = 1.0f, double time = 0);
	void stopAmbient(double time = 0);
	void pauseAmbient(double time = 0);
	void resumeAmbient(double time = 0);
	bool isAmbientPlaying();
	float getAmbientMasterVolume();
	void setAmbientMasterVolume(float volume, double time = 0);
};

extern NxAudioEngine nxAudioEngine;
