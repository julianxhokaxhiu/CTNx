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

#include <StackWalker.h>
#include <dbghelp.h>
#include "globals.h"

#define STACK_MAX_NAME_LENGTH 256

class CTNxStackWalker : public StackWalker
{
public:
    CTNxStackWalker(bool muted = false) : StackWalker(), _baseAddress(0), _size(0), _muted(muted) {}
    DWORD64 getBaseAddress() const {
        return _baseAddress;
    }
    DWORD getSize() const {
        return _size;
    }
protected:
    virtual void OnLoadModule(LPCSTR img, LPCSTR mod, DWORD64 baseAddr,
        DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName,
        ULONGLONG fileVersion
    )
    {
        if (_baseAddress == 0 && _size == 0)
        {
            _baseAddress = baseAddr;
            _size = size;
        }
        StackWalker::OnLoadModule(
            img, mod, baseAddr, size, result, symType, pdbName, fileVersion
        );
    }

    virtual void OnDbgHelpErr(LPCSTR szFuncName, DWORD gle, DWORD64 addr)
    {
        // Silence is golden.
    }

    virtual void OnOutput(LPCSTR szText)
    {
        if (! _muted)
        {
            PLOGD.printf(szText);
        }
    }
private:
    DWORD64 _baseAddress;
    DWORD _size;
    bool _muted;
};

LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS *ep);
