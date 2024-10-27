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

#include <regex>
#include <hwinfo/hwinfo.h>
#include "globals.h"
#include "crashdump.h"
#include "wine.h"

// install directory for the current game
char basedir[BASEDIR_LENGTH];

uint32_t noop() { return 0; }

void ffmpeg_log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
	char msg[4 * 1024]; // 4K
	static int print_prefix = 1;

	av_log_format_line(ptr, level, fmt, vl, msg, sizeof(msg), &print_prefix);

	switch (level) {
	case AV_LOG_VERBOSE:
	case AV_LOG_DEBUG: if (trace_all) PLOGD.printf(msg); break;
	case AV_LOG_INFO:
	case AV_LOG_WARNING: if (trace_all) PLOGI.printf(msg); break;
	case AV_LOG_ERROR:
	case AV_LOG_FATAL:
	case AV_LOG_PANIC: PLOGE.printf(msg); break;
	}

	if (level <= AV_LOG_ERROR) {
		CTNxStackWalker sw;
		sw.ShowCallstack();
	}
}

void CTNx_log_current_pc_specs()
{
	// Start report of PC specs
	PLOGI.printf("--- PC SPECS ---\n");

	// CPU
	auto cpus = hwinfo::getAllCPUs();
	for (const auto& cpu : cpus) {
		PLOGI.printf("   CPU: %s\n", cpu.modelName().c_str());
	}

	// GPU
	auto gpus = hwinfo::getAllGPUs();
	uint16_t gpuidx = 0;
	for (auto& gpu : gpus) {
		uint16_t vendorId = std::stoi(gpu.vendor_id(), 0, 16), deviceId = std::stoi(gpu.device_id(), 0, 16);
		PLOGI.printf("   GPU #%u: %s (%dMB) - Driver: %s\n", gpuidx, gpu.name().c_str(), (int)(gpu.memory_Bytes() / 1024.0 / 1024.0), gpu.driverVersion().c_str());
		gpuidx++;
	}

	// RAM
	hwinfo::Memory memory;
	PLOGI.printf("   RAM: %dMB/%dMB (Free: %dMB)\n", (int)((memory.total_Bytes() - memory.free_Bytes()) / 1024.0 / 1024.0), (int)(memory.total_Bytes() / 1024.0 / 1024.0), (int)(memory.free_Bytes() / 1024.0 / 1024.0));

	// OS
	hwinfo::OS os;
	PLOGI.printf("    OS: %s %s (build %s)\n", os.name().c_str(), (os.is32bit() ? "32 bit" : "64 bit"), os.version().c_str());

	// WINE+PROTON
	const char* env_wineloader = std::getenv("WINELOADER");
	if (env_wineloader != NULL) // Are we running under Wine/Proton?
	{
		PLOGI.printf("  WINE: v%s\n", GetWineVersion());

		const std::regex proton_regex("([Pp]roton[\\s\\-\\w.()]+)");
		std::smatch base_match;
		std::string s_wineloader = std::string(env_wineloader);
		if (std::regex_search(s_wineloader, base_match, proton_regex))
			PLOGI.printf("PROTON: %s\n", base_match[1].str().c_str());
	}

	// End report of PC specs
	PLOGI.printf("----------------\n");
}
