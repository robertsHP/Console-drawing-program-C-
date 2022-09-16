#include "Data.h"
#include "Console.h"

Data data;
Console console;

void mouseEvent(MOUSE_EVENT_RECORD &rec) {
    COORD cursPos = rec.dwMousePosition;
    /* Pārbauda vai pele ir bijusi pakustināta un veic kursora pozīcījas izmaiņas */
    if (rec.dwEventFlags == MOUSE_MOVED) {
        data.cursPos.X = cursPos.X;
        data.cursPos.Y = cursPos.Y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), data.cursPos);
    }
    /* Krāsu izvēles pogu cikls */
    for (ColorButton *colBut : data.colorButtons)
        colBut->mouseEvent(data, rec);

    /* Standarta pogu cikls */
    for (Button *button : data.buttons)
        button->mouseEvent(console, data, rec);

    /* Zīmēšanas funkcionalitāte */
    if(cursPos.X >= 1 && cursPos.Y >= 1) {
        if(cursPos.X <= CANVAS_COL_SIZE && cursPos.Y <= CANVAS_ROW_SIZE) {
            if(rec.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                console.writeToRendArr(cursPos.X, cursPos.Y, 0xDB, data.color);
            } else if(rec.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                console.writeToRendArr(cursPos.X, cursPos.Y, 0xFF, data.color);
            }
        }
    }
}
/* Funkcija, kas izdzēš visus objektus ar konsoles aizvēršanu*/
BOOL WINAPI HandlerRoutine (DWORD eventCode) {
    if(eventCode == CTRL_CLOSE_EVENT) {
        killData(data);
        return FALSE;
    }
    return TRUE;
}
/* Funkcija kurā visa ievade tiek apstrādāta */
void input (DWORD &recInp, INPUT_RECORD *irInBuf) {
    for (DWORD i = 0; i < recInp; i++) {
        switch (irInBuf[i].EventType){
            case MOUSE_EVENT:
                mouseEvent(irInBuf[i].Event.MouseEvent);
                break;
            default: break;
        }
    }
}
/* Funkcija objektu atjaunošanai */
void update () {
    for (Button *button : data.buttons)
        button->update(data);
}
/* Funkcija objektu zīmēsanai */
void draw () {
    for (ColorButton *colBut : data.colorButtons)
        colBut->draw(console);
    for (Button *button : data.buttons)
        button->draw(console);
    console.draw(); /* rezultātu uzzīmē uz konsoles */
}
int main () {
    INPUT_RECORD irInBuf [INPUT_BUFF_SIZE]; /*Buferis kur ievade tiek ierakstīta*/
    DWORD        recInp;  /*Ierakstu skaits*/

    initData(data);
    /*Saista "HandlerRoutine" funkciju, lai būtu iespējams veikt papildus darbības pēc konsoles aizvēršanas*/
    SetConsoleCtrlHandler(HandlerRoutine, true);
    while(console.loopOn){
        if (!ReadConsoleInput(
            console.cInfo.stdRInput, /*Ievades bufera rokturis*/
            irInBuf,
            INPUT_BUFF_SIZE, /*Ievades bufera izmērs*/
            &recInp))
        {
            console.loopOn = false;
        }
        input(recInp, irInBuf);
        update();
        draw();
    }
    killData(data);
    return 0;
}
