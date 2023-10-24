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
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "          조작방법 W A S D / 결정 : 스페이스바   \n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
}

void FindID_Menu() {
    cout << "\n■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "\t\t   < ID 찾기 > \n\n";
    cout << "\t\t 1. 이름 입력 (10자리 이하) \n\n";
    cout << "\t\t 2. 전화번호 입력 (- 제외) \n\n";
    cout << "\t\t ※ 실패 시 다시 입력\n\n";
    cout << "\t\t ※ 'exit' 입력 시 메인 메뉴 복귀\n\n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
}

void FindPW_Menu() {
    cout << "\n■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "\t\t   < PW 찾기 > \n\n";
    cout << "\t\t 1. 아이디 입력 (10자리 이하) \n\n";
    cout << "\t\t 2. 이름 입력 (10자리 이하) \n\n";
    cout << "\t\t 3. 전화번호 입력 (- 제외)\n\n";
    cout << "\t\t ※ 실패 시 다시 입력\n\n";
    cout << "\t\t ※ 'exit' 입력 시 메인 메뉴 복귀\n\n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
}

void Join_Menu() {
    cout << "\n■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "\t\t   < 회원 가입 > \n\n";
    cout << "\t\t 1. 아이디 입력 (10자리 이하) \n\n";
    cout << "\t\t 2. 이름 입력 (10자리 이하) \n\n";
    cout << "\t\t 3. 전화번호 입력 (- 제외) \n\n";
    cout << "\t\t 4. 닉네임 입력 (10자리 이하) \n\n";
    cout << "\t\t ※ 실패 시 다시 입력\n\n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
}

void edit_Menu() {
    cout << "\n■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "\t\t   < 회원 정보 수정 > \n\n";
    cout << "\t\t 1. 본인 확인용 비밀번호 입력 \n\n";
    cout << "\t\t 2. 변경할 비밀번호 입력 (10자리 이하) \n\n";
    cout << "\t\t ※ 실패 시 다시 입력\n\n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
}

void login_Menu() {
    cout << "\n■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "\t\t\t   < 로그인 > \n\n";
    cout << "\t\t 1. 아이디 입력 \n\n";
    cout << "\t\t 2. 비밀번호 입력 \n\n";
    cout << "\t\t ※ 실패 시 메인 화면 복귀\n\n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
}

void friend_register_Menu() {
    cout << "\n■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "\t\t\t   < 친구 추가 > \n\n";
    cout << "\t\t 1. 추가하고 싶은 친구의 아이디 입력 \n\n";
    cout << "\t\t ※ 회원가입 되어 있지 않은 아이디는 친구 추가 불가능\n\n";
    cout << "\t\t ※ 'exit' 입력 시 메인 화면 복귀\n\n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
}

void conversation_Menu() {
    cout << "\n■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n";
    cout << "■                                                     ■\n";
    cout << "■       ****     *     *         *       *******      ■\n";
    cout << "■      *         *     *        * *         *         ■\n";
    cout << "■      *         *******       *****        *         ■\n";
    cout << "■      *         *     *      *     *       *         ■\n";
    cout << "■       ****     *     *     *       *      *         ■\n";
    cout << "■                                                     ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                * **     *   *     *                 ■\n";
    cout << "■                *   *    *   *    * *                ■\n";
    cout << "■                ****      ***    *   *               ■\n";
    cout << "■                                                     ■\n";
    cout << "■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ ■\n\n";
    cout << "\t\t\t   < 대화 상대 선택 > \n\n";
    cout << "\t\t 1. 대화하고 싶은 친구의 아이디 입력 \n\n";
    cout << "\t\t ※ 기존 대화 내역이 없어도 신규 대화 가능\n\n";
    cout << "\t\t ※ 회원가입 되어 있지 않은 아이디는 대화 진행 불가능\n\n";
    cout << "\t\t ※ 'exit' 입력 시 메인 화면 복귀\n\n";

    //getchar(); // 아무키 입력 기다림
    //system("cls"); // 콘솔 창 clear
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
    // 
     // 메뉴 코드 0:로그인 / 1:id찾기 / 2:pw찾기 / 3:회원가입 / 4:대화 / 5:기존채팅 / 6:친추 / 7:비번수정
    int x = 21;
    int y = 20;
    gotoxy(x - 2, y); // 20 , 20
    printf(">  로그인  "); //다른 애들보다 x값이 작은건 > 때문에 자연스럽게 보일려고
    gotoxy(x, y + 1); // 21 , 21
    printf(" ID 찾기 ");
    gotoxy(x, y + 2); // 21, 22
    printf(" PW 찾기 ");
    gotoxy(x, y + 3); // 21, 23
    printf(" 회원 가입 ");
    gotoxy(x, y + 4); // 21, 24
    printf(" 프로그램 종료 ");

    while (1) {
        int n = keyControl(); //키값 받기
        switch (n) {
        case UP: {
            if (y > 20) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, --y);
                printf(">");
            }
            break;
        }
        case DOWN: {
            if (y < 24) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, ++y);
                printf(">");
            }
            break;
        }
        case SUBMIT: {
            return y - 20; // 시작 위치를 빼서 메뉴 선택을 0 , 1 , 2 값으로 좁힘.
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

    int x = 21;
    int y = 20;
    gotoxy(x - 2, y); // 20 , 20
    printf(">  대화하기  "); //다른 애들보다 x값이 작은건 > 때문에 자연스럽게 보일려고
    gotoxy(x, y + 1); // 21 , 21
    printf(" 기존 대화방 ");
    gotoxy(x, y + 2); // 21, 22
    printf(" 친구 추가 ");
    gotoxy(x, y + 3); // 21, 23
    printf(" 비밀번호 수정 ");
    gotoxy(x, y + 4); // 21, 24
    printf(" 프로그램 종료 ");

    while (1) {
        int n = keyControl(); //키값 받기
        switch (n) {
        case UP: {
            if (y > 20) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, --y);
                printf(">");
            }
            break;
        }
        case DOWN: {
            if (y < 24) {
                gotoxy(x - 2, y);
                printf(" ");
                gotoxy(x - 2, ++y);
                printf(">");
            }
            break;
        }
        case SUBMIT: {
            return y - 20; // 시작 위치를 빼서 메뉴 선택을 3, 4, 5 값으로 좁힘.
        }
        }//스위치문 닫는 곳

    }
}
