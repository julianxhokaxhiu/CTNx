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

typedef const char* (*WineVersionFunction)();
inline const char* GetWineVersion() {
    HMODULE hModule = LoadLibraryA("ntdll.dll");
    if (!hModule) {
        return "Wine is not detected.";
    }

    WineVersionFunction wine_get_version = (WineVersionFunction)GetProcAddress(hModule, "wine_get_version");
    if (!wine_get_version) {
        FreeLibrary(hModule);
        return "Wine is not detected.";
    }

    const char* version = wine_get_version();
    FreeLibrary(hModule);
    return version;
}
