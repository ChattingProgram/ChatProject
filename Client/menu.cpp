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
    cout << "\n\n\n\n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << " 아 직 이 름 미 정 ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ ㅠ         \n";
    cout << "\t\t"; cout << "              조작방법 W A S D / 결정 : 스페이스바                \n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
};

void infoGame() {
    system("cls");
    cout << "\n\n\n\n";
    cout << "\t\t"; cout << "******************************************************************* \n";
    cout << "\t\t"; cout << "누구를 위해 누군가 기도하고 있나 봐 숨죽여 쓴 사랑시가 \n";
    cout << "\t\t"; cout << "낮게 들리는 듯해 너에게로 선명히 날아가 늦지 않게 자리에 닿기를 \n";
    cout << "\t\t"; cout << "I'll be there 홀로 걷는 너의 뒤에 Singing till the end 그치지 않을 이 노래 \n";
    cout << "\t\t"; cout << "아주 잠시만 귀 기울여 봐 유난히 긴 밤을 걷는 널 위해 부를게 \n";
    cout << "\t\t"; cout << "또 한 번 너의 세상에 별이 지고 있나 봐 숨죽여 삼킨 눈물이 \n";
    cout << "\t\t"; cout << "여기 흐르는 듯해 할 말을 잃어 고요한 마음에 기억처럼 들려오는 목소리 \n";
    cout << "\t\t"; cout << "I'll be there 홀로 걷는 너의 뒤에 Singing till the end 그치지 않을 이 노래 \n";
    cout << "\t\t"; cout << "아주 커다란 숨을 쉬어 봐 소리 내 우는 법을 잊은 널 위해 부를게 \n";
    cout << "\t\t"; cout << "******************************************************************* \n";
    cout << "\t\t"; cout << "    스페이스바를 한 번 더 입력하시면 초기 화면으로 돌아갑니다. \n";

    while (1) {
        if (keyControl() == SUBMIT) { //스페이스바 누르기 전 까지는 이 정보창에 머무릅니다.
            break;
        }
    }
}

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

    int x = 36;
    int y = 15;
    gotoxy(x - 2, y); // 35 , 15
    printf(">  로그인  "); //다른 애들보다 x값이 작은건 > 때문에 자연스럽게 보일려고
    gotoxy(x, y + 1); // 36 , 16
    printf(" ID 찾기 ");
    gotoxy(x, y + 2); // 36, 17
    printf(" PW 찾기 ");
    gotoxy(x, y + 3); // 36, 18
    printf(" 기능 미정 ");

    while (1) {
        int n = keyControl(); //키값 받기
        switch (n) {
        case UP: {
            if (y > 15) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, --y);
                printf(">");
            }
            break;
        }
        case DOWN: {
            if (y < 17) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, ++y);
                printf(">");
            }
            break;
        }
        case SUBMIT: {
            return y - 15; // 시작 위치를 빼서 메뉴 선택을 0 , 1 , 2 값으로 좁힘.
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