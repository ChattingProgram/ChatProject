#pragma once
#include "main.h"

#ifndef KEY_CODE
#define KEY_CODE

// ★★★★ 키보드 값 정의 ★★★★ //
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define SUBMIT 4 // 선택하는 거

#endif

int MenuDraw();        // 게임시작 등등 버튼 생성 함수
int Login_MenuDraw();
int keyControl();       // 키보드 입력 값 컨트롤 함수
void infoGame();        //게임정보 출력
void MainMenu();
int Gaming_select();