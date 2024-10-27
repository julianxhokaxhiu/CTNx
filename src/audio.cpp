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
#include "utils.h"

#if defined(__cplusplus)
extern "C" {
#endif

#include <libvgmstream/vgmstream.h>

#if defined(__cplusplus)
}
#endif

NxAudioEngine nxAudioEngine;

// PRIVATE

void NxAudioEngine::loadConfig()
{
	char _fullpath[MAX_PATH];

	for (int idx = NxAudioEngineLayer::NXAUDIOENGINE_VOICE; idx <= NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT; idx++)
	{
		NxAudioEngineLayer type = NxAudioEngineLayer(idx);

		switch (type)
		{
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_voice_path.c_str());
			if (trace_all || trace_voice) PLOGD.printf("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			sprintf(_fullpath, "%s/%s/config.toml", basedir, external_ambient_path.c_str());
			if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: %s\n", __func__, _fullpath);
			break;
		}

		try
		{
			nxAudioEngineConfig[type] = toml::parse_file(_fullpath);
		}
		catch (const toml::parse_error &err)
		{
			PLOGW.printf("Parse error while opening the file %s. Will continue with the default settings.\n", _fullpath);
			PLOGW.printf("%s (Line %u Column %u)\n", err.what(), err.source().begin.line, err.source().begin.column);

			nxAudioEngineConfig[type] = toml::parse("");
		}
	}
}

bool NxAudioEngine::getFilenameFullPath(char *_out, const char* _key, NxAudioEngineLayer _type)
{
	std::vector<std::string> extensions;

	switch(_type)
	{
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			extensions = external_voice_ext;
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			extensions = external_ambient_ext;
			break;
	}

	for (const std::string &extension: extensions) {
		switch (_type)
		{
		case NxAudioEngineLayer::NXAUDIOENGINE_VOICE:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_voice_path.c_str(), _key, extension.c_str());
			break;
		case NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT:
			sprintf(_out, "%s/%s/%s.%s", basedir, external_ambient_path.c_str(), _key, extension.c_str());
			break;
		}

		if (fileExists(_out)) {
			return true;
		}
	}

	return false;
}

bool NxAudioEngine::fileExists(const char* filename)
{
	bool ret = ::fileExists(filename);

	if (!ret && (trace_all || trace_voice || trace_ambient))
		PLOGW.printf("NxAudioEngine::%s: Could not find file %s\n", __func__, filename);

	return ret;
}

// PUBLIC

bool NxAudioEngine::init()
{
	if (_engine.init(SoLoud::Soloud::CLIP_ROUNDOFF, SoLoud::Soloud::AUTO, external_audio_sample_rate, SoLoud::Soloud::AUTO, external_audio_number_of_channels) == 0)
	{
		_engineInitialized = true;

		PLOGI.printf("NxAudioEngine initialized: channels=%u,sample_rate=%u\n", _engine.getBackendChannels(), _engine.getBackendSamplerate());

		// 100 -> LOG_LEVEL_ALL: https://github.com/vgmstream/vgmstream/blob/4cda04d02595b381dc8cf98ec39e771c80987d18/src/util/log.c#L20
		if (trace_all || trace_ambient || trace_voice) vgm_log_set_callback(NULL, 100, 0, NxAudioEngineVgmstreamCallback);

		loadConfig();

		return true;
	}

	return false;
}

void NxAudioEngine::flush()
{
	_engine.stopAll();

	for (int slot = 0; slot < _voiceMaxSlots; slot++)
	{
		_currentVoice[slot] = NxAudioEngineVoice();
	}

	_currentAmbient = NxAudioEngineAmbient();
}

void NxAudioEngine::cleanup()
{
	_engine.deinit();
}

// Voice
bool NxAudioEngine::canPlayVoice(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
}

