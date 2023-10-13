#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>

#define MAX_SIZE 1024

using std::cout;
using std::cin;
using std::endl;
using std::string;

SOCKET client_sock;
string my_nick;
string my_pw;
string result;

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            cout << "buf = " << buf << endl;

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            /*cout << " tokens[0] 은 " << tokens[0] << endl;
            cout << " tokens[1] 은 " << tokens[1] << endl;
            cout << " tokens[2] 은 " << tokens[2] << endl;*/


            //msg = buf;
            //std::stringstream ss(msg);  // 문자열을 스트림화
            //string user;
            //ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당. 보낸 사람의 이름만 user에 저장됨.
            //if (user != my_nick) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
            //cout << "씨 아웃 버퍼 : " << buf << endl;;


            if (tokens[0] != my_nick) {
                if (tokens[0] == "server") { // 서버로부터 오는 메시지인 
                    //ss >> result; // 결과
                    result = tokens[1];
                    cout << "서버가 보낸거 : " << result << endl;
                    // 여기에서 결과(result)를 사용하거나 처리
                }
                else {
                    cout << buf << endl;
                }
            }

            if (tokens[0] == my_nick) {
                if (tokens[0] == "server") {
                    cout << "여긴 무슨 예외가 와야함?." << endl;
                }
                else {
                    cout <<" 메세지가 전송되었습니다." << endl;
                    //cout << tokens[1] << " 에게 메세지가 전송되었습니다." << endl;
                }
            }


        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}

int main() {
    WSADATA wsa;

    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        string User_request = "1";
        cout << "아아디 입력 >> ";
        cin >> my_nick;
        cout << "비밀번호 입력 >> ";
        cin >> my_pw;

        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

        // 연결할 서버 정보 설정 부분
        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // 위에 설정한 정보에 해당하는 server로 연결!
                cout << "Server Connect" << endl;
                //send(client_sock, my_nick.c_str(), my_nick.length(), 0); // 연결에 성공하면 client 가 입력한 닉네임을 서버로 전송
                //send(client_sock, my_pw.c_str(), my_pw.length(), 0);
                string msg = User_request + " " + my_nick + " " + my_pw;
                send(client_sock, msg.c_str(), msg.length(), 0);
                break;
            }
            cout << "Connecting..." << endl;
        }

        std::thread th2(chat_recv);

        while (1) {            
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환            
            send(client_sock, buffer, strlen(buffer), 0);
        }
        th2.join();
        closesocket(client_sock);
    }

    WSACleanup();
    return 0;
}

