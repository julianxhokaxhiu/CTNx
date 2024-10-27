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

#include "globals.h"
class CTNxStatusLayer : public Layer {
public:
    CTNxStatusLayer(const std::string& text) {
        auto label = Label::createWithSystemFont(text, "Arial", 12);

        // Cyan color (R=0, G=255, B=255, A=255)
        label->setTextColor(Color4B(0, 255, 255, 255));
        // Set label position at the bottom-left corner of the screen
        label->setAnchorPoint(Vec2(0, 0));
        label->setPosition(Vec2(10, 10));

        // Add the label to the layer
        this->addChild(label, 1);

        // Make the layer disappear after 1 second
        auto delay = DelayTime::create(1.0f);
        auto remove = RemoveSelf::create();
        auto sequence = Sequence::create(delay, remove, nullptr);
        this->runAction(sequence);
    }

    static CTNxStatusLayer* create(const std::string& text) {
        CTNxStatusLayer* ret = new CTNxStatusLayer(text);
        if (ret) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

void show_popup_msg(std::string& text);
