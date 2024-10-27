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

#include "voice.h"

#include "audio.h"
#include "game.h"
#include "patch.h"
#include "utils.h"

#include <ranges>
#include <queue>

//=============================================================================

int currentTxtFileId = -1;
int currenttxtDialogID = -1;
uint8_t currentPage = 0;

int fieldId;
std::string* fieldName = new std::string();

bool play_voice(const char* field_name, uint32_t window_id, uint32_t dialog_id, uint8_t page_count)
{
  char name[MAX_PATH];

	char page = 'a' + page_count;
	if (page > 'z') page = 'z';

	sprintf(name, "%s/w%u_%u%c", field_name, window_id, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name))
		sprintf(name, "%s/%u%c", field_name, dialog_id, page);

	if (!nxAudioEngine.canPlayVoice(name) && page_count == 0)
	{
		sprintf(name, "%s/w%u_%u", field_name, window_id, dialog_id);

		if (!nxAudioEngine.canPlayVoice(name))
			sprintf(name, "%s/%u", field_name, dialog_id);
	}

	return nxAudioEngine.playVoice(name, window_id);
}

std::string* __fastcall sub_1B92D0(void* _this, void* _, void* a2, int txtFileId, int txtDialogID) {
	if (txtFileId < 27 || txtFileId > 59)
		return game_externals.sub_1B92D0(_this, a2, txtFileId, txtDialogID);

	currentTxtFileId = txtFileId;
	currenttxtDialogID = txtDialogID;

  // Get current Field ID
	game_externals.field_object = *(field_object**)calc_game_address_offset(0x41B4C4);
  fieldId = game_externals.field_object->currentFieldId-1;
  game_externals.get_game_string(_this, fieldName, 11, fieldId);
  *fieldName = split(*fieldName, ",")[0];
  replaceOnce(*fieldName, "MSG_DEBUG_", "");

  play_voice(fieldName->c_str(), 0, currenttxtDialogID, currentPage);

	return game_externals.sub_1B92D0(_this, a2, txtFileId, txtDialogID);;
}

void __fastcall sub_next_page(MsgWindow* _this) {
	if (_this->currentLineNoMaybe == 0 || currentTxtFileId == -1 || currenttxtDialogID == -1)
	{
		game_externals.sub_msgwindow_nextpage(_this);

		return;
	}

  currentPage++;

  play_voice(fieldName->c_str(), 0, currenttxtDialogID, currentPage);

	game_externals.sub_msgwindow_nextpage(_this);
}

// Called by CT when dialogue window closed
void __fastcall sub_195C70(MsgWindow* _this, void* _, int lineCount) {
	currentTxtFileId = -1;
	currenttxtDialogID = -1;
	currentPage = 0;

	nxAudioEngine.stopVoice();

	game_externals.sub_195C70(_this, lineCount);
}

void voice_init()
{
	// Prepare up to 1 voice slots
	nxAudioEngine.setVoiceMaxSlots(1);

	// Hook voice functions
	DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  // ------------------------------------
  DetourAttach((void**)&game_externals.sub_1B92D0, sub_1B92D0);
  DetourAttach((void**)&game_externals.sub_msgwindow_nextpage, sub_next_page);
  DetourAttach((void**)&game_externals.sub_195C70, sub_195C70);
  // ------------------------------------
  DetourTransactionCommit();
}

void voice_term()
{
  // Hook voice functions
	DetourTransactionBegin();
  DetourUpdateThread(GetCurrentThread());
  // ------------------------------------
  DetourDetach((void**)&game_externals.sub_1B92D0, sub_1B92D0);
  DetourDetach((void**)&game_externals.sub_msgwindow_nextpage, sub_next_page);
  DetourDetach((void**)&game_externals.sub_195C70, sub_195C70);
  // ------------------------------------
  DetourTransactionCommit();
}
