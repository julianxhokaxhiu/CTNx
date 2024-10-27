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

#include <stdint.h>
#include <windows.h>

class GameHacks
{
private:
	uint16_t hold_input_for_frames = 0;
	bool enable_hold_input = true;

	bool speedhack_enabled;
	double speedhack_current_speed;

	// SPEEDHACK
	void toggleSpeedhack();
	void resetSpeedhack();
	void increaseSpeedhack();
	void decreaseSpeedhack();

	// INPUT VALIDATION
	void holdInput();
	void drawnInput();

	// VOICE AUTO TEXT
	void toggleAutoText();

public:
	void init();

	// GLOBALS
	void processKeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam);
	void processGamepadInput();

	// SPEEDHACK
	double getCurrentSpeedhack();

	// INPUT VALIDATION
	bool canInputBeProcessed();

private:
	bool isKeyboardShortcutMode = false;
	bool isGamepadShortcutMode = false;
};

extern GameHacks gamehacks;
