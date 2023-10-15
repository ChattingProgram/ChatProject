#include "util.h"

void gotoxy(int x, int y) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); //콘솔 핸들 가져오고
    COORD pos;
    pos.X = x;
    pos.Y = y;
    SetConsoleCursorPosition(consoleHandle, pos);
}

void init() {
    system("mode con cols=100 lines=30 | title 어떻게 C++까지 사랑하겠어 돈을 사랑하는거지, 업그레이드버전"); // 콘솔창 크기 및 제목 설정

    // 커서 깜빡이는거 삭제해버리
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); //콘솔 핸들 가져오기
    CONSOLE_CURSOR_INFO consoleCrusor;
    consoleCrusor.bVisible = 0; // 0이나 false 처리하면 커서 숨기기
    consoleCrusor.dwSize = 1;
    SetConsoleCursorInfo(consoleHandle, &consoleCrusor);
}