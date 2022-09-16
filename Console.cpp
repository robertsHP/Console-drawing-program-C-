#include "Console.h"

void Console::consInit (){
    /* Konsoles izmērs */
    SMALL_RECT  consSize       = {0,0,CONS_COL_SIZE,CONS_ROW_SIZE - 1};
    /*Konsoles zīmēšanas lauka izmērs*/
    COORD consBufferSize = {CONS_COL_SIZE,CONS_ROW_SIZE};

    /*Papildus parametri, lai būtu dinamiska konsoles funkcionalitāte*/
    cInfo.stdWOutput     = GetStdHandle(STD_OUTPUT_HANDLE);
    cInfo.consWriteArea  = (SMALL_RECT){0,0,CONS_COL_SIZE - 1,CONS_ROW_SIZE - 1};
    cInfo.charBufferSize = (COORD){CONS_COL_SIZE,CONS_ROW_SIZE};
    cInfo.charArrRendPos = (COORD){0,0};

    /*Piemēro visus parametrus*/
    SetConsoleTitle(CONS_TITLE);
    SetConsoleWindowInfo(
        cInfo.stdWOutput,
        true,
        &consSize);
    SetConsoleScreenBufferSize(cInfo.stdWOutput, consBufferSize);
}
WORD Console::getCharAttributes(BYTE ch) {
    /*Atgriež krāsu, kas atbilst katram simbolam*/
    switch (ch) {
        case 0xBA :
        case 0xBB :
        case 0xBC :
        case 0xC8 :
        case 0xC9 :
        case 0xCA :
        case 0xCB :
        case 0xCD :
            return FG_LIGHTBLUE;
        default :
            return FG_BLACK;
    }
}
void Console::visInit () {
    std::fstream uiFile ("UI.txt", std::ios::in);
    CHAR_INFO    defChInfo; /*Noklusētais simbols*/
    unsigned     y, x, index;
    char         curChar;

    defChInfo.Char.AsciiChar = 0xFF;
    defChInfo.Attributes = FG_BLACK;
    /*Ielādē visus simbolus lietotāja interfeisam no UI.txt*/
    for(y = 0; y < CONS_ROW_SIZE; ++y) {
        for(x = 0; x < CONS_COL_SIZE; ++x) {
            index = x + CONS_COL_SIZE * y;
            cInfo.clearArr[index] = defChInfo;
            curChar = uiFile.get();
            if(curChar != '\n') {
                cInfo.renderArr[index].Char.AsciiChar = curChar;
                cInfo.renderArr[index].Attributes = getCharAttributes(curChar);
            } else {
                cInfo.renderArr[index] = defChInfo;
            }
        }
    }
    uiFile.close();
}

void Console::inputInit (){
    /*Rokturis ievadei*/
    cInfo.stdRInput = GetStdHandle(STD_INPUT_HANDLE);
    /*Konsoles režīms*/
    DWORD  consMode  = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
    SetConsoleMode(cInfo.stdRInput, consMode);
}
/*Ieraksta simbolu renderArr masīvā*/
void Console::writeToRendArr (unsigned x, unsigned y, char ch, WORD attr) {
    CHAR_INFO chInf;
    chInf.Char.AsciiChar = ch;
    chInf.Attributes = attr;
    cInfo.renderArr[x + CONS_COL_SIZE * y] = chInf;
}
void Console::draw () {
    WriteConsoleOutputA(
        cInfo.stdWOutput,
        cInfo.renderArr,
        cInfo.charBufferSize,
        cInfo.charArrRendPos,
        &cInfo.consWriteArea
    );
}
