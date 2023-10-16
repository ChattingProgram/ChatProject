#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>

#include "main.h" // 기본적인 설정 저장해놓은 헤더 불러오기
#include "util.h" // 커서 관련 기능
#include "menu.h" // 메뉴 관련 그리기, 커서 좌표값 등 

#define MAX_SIZE 1024

SOCKET client_sock;
SOCKADDR_IN client_addr = {};
string my_id, my_nick, my_pw, my_name, my_phonenumber;
string login_User_nick; //로그인한 유저 이름 아이디 저장
string result;

WSADATA wsa;
// Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
// 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
// 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
int code = WSAStartup(MAKEWORD(2, 2), &wsa);

void login(); // 로그인 
bool login_flag = false;
bool findID_flag = false;
bool findPW_flag = false;
void socket_init(); // 소켓정보 저장
void findID(); // 아이디 찾기
void findPW(); // 패스워드 찾기

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            //cout << "buf = " << buf << endl;

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

            //void dm_send_result(int server_request, const string & sender, int variable, const string & recipientUser)
            //서버로부터 오는 메시지 형태
            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (이걸로 로그인 성공 여부) / [3] : 받는 사람 )
            if (tokens[1] != my_nick) {
                if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                    //ss >> result; // 결과
                    if (tokens[0] == "1") {
                        result = tokens[2];
                        if (result == "1") {
                            cout << "==============테스트 정보 출력=============" << endl;
                            cout << "tokens[o] : " << tokens[0] << endl;
                            cout << "로그인 성공" << endl;
                            cout << " tokens[0] 은 " << tokens[0] << endl;
                            cout << " tokens[1] 은 " << tokens[1] << endl;
                            cout << " tokens[2] 은 " << tokens[2] << endl;
                            cout << " tokens[3] 은 " << tokens[3] << endl;
                            login_User_nick = tokens[3];
                            cout << " 내 닉네임 저장 => " << login_User_nick << endl;
                            cout << "==============테스트 정보 출력=============" << endl;
                            // 여기에서 결과(result)를 사용하거나 처리
                                                       
                        }
                        else {
                            login_flag = false; //이걸 해야지 로그인 여부 결정할 수 있음.
                            cout << "로그인 실패" << endl;
                        }
                    }
                    else {
                        login_flag = false; //이걸 해야지 로그인 여부 결정할 수 있음.
                        cout << "로그인 실패" << endl;
                    }
                }
                else {
                    cout << buf << endl;
                }
            }

            if (tokens[1] == my_nick) {
                    //cout <<" 메세지가 전송되었습니다." << endl;
                    //cout << tokens[1] << " 에게 메세지가 전송되었습니다." << endl;
                    cout << login_User_nick << " 님 로그인 되었습니다." << endl;
                    cout << " 4초 뒤에 메인 화면으로 갑니다." << endl;
                    Sleep(4000);
                    login_flag = true; //이걸 해야지 로그인 여부 결정할 수 있음.
                    //break;
                
            }


        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}

int findID_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            //cout << "buf = " << buf << endl;

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (ID 찾기 성공 여부) / [3] : 받는 사람 / [4] : 결과(찾은 ID) )
            if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                //ss >> result; // 결과
                if (tokens[0] == "2") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " ※ 아이디 찾기 성공!" << endl;
                        string find_User_id = tokens[4];
                        cout << tokens[3] << " 님의 아이디는 : " << find_User_id << " 입니다." << endl;
                        cout << "5초 후 메인 화면으로 돌아갑니다." << endl;
                        findID_flag = true;
                        Sleep(5000);
                        break;
                        // 여기에서 결과(result)를 사용하거나 처리
                    }
                    else {
                        findID_flag = false; // ID 찾기 성공 여부
                        cout << "# 159 // ID 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
                        Sleep(5000);
                        break;
                    }
                }
                else {
                    findID_flag = false; // ID 찾기 성공 여부
                    cout << "# 166 // ID 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
                    Sleep(5000);
                    break;
                }
            }
            else {
                cout << buf << endl;
            }
        }

    }
}

int findPW_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            //cout << "buf = " << buf << endl;

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (ID 찾기 성공 여부) / [3] : 받는 사람 / [4] : 결과(찾은 ID) )
            if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                //ss >> result; // 결과
                if (tokens[0] == "3") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " ※ 비밀번호 찾기 성공!" << endl;
                        string find_User_pw = tokens[4];
                        cout << tokens[3] << " 님의 비밀번호는 : " << find_User_pw << " 입니다." << endl;
                        cout << "5초 후 메인 화면으로 돌아갑니다." << endl;
                        findPW_flag = true;
                        Sleep(5000);
                        break;
                        // 여기에서 결과(result)를 사용하거나 처리
                    }
                    else {
                        findPW_flag = false; // ID 찾기 성공 여부
                        cout << "# 170 // PW 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
                        Sleep(5000);
                        break;
                    }
                }
                else {
                    findPW_flag = false; // ID 찾기 성공 여부
                    cout << "# 176 // PW 찾기 실패. 입력한 정보를 다시 확인해주세요." << endl;
                    Sleep(5000);
                    break;
                }
            }
            else {
                cout << buf << endl;
            }

        }
    }
}