bool NxAudioEngine::playVoice(const char* name, int slot, float volume)
{
	char filename[MAX_PATH];

	bool exists = false;

	_currentVoice[slot].volume = volume * getVoiceMasterVolume();

	std::string _name(name);
	// TOML doesn't like the / char as key, replace it with - ( one of the valid accepted chars )
	replaceAll(_name, '/', '-');

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_VOICE][_name];
	if (node)
	{
		// Set volume for the current track
		toml::node *trackVolume = node["volume"].as_integer();
		if (trackVolume)
		{
			_currentVoice[slot].volume = (trackVolume->value_or(100) / 100.0f) * getVoiceMasterVolume();
		}

		// Shuffle Voice playback, if any entry found for the current id
		toml::array *shuffleNames = node["shuffle"].as_array();
		if (shuffleNames && !shuffleNames->empty() && shuffleNames->is_homogeneous(toml::node_type::string))
		{
			auto _newName = shuffleNames->get(getRandomInt(0, shuffleNames->size() - 1));

			exists = getFilenameFullPath(filename, _newName->value_or(name), NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
		}

		// Sequentially playback new voice items, if any entry found for the current id
		toml::array *sequentialNames = node["sequential"].as_array();
		if (sequentialNames && !sequentialNames->empty() && sequentialNames->is_homogeneous(toml::node_type::string))
		{
			if (_voiceSequentialIndexes.find(name) == _voiceSequentialIndexes.end() || _voiceSequentialIndexes[name] >= sequentialNames->size())
				_voiceSequentialIndexes[name] = 0;

			auto _newName = sequentialNames->get(_voiceSequentialIndexes[name]);

			_voiceSequentialIndexes[name]++;

			exists = getFilenameFullPath(filename, _newName->value_or(name), NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
		}
	}

	// If none of the previous configurations worked, load the default one as last tentative
	if (!exists) {
		exists = getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_VOICE);
	}

	if (trace_all || trace_voice) PLOGD.printf("NxAudioEngine::%s: slot[%d] %s exists=%d\n", __func__, slot, filename, exists);

	if (exists)
	{
		// Stop any previously playing voice
		if (_engine.isValidVoiceHandle(_currentVoice[slot].handle))
		{
			_engine.stop(_currentVoice[slot].handle);

			delete _currentVoice[slot].stream;

			_currentVoice[slot].handle = NXAUDIOENGINE_INVALID_HANDLE;
		}

		_currentVoice[slot].stream = new SoLoud::VGMStream();

		SoLoud::result res = _currentVoice[slot].stream->load(filename);
		if (res != SoLoud::SO_NO_ERROR) {
			PLOGE.printf("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
			delete _currentVoice[slot].stream;
			return false;
		}

		_currentVoice[slot].handle = _engine.play(*_currentVoice[slot].stream, _currentVoice[slot].volume);

		return _engine.isValidVoiceHandle(_currentVoice[slot].handle);
	}
	else
		return false;
}

void NxAudioEngine::stopVoice(int slot, double time)
{
	SoLoud::handle handle = _currentVoice[slot].handle;

	if (trace_all || trace_voice) PLOGD.printf("NxAudioEngine::%s: slot=%d time=%lf handle=%X\n", __func__, slot, time, handle);

	if (!_engine.isValidVoiceHandle(handle))
	{
		return;
	}

	if (time > 0.0)
	{
		_engine.fadeVolume(handle, 0, time);
		_engine.scheduleStop(handle, time);
	}
	else
	{
		_engine.stop(handle);
	}
}

void NxAudioEngine::pauseVoice(int slot, double time)
{
	if (time > 0.0)
	{
		_engine.fadeVolume(_currentVoice[slot].handle, 0, time);
		_engine.schedulePause(_currentVoice[slot].handle, time);
	}
	else
	{
		_engine.setPause(_currentVoice[slot].handle, true);
	}
}

void NxAudioEngine::resumeVoice(int slot, double time)
{
	if (time > 0.0)
	{
		_engine.setPause(_currentVoice[slot].handle, false);
		_engine.fadeVolume(_currentVoice[slot].handle, _currentVoice[slot].volume, time);
	}
	else
	{
		_engine.setVolume(_currentVoice[slot].handle, _currentVoice[slot].volume);
		_engine.setPause(_currentVoice[slot].handle, false);
	}
}

bool NxAudioEngine::isVoicePlaying(int slot)
{
	return _engine.isValidVoiceHandle(_currentVoice[slot].handle) && !_engine.getPause(_currentVoice[slot].handle);
}

void NxAudioEngine::setVoiceMaxSlots(int slot)
{
	_voiceMaxSlots = slot;
}

float NxAudioEngine::getVoiceMasterVolume()
{
	return _voiceMasterVolume < 0.0f ? 1.0f : _voiceMasterVolume;
}

void NxAudioEngine::setVoiceMasterVolume(float volume, double time)
{
	_voiceMasterVolume = volume;
}

// Ambient
bool NxAudioEngine::canPlayAmbient(const char* name)
{
	char filename[MAX_PATH];

	return getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
}

bool NxAudioEngine::playAmbient(const char* name, float volume, double time)
{
	char filename[MAX_PATH];
	bool exists = false;

	// Reset state
	_currentAmbient.fade_in = 0.0f;
	_currentAmbient.fade_out = 0.0f;
	_currentAmbient.volume = volume * getAmbientMasterVolume();

	auto node = nxAudioEngineConfig[NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT][name];
	if (node)
	{
		// Shuffle Ambient playback, if any entry found for the current id
		toml::array *shuffleIds = node["shuffle"].as_array();
		if (shuffleIds && !shuffleIds->empty() && shuffleIds->is_homogeneous(toml::node_type::string))
		{
			auto _newName = shuffleIds->get(getRandomInt(0, shuffleIds->size() - 1));

			exists = getFilenameFullPath(filename, _newName->value_or(""), NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
		}

		// Sequentially playback new Ambient ids, if any entry found for the current id
		toml::array *sequentialIds = node["sequential"].as_array();
		if (sequentialIds && !sequentialIds->empty() && sequentialIds->is_homogeneous(toml::node_type::string))
		{
			// If the key doesn't exist already, add it
			if (_ambientSequentialIndexes.count(name) == 0) _ambientSequentialIndexes[name] = NULL;

			if (_ambientSequentialIndexes.find(name) == _ambientSequentialIndexes.end() || _ambientSequentialIndexes[name] >= sequentialIds->size())
				_ambientSequentialIndexes[name] = 0;

			auto _newName = sequentialIds->get(_ambientSequentialIndexes[name]);

			_ambientSequentialIndexes[name]++;

			exists = getFilenameFullPath(filename, _newName->value_or(""), NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
		}

		// Fade In time for this track, if configured
		toml::node *fadeInTime = node["fade_in"].as_floating_point();
		if (fadeInTime)
		{
			_currentAmbient.fade_in = fadeInTime->value_or(0.0f);

			time = _currentAmbient.fade_in;
		}

		// Fade Out time for this track, if configured
		toml::node *fadeOutTime = node["fade_out"].as_floating_point();
		if (fadeOutTime)
		{
			_currentAmbient.fade_out = fadeOutTime->value_or(0.0f);
		}

		// Set volume for the current ambient
		toml::node *ambientVolume = node["volume"].as_integer();
		if (ambientVolume)
		{
			_currentAmbient.volume = (ambientVolume->value_or(100) / 100.0f) * getAmbientMasterVolume();
		}
	}

	// If none of the previous configurations worked, load the default one as last tentative
	if (!exists) {
		exists = getFilenameFullPath(filename, name, NxAudioEngineLayer::NXAUDIOENGINE_AMBIENT);
	}

	if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: %s exists=%d handle=%X\n", __func__, filename, exists, _currentAmbient.handle);

	if (exists)
	{
		// Stop any previously playing ambient
		if (_engine.isValidVoiceHandle(_currentAmbient.handle))
		{
			_engine.stop(_currentAmbient.handle);

			delete _currentAmbient.stream;

			_currentAmbient.handle = NXAUDIOENGINE_INVALID_HANDLE;
		}

		_currentAmbient.stream = new SoLoud::VGMStream();

		SoLoud::result res = _currentAmbient.stream->load(filename);
		if (res != SoLoud::SO_NO_ERROR) {
			PLOGE.printf("NxAudioEngine::%s: Cannot load %s with vgmstream ( SoLoud error: %u )\n", __func__, filename, res);
			delete _currentAmbient.stream;
			return false;
		}

		_currentAmbient.handle = _engine.play(*_currentAmbient.stream, time > 0.0f ? 0.0f : _currentAmbient.volume, 0.0f, time > 0.0f);

		if (time > 0.0f) resumeAmbient(time);

		return _engine.isValidVoiceHandle(_currentAmbient.handle);
	}
	else
		return false;
}

void NxAudioEngine::stopAmbient(double time)
{
	if (_currentAmbient.fade_out > 0.0f)
	{
		time = _currentAmbient.fade_out;

		if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.fadeVolume(_currentAmbient.handle, 0, time);
		_engine.scheduleStop(_currentAmbient.handle, time);
	}
	else
	{
		_engine.stop(_currentAmbient.handle);
	}
}

void NxAudioEngine::pauseAmbient(double time)
{
	if (_currentAmbient.fade_out > 0.0f)
	{
		time = _currentAmbient.fade_out;

		if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.fadeVolume(_currentAmbient.handle, 0, time);
		_engine.schedulePause(_currentAmbient.handle, time);
	}
	else
	{
		_engine.setPause(_currentAmbient.handle, true);
	}
}

void NxAudioEngine::resumeAmbient(double time)
{
	if (_currentAmbient.fade_in > 0.0f)
	{
		time = _currentAmbient.fade_in;

		if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: time=%f ( overridden through config.toml )\n", __func__, time);
	}
	else if (trace_all || trace_ambient) PLOGD.printf("NxAudioEngine::%s: time=%f\n", __func__, time);

	if (time > 0.0)
	{
		_engine.setPause(_currentAmbient.handle, false);
		_engine.fadeVolume(_currentAmbient.handle, _currentAmbient.volume, time);
	}
	else
	{
		_engine.setVolume(_currentAmbient.handle, _currentAmbient.volume);
		_engine.setPause(_currentAmbient.handle, false);
	}
}

bool NxAudioEngine::isAmbientPlaying()
{
	return _engine.isValidVoiceHandle(_currentAmbient.handle) && !_engine.getPause(_currentAmbient.handle);
}

float NxAudioEngine::getAmbientMasterVolume()
{
	return _ambientMasterVolume < 0.0f ? 1.0f : _ambientMasterVolume;
}

void NxAudioEngine::setAmbientMasterVolume(float volume, double time)
{
	_ambientMasterVolume = volume;
}
