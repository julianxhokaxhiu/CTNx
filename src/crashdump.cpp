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

#include <shlwapi.h>

#include "audio.h"

#include "crashdump.h"

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *ep)
{
	static uint32_t had_exception = false;
	char filePath[260]{ 0 };

	// give up if we crash again inside the exception handler (this function)
	if(had_exception)
	{
		PLOGV.printf("ExceptionHandler: crash while running another ExceptionHandler. Exiting.\n");
		SetUnhandledExceptionFilter(0);
		return EXCEPTION_CONTINUE_EXECUTION;
	}

	PLOGD.printf("*** Exception 0x%x, address 0x%x ***\n", ep->ExceptionRecord->ExceptionCode, ep->ExceptionRecord->ExceptionAddress);
	CTNxStackWalker sw;
	sw.ShowCallstack(
		GetCurrentThread(),
		ep->ContextRecord
	);

	had_exception = true;

	// show cursor in case it was hidden
	while (ShowCursor(true) < 0);

	if (create_crash_dump)
	{
		PathAppendA(filePath, "crash.dmp");

		HANDLE file = CreateFile(filePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		HANDLE proc = GetCurrentProcess();
		DWORD procid = GetCurrentProcessId();
		MINIDUMP_EXCEPTION_INFORMATION mdei;

		CONTEXT c;
		HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());;
		memset(&c, 0, sizeof(c));
		c.ContextFlags = CONTEXT_FULL;
		GetThreadContext(hThread, &c);

		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = ep;
		mdei.ExceptionPointers->ContextRecord = &c;
		mdei.ClientPointers = true;

		if (!MiniDumpWriteDump(
			proc,
			procid,
			file,
			(MINIDUMP_TYPE)(MiniDumpWithFullMemory |
				MiniDumpWithFullMemoryInfo |
				MiniDumpWithHandleData |
				MiniDumpWithUnloadedModules |
				MiniDumpWithThreadInfo),
			&mdei, NULL, NULL)) {
			wchar_t buf[256];

			FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL, GetLastError(), MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				buf, (sizeof(buf) / sizeof(wchar_t)), NULL);

			PLOGD.printf("MiniDumpWriteDump failed with error: %ls\n", buf);
		}
	}

	PLOGE.printf("Unhandled Exception. See dumped information above.\n");

	MessageBoxA(NULL, "Feel free to visit this link to know about further next steps you can take: https://github.com/julianxhokaxhiu/CTNx/blob/master/docs/faq.md", "Game crashed :(", MB_ICONERROR | MB_OK);

	on_game_close();

	// let OS handle the crash
	SetUnhandledExceptionFilter(0);
	return EXCEPTION_CONTINUE_EXECUTION;
}