void login() {
    system("cls");
    
    while (!code) {

        // 이걸 먼저해야 다시 로그인시도 못함. 
        if (login_flag == true) {
            break;
        }

        string User_request = "1";
        cout << "아이디 입력 >> ";
        cin >> my_nick;
        cout << "비밀번호 입력 >> ";
        cin >> my_pw;       

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
        //if (keyControl() == SUBMIT) { //스페이스바 누르기 전 까지는 이 정보창에 머무릅니다.
        //    login_flag = false;
        //}

        while (1) {        
            break;
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환       
            send(client_sock, buffer, strlen(buffer), 0);
        }

        th2.join();        
        //closesocket(client_sock); //이거 주석처리 안하면 페이지 전환시 (서버에서) 로그 아웃 처리됨
     
    }

    //WSACleanup(); //이거 주석처리 안하면 페이지 전환시 (서버에서) 로그 아웃 처리됨

}

void findID() {
    system("cls");

    while (!code) {

        if (findID_flag == true) {
            break;
        }

        string User_request = "2"; // ID 찾기 번호?
        cout << "이름 입력 >> ";
        cin >> my_name;

        while(true) {
            cout << "전화번호 입력(- 포함) >> ";
            cin >> my_phonenumber;

            if (my_phonenumber.length() != 13) {
                cout << "※ 전화번호를 다시 입력해주세요. (- 포함)" << endl;
                continue;
            }
            break;
        }
        
        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
                string msg = User_request + " " + my_name + " " + my_phonenumber;
                send(client_sock, msg.c_str(), msg.length(), 0);
                break;
            }
        }

        cout << User_request << endl;
        cout << my_name << endl;
        cout << my_phonenumber << endl;

        std::thread th2(findID_recv);
        //if (keyControl() == SUBMIT) { //스페이스바 누르기 전 까지는 이 정보창에 머무릅니다.
        //    login_flag = false;
        //}

        while (1) {
            break;
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환       
            send(client_sock, buffer, strlen(buffer), 0);
        }

        th2.join();
        //closesocket(client_sock); //이거 주석처리 안하면 페이지 전환시 (서버에서) 로그 아웃 처리됨

    }

    //WSACleanup(); //이거 주석처리 안하면 페이지 전환시 (서버에서) 로그 아웃 처리됨
}

void findPW() {
    system("cls");

    while (!code) {

        if (findPW_flag == true) {
            break;
        }

        string User_request = "3"; // PW 찾기 번호?
        cout << "ID 입력 >> ";
        cin >> my_id;        
        cout << "이름 입력 >> ";
        cin >> my_name;

        while (true) {
            cout << "전화번호 입력(- 포함) >> ";
            cin >> my_phonenumber;

            if (my_phonenumber.length() != 13) {
                cout << "※ 전화번호를 다시 입력해주세요. (- 포함)" << endl;
                continue;
            }
            break;
        }

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
                string msg = User_request + " " + my_id + " " + my_name + " " + my_phonenumber;
                send(client_sock, msg.c_str(), msg.length(), 0);
                break;
            }
        }

        std::thread th2(findPW_recv);
        //if (keyControl() == SUBMIT) { //스페이스바 누르기 전 까지는 이 정보창에 머무릅니다.
        //    login_flag = false;
        //}

        while (1) {
            break;
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환       
            send(client_sock, buffer, strlen(buffer), 0);
        }

        th2.join();
        //closesocket(client_sock); //이거 주석처리 안하면 페이지 전환시 (서버에서) 로그 아웃 처리됨

    }

    //WSACleanup(); //이거 주석처리 안하면 페이지 전환시 (서버에서) 로그 아웃 처리됨
}
void socket_init() {

    client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

    // 연결할 서버 정보 설정 부분
    //SOCKADDR_IN client_addr = {};
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(7777);
    InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
}

int main()
{
    init(); //커서 깜빡거리는거 삭제해주는 함수
    socket_init();

    while (1) {
        MainMenu(); // 메인 메뉴 그리기 생성자 호출

        //로그인 성공했을 때만 트루로 바꿔줬으므로, 로그인 됐을 때만 아이디가 출력됨.
        if (login_flag == true) { 
            cout << login_User_nick << " 님 환영합니다." << endl;
        }
        int menuCode = MenuDraw(); // 게임시작 버튼 생성 및 y좌표 값 저장
        //printf("메뉴 코드는 %d ", menuCode); <<로 y좌표 확인 가능

        if (menuCode == 0) {
            login(); // 로그인 함수 실행
        }
        else if (menuCode == 1) {
            findID();// 아이디 찾기
        }
        else if (menuCode == 2) {
            findPW();// 비밀번호 찾기
        }
        else if (menuCode == 3) {
            cout << "\n\n\n";
            WSACleanup();
            return 0; // 게임 종료
        }
        system("cls"); // 콘솔창을 클린 하란 의미
    }
}