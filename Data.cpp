#include "Data.h"

void initData (Data &data) {
    unsigned x = CONS_COL_SIZE - 14, y = 2;
    for (unsigned i = 0; i < FG_END; ++i, ++x) {
        if(i == 8) {
            y++;
            x = CONS_COL_SIZE - 14;
        }
        data.colorButtons.push_back(new ColorButton(x, y, i));
    }
    data.buttons.push_back(new Button (CONS_COL_SIZE - 12, 6, "New", &newCanv));
    data.buttons.push_back(new Button (CONS_COL_SIZE - 12, 9, "Open", &openCanv));
    data.buttons.push_back(new Button (CONS_COL_SIZE - 12, 12, "Save", &saveCanv));
}
void killData (Data& data) {
    for (ColorButton *cBut : data.colorButtons)  {
        if(cBut != nullptr)
            delete cBut;
    }
    for (Button *button : data.buttons)  {
        if(button != nullptr)
            delete button;
    }
}
Button::Button (short x, short y, std::string txt, void (*action)(Console&)) {
    this->x = x;
    this->y = y;
    this->width = txt.length();
    this->action = action;
    this->attr = FG_WHITE;
    /*Sagatavo objektu priekš zīmēšanas*/
    for (unsigned i = 0; i < txt.length(); ++i) {
        CHAR_INFO ch;
        ch.Char.AsciiChar = txt[i];
        ch.Attributes = FG_WHITE;
        chArr.push_back(ch);
    }
}
void Button::draw (Console &console) {
    for (short i = 0; i < width; ++i) {
        console.cInfo.renderArr[(this->x + i) + this->y * CONS_COL_SIZE] = this->chArr[i];
    }
}
void Button::update (Data &data) {
    COORD &cursPos = data.cursPos;
    /*Pārbauda vai kursors atrodas objekta pozīcijā*/
    if(cursPos.X >= this->x && cursPos.X <= this->x + this->width && cursPos.Y == this->y)
        this->attr = FG_RED;
    else this->attr = FG_WHITE;
    /*Pārbauda vai ir jāveic izmaiņas, lai samazinātu liekas darbības*/
    if(chArr[0].Attributes != this->attr) {
        for (CHAR_INFO &ch : chArr) {
            ch.Attributes = this->attr;
        }
    }
}
void Button::mouseEvent (Console &console, Data &data, MOUSE_EVENT_RECORD &rec) {
    if(rec.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
        if(this->attr == FG_RED) {
            this->action(console);
        }
    }
}
void newCanv (Console &console) {
    unsigned y, x, index;
    for(y = 1; y <= CANVAS_ROW_SIZE; ++y) {
        for(x = 1; x <= CANVAS_COL_SIZE; ++x) {
            index = x + CONS_COL_SIZE * y;
            console.cInfo.renderArr[index].Attributes = FG_BLACK;
        }
    }
}
void prepareFileDialogBox (char filePath [], OPENFILENAME &ofn, DWORD flags) {
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner   = NULL; /*Norāda to kuram logam pieder dialoga logs (šajā gadījumā nav neviens)*/
    ofn.lpstrFilter = "Console picture files\0*.cpic\0Any File\0*.*\0"; /*Atļautie failu formāti*/
    ofn.lpstrFile   = filePath;
    ofn.nMaxFile    = MAX_PATH;
    ofn.lpstrTitle  = "Select file"; /*Dialoga loga virsraksts augšējā daļā*/
    ofn.Flags       = flags;
    ofn.lpstrDefExt = "cpic"; /*Faila formāts*/
}
void saveCanv (Console &console) {
    char filePath[MAX_PATH];
    OPENFILENAME ofn;

    /*Piepilda atmiņas blokus ar nullēm*/
    ZeroMemory(&filePath, sizeof(char) * MAX_PATH);
    ZeroMemory(&ofn, sizeof(ofn));
    /*Sagatavo dialoga logu*/
    prepareFileDialogBox(filePath, ofn, OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);

    if (GetSaveFileNameA(&ofn)) {
        std::ofstream file (filePath, std::ios::binary);

        for(unsigned y = 1; y <= CANVAS_ROW_SIZE; ++y) {
            for(unsigned x = 1; x <= CANVAS_COL_SIZE; ++x) {
                WORD color = console.cInfo.renderArr[x + CONS_COL_SIZE * y].Attributes;
                /*Ievieto saspiestu krāsu failā ar kreiso bināro maiņu (Left shift)*/
                file.put(color << 4);
            }
        }
        file.close();
    }
}
void openCanv (Console &console) {
    char filePath[MAX_PATH];
    OPENFILENAME ofn;

    /*Piepilda atmiņas blokus ar nullēm*/
    ZeroMemory(&filePath, sizeof(char) * MAX_PATH);
    ZeroMemory(&ofn, sizeof(ofn));
    /*Sagatavo dialoga logu*/
    prepareFileDialogBox(filePath, ofn, OFN_EXPLORER | OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);

    if (GetOpenFileName(&ofn)) {
        std::ifstream file (filePath, std::ios::binary);

        for(unsigned y = 1; y <= CANVAS_ROW_SIZE; ++y) {
            for(unsigned x = 1; x <= CANVAS_COL_SIZE; ++x) {
                /*Atspiež krāsu no failā ar labo bināro maiņu (Right shift)*/
                WORD attr = (WORD)(file.get()) >> 4;
                console.cInfo.renderArr[x + CONS_COL_SIZE * y].Char.AsciiChar = 0xDB;
                console.cInfo.renderArr[x + CONS_COL_SIZE * y].Attributes = attr;
            }
        }
        file.close();
    }
}
ColorButton::ColorButton (short x, short y, WORD color) {
    this->x = x;
    this->y = y;
    this->chInfo.Char.AsciiChar = 0xDB;
    this->chInfo.Attributes = color;
}
void ColorButton::draw (Console &console) {
    console.cInfo.renderArr[this->x + this->y * CONS_COL_SIZE] = this->chInfo;
}
void ColorButton::mouseEvent (Data &data, MOUSE_EVENT_RECORD &rec) {
    COORD &cursPos = data.cursPos;
    if(rec.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
        /*Pārbauda vai kursors atrodas pogas atrašanās vietā*/
        if(cursPos.X == this->x && cursPos.Y == this->y){
            /*Uzstāda krāsu lietotājam priekš zīmēšanas*/
            data.color = this->chInfo.Attributes;
        }
    }
}
