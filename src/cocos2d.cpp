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

#include "cocos2d.h"
#include "gamehacks.h"
#include "patch.h"

void show_popup_msg(std::string& text)
{
  auto currentScene = Director::getInstance()->getRunningScene();

  if (currentScene)
  {
    auto textLayer = CTNxStatusLayer::create(text);
    currentScene->addChild(textLayer, 1);
  }
}
