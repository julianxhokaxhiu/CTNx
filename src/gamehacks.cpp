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

#include <format>
#include "gamehacks.h"
#include "audio.h"
#include "cocos2d.h"

GameHacks gamehacks;

// PRIVATE

void GameHacks::toggleSpeedhack()
{
	speedhack_enabled = !speedhack_enabled;

	std::string text = std::format("Current Speedhack: {}", speedhack_enabled ? "ENABLED" : "DISABLED");
	show_popup_msg(text);

	holdInput();
}

void GameHacks::resetSpeedhack()
{
	speedhack_current_speed = speedhack_min;
}

void GameHacks::increaseSpeedhack()
{
	speedhack_enabled = true;

	if ((speedhack_current_speed + speedhack_step) <= speedhack_max) speedhack_current_speed += speedhack_step;

	std::string text = std::format("Current Speedhack: {:.2f}x", speedhack_current_speed);
	show_popup_msg(text);

	holdInput();
}

void GameHacks::decreaseSpeedhack()
{
	speedhack_enabled = true;

	if ((speedhack_current_speed - speedhack_step) >= speedhack_min) speedhack_current_speed -= speedhack_step;

	std::string text = std::format("Current Speedhack: {:.2f}x", speedhack_current_speed);
	show_popup_msg(text);

	holdInput();
}

void GameHacks::toggleAutoText()
{
	enable_voice_auto_text = !enable_voice_auto_text;

	std::string text = std::format("Voice auto text mode: {}", enable_voice_auto_text ? "ENABLED" : "DISABLED");
	show_popup_msg(text);

	holdInput();
}

// PUBLIC

void GameHacks::init()
{
	resetSpeedhack();

	if (speedhack_current_speed > 1.0) speedhack_enabled = true;
}

void GameHacks::processKeyboardInput(UINT msg, WPARAM wParam, LPARAM lParam)
{
	isKeyboardShortcutMode = false;
	switch (msg)
	{
	case WM_KEYDOWN:
		if ((::GetKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			isKeyboardShortcutMode = true;
			switch (wParam)
			{
			case 'T':
				toggleAutoText();
				break;
			case VK_UP:
				increaseSpeedhack();
				break;
			case VK_DOWN:
				decreaseSpeedhack();
				break;
			case VK_LEFT:
			case VK_RIGHT:
				toggleSpeedhack();
				break;
			}
		}
		break;
	}
}

double GameHacks::getCurrentSpeedhack()
{
	return speedhack_enabled ? speedhack_current_speed : 1.0;
}

void GameHacks::holdInput()
{
	if(!enable_hold_input) return;
	hold_input_for_frames = 30; // ~1 sec
	enable_hold_input = false;
}

void GameHacks::drawnInput()
{
	if (hold_input_for_frames > 0) hold_input_for_frames--;
}

bool GameHacks::canInputBeProcessed()
{
	return !isGamepadShortcutMode && !isKeyboardShortcutMode;
}
