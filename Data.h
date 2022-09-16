#pragma once

#include <iostream>
#include <vector>
#include <windows.h>
#include <commdlg.h>

#include "Console.h"

class Button;
class ColorButton;

struct Data {
    std::vector<ColorButton*> colorButtons;
    std::vector<Button*>      buttons;
    COORD cursPos;
    WORD  color = FG_BLACK;
};

void initData (Data&);
void killData (Data&);

class Button {
    public :
        short x, y, width;
        std::vector<CHAR_INFO> chArr;
        void (*action) (Console&);
        WORD attr;

        Button (short, short, std::string, void (*action)(Console&));
        Button () {}
        virtual ~Button () {}
        void update (Data&);
        virtual void draw (Console&);
        virtual void mouseEvent(Console&, Data&, MOUSE_EVENT_RECORD&);
};

void newCanv (Console &console);
void prepareFileDialogBox (char filePath [], OPENFILENAME &ofn, DWORD flags);
void saveCanv (Console &console);
void openCanv (Console &console);

class ColorButton {
    public :
        short x, y;
        CHAR_INFO chInfo;

        ColorButton (short, short, WORD color);
        void draw (Console&);
        void mouseEvent(Data&, MOUSE_EVENT_RECORD&);
};
