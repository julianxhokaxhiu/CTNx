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

static const char* KEY_PREFIXES[] = {
	"CMES0", "CMES1", "CMES2", "CMES3", "CMES4", "CMES5",
	"KMES0", "KMES1", "KMES2",
	"MESI0",
	"MESK0", "MESK1", "MESK2", "MESK3", "MESK4",
	"MESS0",
	"MEST0", "MEST1", "MEST2", "MEST3", "MEST4", "MEST5",
	"MSG01", "MSG02", "MSG03", "MSG04",
	"EXMS0", "EXMS1", "EXMS2", "EXMS3",
	"QUES0",
	"STAF",
	"COMU0",
};

static const char* CHARACTER_NAMES[] = {
	"crono", "marle", "lucca", "robo", "frog", "ayla", "magus"
};

void voice_init();
void voice_term();
