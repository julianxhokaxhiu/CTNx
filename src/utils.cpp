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
#include "utils.h"

#include "globals.h"

#include <sys/stat.h>
#include <filesystem>

bool fileExists(const char *filename)
{
    struct stat dummy;

    // Use stat to keep compatibility with 7th Heaven
    return stat(filename, &dummy) == 0;
}

bool dirExists(const char *dirname)
{
    struct stat dummy;

    // Use stat to keep compatibility with 7th Heaven
    return stat(dirname, &dummy) == 0;
}
