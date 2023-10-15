#pragma once
#ifndef __STDIO_H__
#define __STDIO_H__

#include <stdio.h>

#endif

#ifndef __WINDOWS_H__
#define __WINDOWS_H__

#include <windows.h>
using namespace std;

//using std::cout; //이중으로 사용할 수 없음.
//using std::cin;
//using std::endl;
//using std::string;

#endif

//#ifndef __SCOKET_INFO__
//#define __SCOKET_INFO__
//#define client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 
//
//// 연결할 서버 정보 설정 부분
//#define SOCKADDR_IN client_addr = {};
//#define client_addr.sin_family = AF_INET;
//#define client_addr.sin_port = htons(7777);
//#define InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
//
//#endif