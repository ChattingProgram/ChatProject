#include "menu.h"
#include "util.h"

#include<iostream>
#include<stdio.h> 
#include<vector>
#include<windows.h> //좌표 핸들러 사용
#include<conio.h> //_getch가 포함되어있는 헤더



int keyControl() {
    char temp = _getch(); //입력한 값이 콘솔창에 안나오고 바로 받아짐. 헤더추가 필수

    if (temp == 'w' || temp == 'W') {
        return UP;
    }
    else if (temp == 'a' || temp == 'A') {
        return LEFT;
    }
    else if (temp == 's' || temp == 'S') {
        return DOWN;
    }
    else if (temp == 'd' || temp == 'D') {
        return RIGHT;
    }
    else if (temp == ' ') {
        return SUBMIT;
    }

}

void MainMenu() {
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                       ■\n";
    cout << "■       ****    *      *         *        *********     ■\n";
    cout << "■     *         *      *        * *           *         ■\n";
    cout << "■    *          *      *       *   *          *         ■\n";
    cout << "■    *          ********      *******         *         ■\n";
    cout << "■    *          *      *     *       *        *         ■\n";
    cout << "■     *         *      *    *         *       *         ■\n";
    cout << "■       ****    *      *   *           *      *         ■\n";
    cout << "■                                                       ■\n";
    cout << "■              *****      ****    *     *               ■\n";
    cout << "■              *    *    *    *    *   *                ■\n";
    cout << "■              *    *   *      *    * *                 ■\n";
    cout << "■              *  **    *      *     *                  ■\n";
    cout << "■              *    *   *      *    * *                 ■\n";
    cout << "■              *    *    *    *    *   *                ■\n";
    cout << "■              *****      ****    *     *               ■\n";
    cout << "■                                                       ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "        조작방법 W A S D / 결정 : 스페이스바   \n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
};

int MenuDraw() { //첫 게임 시작화면

    // 임시 테스트 위치 파악 용도로 사용!
    //gotoxy(2, 2);
    //printf("2, 2 위치입니다.");
    // 임시 테스트 위치 파악 용도로 사용함!
    // [ 이동방향 ]
    // 왼쪽 : x감소
    // 오른 : x증가
    // 아래 : y증가
    // 위쪽 : y감소
    // 
     // 메뉴 코드 0:로그인 / 1:id찾기 / 2:pw찾기 / 3:회원가입 / 4:대화 / 5:기존채팅 / 6:친추 / 7:비번수정
    int x = 24;
    int y = 24;
    gotoxy(x - 2, y); // 23 , 24
    printf(">  로그인  "); //다른 애들보다 x값이 작은건 > 때문에 자연스럽게 보일려고
    gotoxy(x, y + 1); // 24 , 25
    printf(" ID 찾기 ");
    gotoxy(x, y + 2); // 24, 26
    printf(" PW 찾기 ");
    gotoxy(x, y + 3); // 24, 27
    printf(" 회원 가입 ");
    gotoxy(x, y + 4); // 24, 28
    printf(" 프로그램 종료 ");

    while (1) {
        int n = keyControl(); //키값 받기
        switch (n) {
        case UP: {
            if (y > 24) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, --y);
                printf(">");
            }
            break;
        }
        case DOWN: {
            if (y < 28) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, ++y);
                printf(">");
            }
            break;
        }
        case SUBMIT: {
            return y - 24; // 시작 위치를 빼서 메뉴 선택을 0 , 1 , 2 값으로 좁힘.
        }
        }//스위치문 닫는 곳

    }
}

int Login_MenuDraw() { //첫 게임 시작화면

    // 임시 테스트 위치 파악 용도로 사용!
    //gotoxy(2, 2);
    //printf("2, 2 위치입니다.");
    // 임시 테스트 위치 파악 용도로 사용함!
    // [ 이동방향 ]
    // 왼쪽 : x감소
    // 오른 : x증가
    // 아래 : y증가
    // 위쪽 : y감소

    int x = 24;
    int y = 24;
    gotoxy(x - 2, y); // 23 , 24
    printf(">  대화하기  "); //다른 애들보다 x값이 작은건 > 때문에 자연스럽게 보일려고
    gotoxy(x, y + 1); // 24 , 25
    printf(" 기존 대화방 ");
    gotoxy(x, y + 2); // 24, 26
    printf(" 친구 추가 ");
    gotoxy(x, y + 3); // 24, 27
    printf(" 비밀번호 수정 ");
    gotoxy(x, y + 4); // 24, 28
    printf(" 프로그램 종료 ");

    while (1) {
        int n = keyControl(); //키값 받기
        switch (n) {
        case UP: {
            if (y > 24) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, --y);
                printf(">");
            }
            break;
        }
        case DOWN: {
            if (y < 28) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, ++y);
                printf(">");
            }
            break;
        }
        case SUBMIT: {
            return y - 24; // 시작 위치를 빼서 메뉴 선택을 3, 4, 5 값으로 좁힘.
        }
        }//스위치문 닫는 곳

    }
}


int Gaming_select() { //게임 진행 중 선택하는 곳
    int x = 3;
    int y = 13;
    gotoxy(x - 2, y);
    printf(">  총기 획득 "); // 인덱스 0 반환!!!! 다른 애들보다 x값이 작은건 > 때문에 자연스럽게 보일려고
    gotoxy(x, y + 1); // 36 , 16
    printf(" 칼칼 획득 "); // 인덱스 1 반환!!!!
    gotoxy(x, y + 2); // 36, 17
    printf(" 공격 하기"); // 인덱스 2 반환!!!!
    gotoxy(x, y + 3); // 36, 17
    printf(" 종료 하기"); // 인덱스 3 반환!!!!

    while (1) {
        int n = keyControl(); //키값 받기
        switch (n) {
        case UP: {
            if (y > 13) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, --y);
                printf(">");
            }
            break;
        }
        case DOWN: {
            if (y < 16) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, ++y);
                printf(">");
            }
            break;
        }
        case SUBMIT: {
            return y - 13; // 시작 위치를 빼서 메뉴 선택을 0 , 1 , 2 값으로 좁힘.
        }
        }//스위치문 닫는 곳

    }
}