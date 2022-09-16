#pragma once

#include <iostream>
#include <windows.h>
#include <fstream>

/* Visas pieejamās krāsas */
enum {
    FG_BLACK,
    FG_BLUE,
    FG_GREEN,
    FG_CYAN,
    FG_RED,
    FG_MAGENTA,
    FG_BROWN,
    FG_LIGHTGRAY,
    FG_DARKGRAY,
    FG_LIGHTBLUE,
    FG_LIGHTGREEN,
    FG_LIGHTCYAN,
    FG_LIGHTRED,
    FG_LIGHTMAGENTA,
    FG_YELLOW,
    FG_WHITE,
        FG_END /* Beigu vērtība priekš cikliem */
};

/* Konsoles loga nosaukums */
#define CONS_TITLE "Kursa darbs"
/* Izmēri pašam konsoles logam */
#define CONS_ROW_SIZE 27
#define CONS_COL_SIZE 102
/* Izmēri zīmēšanas sadaļai*/
#define CANVAS_ROW_SIZE CONS_ROW_SIZE - 4
#define CANVAS_COL_SIZE 81
/* Ievades nolasīšanas mainīgā izmērs */
#define INPUT_BUFF_SIZE 1

struct ConsoleInfo {
    HANDLE      stdWOutput;     /*Rokturis izvadei*/
    SMALL_RECT  consWriteArea;  /*Konsoles rakstīšanas lauks*/
    COORD       charBufferSize; /*Char masīvu zīmēšanas izmērs*/
    COORD       charArrRendPos; /*Zīmēšanas pozīcīja uz konsoles*/
    CHAR_INFO   clearArr  [CONS_ROW_SIZE * CONS_COL_SIZE]; /* Char masīvs, kas dzēš konsolē iezīmeto */
    CHAR_INFO   renderArr [CONS_ROW_SIZE * CONS_COL_SIZE]; /* Char masīvs zīmēšanai */
    HANDLE      stdRInput;  /*Rokturis ievadei*/
};

class Console {
    public:
        ConsoleInfo cInfo;
        bool loopOn = true;
    private:
        void consInit ();
        WORD getCharAttributes (BYTE);
        void visInit ();
        void inputInit ();
    public :
        Console () {
            consInit();
            visInit();
            inputInit();
        }
        void writeToRendArr (unsigned, unsigned, char, WORD);
        void draw ();
};
