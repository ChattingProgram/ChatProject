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
string my_id, my_nick, my_pw, my_name, my_phonenumber, result;
string login_User_nick, login_User_id; //로그인 정보 
string User_edit_pw; //로그인한 유저 이름 아이디 저장
string edit_check = "N";
string friend_id; // 친구 추가할 친구 아이디

WSADATA wsa;
// Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미..+
// 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
// 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
int code = WSAStartup(MAKEWORD(2, 2), &wsa);

void login(); // 로그인 
bool login_flag = false;
bool findID_flag = false;
bool findPW_flag = false;
bool join_flag = false;
bool join_id_flag = false;
bool dblist_flag = false;
bool register_flag = false;
bool friend_list_flag = false;
void socket_init(); // 소켓정보 저장
void findID(); // 아이디 찾기
void findPW(); // 패스워드 찾기
void join();
bool User_Edit_falg = false;
void friend_list();

void User_Edit(); // 정보 수정 메뉴버튼 8
void chatting(); // 채팅하기 메뉴버튼 3
//void chat_list(); 
void friend_register();
void friend_list_recv();

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (!login_flag) { //이거 설정 필수!!
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            cout << "서버가 보낸 buf = " << buf << endl;

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
            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (이걸로 로그인 성공 여부) 
            // / [3] : 받는 사람 / [4] 디비 유저 이름 ) / [5] 디비 아이디
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
                            cout << " tokens[4] 은 " << tokens[4] << endl;
                            cout << " tokens[5] 은 " << tokens[5] << endl;
                            login_User_nick = tokens[4];
                            login_User_id = tokens[5];
                            cout << " 내 닉네임 저장 => " << login_User_nick << endl;
                            cout << "==============테스트 정보 출력=============" << endl;
                            // 여기에서 결과(result)를 사용하거나 처리
                            cout << login_User_nick << " 님 로그인 되었습니다." << endl;
                            cout << " 4초 뒤에 메인 화면으로 갑니다." << endl;
                            Sleep(5000);
                            login_flag = true; //이걸 해야지 로그인 여부 결정할 수 있음.
                            break;
                        }
                        else {
                            cout << "2로그인 실패" << endl;
                            Sleep(2000);
                            login();
                        }
                    }
                    else {
                        cout << "3로그인 실패 2초대기" << endl;
                        Sleep(2000);
                        login();
                    }
                }
                else {
                    cout << buf << endl;
                }
            }

            if (tokens[1] == login_User_nick) {
                //cout <<" 메세지가 전송되었습니다." << endl;
                //cout << tokens[1] << " 에게 메세지가 전송되었습니다." << endl;
                cout << login_User_nick << " 님 로그인 되었습니다." << endl;
                cout << " 4초 뒤에 메인 화면으로 갑니다." << endl;
                Sleep(4000);
                login_flag = true; //이걸 해야지 로그인 여부 결정할 수 있음.
                break;

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

int join_recv() {
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

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (회원가입 성공 여부) / [3] : 받는 사람 / [4] : 유저 이름 )
            if (tokens[1] == "server") {
                if (tokens[0] == "4") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " ※ 회원 가입 성공!" << endl;
                        cout << " ※ " << tokens[3] << " 님 환영합니다. " << endl;
                        cout << "5초 뒤 메인 화면으로 돌아갑니다. " << endl;
                        join_flag = true; // 회원가입 성공 여부
                        Sleep(5000);
                        break;
                    }
                    else if (result == "3") {
                        join_flag = false;
                        cout << "#276 입력한 아이디가 존재하지 않습니다." << endl;
                        cout << "회원가입을 진행합니다." << endl;
                        join_id_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (result == "4") {
                        cout << "#263 회원가입 실패! 이미 존재하는 아이디입니다. " << endl;
                        join_flag = false;
                        join_id_flag = false;
                        Sleep(5000);
                        join();
                        break;
                    }
                    else {
                        join_flag = false; // 회원가입 성공 여부
                        cout << " #274 // 회원가입 실패! 입력한 정보를 다시 확인해주세요. " << endl;
                        Sleep(5000);
                        join();
                        break;
                    }
                }
                else {
                    join_flag = false; // 회원가입 성공 여부
                    cout << " #281 // 회원가입 실패! 입력한 정보를 다시 확인해주세요. " << endl;
                    Sleep(5000);
                    join();
                    break;
                }
            }


        }
    }
}

void login() {
    system("cls");

    while (!login_flag) {

        // 이걸 먼저해야 다시 로그인시도 못함. 
        if (login_flag == true) {
            break;
        }

        string User_input;

        string User_request = "1";
        cout << "아아디 입력 >> ";
        cin >> User_input;
        my_nick = User_input;
        cout << "비밀번호 입력 >> ";
        cin >> User_input;
        my_pw = User_input;

        cout << "아이딘는 : " << my_nick << endl;

        while (1) {
            string msg = User_request + " " + my_nick + " " + my_pw;
            send(client_sock, msg.c_str(), msg.length(), 0);
            break;
        }

        std::thread th2(chat_recv);
        //while(1)

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

        while (true) {
            cout << "전화번호 입력(- 포함) >> ";
            cin >> my_phonenumber;

            if (my_phonenumber.length() != 13) {
                cout << "※ 전화번호를 다시 입력해주세요. (- 포함)" << endl;
                continue;
            }
            break;
        }

        string msg = User_request + " " + my_name + " " + my_phonenumber;
        send(client_sock, msg.c_str(), msg.length(), 0);


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

        string msg = User_request + " " + my_id + " " + my_name + " " + my_phonenumber;
        send(client_sock, msg.c_str(), msg.length(), 0);

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

void join() {
    system("cls");


    while (!code) {

        if (join_flag == true) {
            break;
        }

        if (join_id_flag == false) {

            while (1) {
                cout << "아이디를 입력하세요. (영문 10자리 이하) : ";
                cin >> my_id;

                if (my_id.length() > 10) {
                    cout << "입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            string User_request = "41"; // 회원가입 전 아이디 확인 요청 번호
            string msg_id = User_request + " " + my_id;
            send(client_sock, msg_id.c_str(), msg_id.length(), 0);
        }

        else if (join_id_flag == true) {
            while (1) {
                cout << "이름을 입력하세요. : ";
                cin >> my_name;

                if (my_name.length() > 10) {
                    cout << "입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << "비밀번호를 입력하세요. (10자리 이하) : ";
                cin >> my_pw;

                if (my_pw.length() > 10) {
                    cout << "입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << "전화번호를 입력하세요. (- 포함) : "; // 추후 - 입력 안해도 자동 입력되게끔?
                cin >> my_phonenumber;

                if (my_phonenumber.length() > 13) {
                    cout << "잘못 입력하셨습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << "닉네임을 입력하세요. (10자리 이하) : ";
                cin >> my_nick;

                if (my_nick.length() > 10) {
                    cout << "입력 가능한 자리수를 넘었습니다. 다시 입력해주세요. ";
                    continue;
                }
                else { break; }
            }

            string User_request = "4"; // 회원가입 전 아이디 확인 요청 번호
            string msg = User_request + " " + my_id + " " + my_name + " " + my_pw + " " + my_phonenumber + " " + my_nick; // 친구 목록은 제외?
            send(client_sock, msg.c_str(), msg.length(), 0);

        }

        std::thread th(join_recv);

        while (1) {
            break;
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str();
            send(client_sock, buffer, strlen(buffer), 0);
        }
        th.join();
    }
}

int edit_recv() {
    string msg;

    while (!User_Edit_falg) {
        char buf[MAX_SIZE] = { };
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            cout << "들어온 buf = " << buf << endl;

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
                if (tokens[0] == "8") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " 입력하신 비밀번호가 일치합니다. 2초후 변경 페에지." << endl;
                        Sleep(2000);
                        system("cls");

                        cout << "변경하실 비밀번호를 입력하세요. ";
                        string User_input;
                        string User_request = "8";
                        edit_check = "Y";

                        cin >> User_input;
                        User_edit_pw = User_input;

                        while (1) {
                            string msg = User_request + " " + User_edit_pw + " " + login_User_id + " " + edit_check;
                            send(client_sock, msg.c_str(), msg.length(), 0);
                            break;
                        }

                        // 여기에서 결과(result)를 사용하거나 처리
                    }

                    else if (result == "2") {
                        cout << "비밀번호를 잘못 입력하셨습니다. " << endl;

                        Sleep(2000);
                        break;
                        User_Edit();

                    }
                    else if (result == "3") {
                        cout << "3 비밀번호가 변경되었습니다." << endl;
                        User_Edit_falg = true;
                        tokens.clear();
                        Sleep(2000);
                        break;

                    }
                    else {

                        cout << "# 159 // 변경실패." << endl;
                        Sleep(5000);
                        //User_Edit();
                    }
                }
                else {

                    cout << "# 166 // 변경실패." << endl;
                    Sleep(5000);
                    User_Edit();
                }
            }
            else {
                cout << buf << endl;
            }
        }

    }
}


void User_Edit() {
    while (edit_check == "N") {
        system("cls");
        //edit_check 의 기본값 N
        string User_input;
        string User_request = "8";


        cout << login_User_nick << " 님의 회원 정보 비밀번호 변경 페이지" << endl;
        cout << "[보안] 현재 비밀번호를 입력하세요. ";
        cin >> User_input;
        User_edit_pw = User_input;

        while (1) {
            string msg = User_request + " " + User_edit_pw + " " + login_User_id + " " + edit_check;
            send(client_sock, msg.c_str(), msg.length(), 0);
            break;
        }

        std::thread th2(edit_recv);

        th2.join();
    }
}

int list_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (!dblist_flag) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            cout << "buf = " << buf << endl;
            //void dm_send_db(int server_request, const string& sender, const std::string & recipientUser, const std::string& user_2, const std::vector<std::vector<std::string>>&result)
            // 서버요청 번호 / 보낸사람 : server / 받는사람 : 나 / 같이 대화하는 사람 / 대화 내용
            
            std::vector<std::string> tokens;
            std::vector<std::string> DB_contents;

            std::stringstream ss(buf);
            std::string token;

            while (std::getline(ss, token, '/')) {
                // "/"로 분할한 첫 번째 부분은 공백을 기준으로 분리하여 first 벡터에 저장
                if (tokens.empty()) {
                    std::stringstream tokens_iss(token);
                    while (tokens_iss >> token) {
                        tokens.push_back(token);
                    }
                }
                else {
                    // "/"로 분할한 나머지 부분은 "\n"을 기준으로 분리하여 저장
                    std::stringstream DB_contents_iss(token);
                    std::string line;
                    while (std::getline(DB_contents_iss, line, '\n')) {
                        DB_contents.push_back(line);
                    }
                }
            }

            if (tokens[0] == "update") {
                cout << "업데이트하세요" << endl;
                break;
            }

            // tokens 벡터 값 출력
            //std::cout << "tokens[0]: " << tokens[0] << std::endl;
            //std::cout << "tokens[1]: " << tokens[1] << std::endl;
            //std::cout << "tokens[2]: " << tokens[2] << std::endl;


            std::cout << tokens[3] << "과 대화하고 있습니다." << endl;
            cout << " ============================================ " << endl;

            // second 벡터 값 출력
            for (size_t i = 0; i < DB_contents.size(); ++i) {
                std::cout << "DB_contents[" << i << "]: " << DB_contents[i] << std::endl;
            }

            if (tokens[0] == "5") {
                msg = buf;
                std::stringstream ss(msg);  // 문자열을 스트림화                
                cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록
            }

            cout << " ============================================ " << endl;
            cout << " 보낼 메세지를 입력하세요." << endl;
            while (1) {
                string User_request = "51";
                string user_msg;
                cin >> user_msg;
                if (user_msg == "exit") {
                    dblist_flag = true;
                    break;
                }
                string msg = User_request + " " + login_User_id + " " + user_msg;
                //수정 필요
                send(client_sock, msg.c_str(), msg.length(), 0);
                break;
            }
            cout << " ============================================ " << endl;
            //system("cls");
            
            //cout << " 스페이스바를 누르면 (로그인된) 메인 화면 이동 " << endl;

            //while (1) {
            //    if (keyControl() == SUBMIT) { //스페이스바 누르기 전 까지는 이 정보창에 머무릅니다.
            //        dblist_flag = true;
            //        break;
            //    }
            //}
            
        }
    }
}

void friend_list() {
    system("cls");

    while (!dblist_flag) {
        //cout << "친구 목록을 요청합니다." << endl;
        string User_request = "5"; //채팅하기 초반부

        while (1) {
            string msg = User_request + " " + login_User_id ;
            send(client_sock, msg.c_str(), msg.length(), 0);
            break;
        }

        std::thread th2(list_recv);
        while (1);
        th2.join();
    }
}

void friend_register() {
    system("cls");

    while (!code) {
        
        while (!register_flag) {

            if (friend_list_flag == false) {
                string User_request = "71";
                string msg = User_request + " " + login_User_id;
                send(client_sock, msg.c_str(), msg.length(), 0);
            }

            if (friend_list_flag == true) {
                cout << "추가할 친구의 아이디를 입력하세요. : ";
                cin >> friend_id;

                string User_request = "7"; //
                string msg_register = User_request + " " + login_User_id + " " + friend_id;
                send(client_sock, msg_register.c_str(), msg_register.length(), 0);
            }

            std::thread th(friend_list_recv);

            th.join();
        }
    }
}

void friend_list_recv() {
    system("cls");

    while (!register_flag) {
        char buf[MAX_SIZE] = { };
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            cout << "들어온 buf = " << buf << endl;

            // 문자열을 스트림에 넣고 공백을 기준으로 분할하여 벡터에 저장
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // 토큰 배열 저장 값 확인용
            for (int i = 0; i < tokens.size(); i++) {
                cout << "tokens[" << i << "]" << tokens[i] << endl;
            }
            cout << "토큰 크기 : " << tokens.size() << endl;

            // ( [0] : 요청 결과 (1=로그인 등) / [1] : 보낸 사람 ( 왠만해선 "server") / [2] : 결과값 (ID 찾기 성공 여부) / [3] : 받는 사람 / [4] : 찾은 친구 리스트(한줄) )
            if (tokens[1] == "server") { // 서버로부터 오는 메시지인 
                //ss >> result; // 결과
                if (tokens[0] == "71") {
                    result = tokens[2];
                    if (result == "1" && tokens.size() > 4) {
                        cout << "# 707" << endl;
                        cout << " ※ 현재 등록되어 있는 친구 목록입니다. " << endl;
                        for (int i = 4; i < tokens.size(); i++) {
                            cout << i-3 << "친구 ID : " << tokens[i] << endl;
                        }
                        friend_list_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (result == "2" ) {
                        cout << "현재 등록되어 있는 친구가 없습니다." << endl;
                        cout << "대화를 원하면 친구를 추가해주세요." << endl;
                        friend_list_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (tokens.size() < 5) {
                        cout << "현재 등록되어 있는 친구가 없습니다." << endl;
                        cout << "대화를 원하면 친구를 추가해주세요." << endl;
                        friend_list_flag = true;
                        Sleep(5000);
                        break;
                    }
                }
                else if (tokens[0] == "7") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << "#746" << endl;
                        cout << "친구 추가가 완료 되었습니다. " << endl;
                        register_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (result == "2") {
                        cout << "#751" << endl;
                        cout << "이미 존재하는 친구입니다." << endl;
                        register_flag = false;
                        Sleep(5000);
                        friend_register();
                    }
                    else if (result == "3") {
                        cout << "#757" << endl;
                        cout << "존재하지 않는 사용자입니다." << endl;
                        cout << "ID를 다시 확인해주세요." << endl;
                        register_flag = false;
                        Sleep(5000);
                        friend_register();
                    }
                }
            }
        }
    }
}

void socket_init() {

    client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

    // 연결할 서버 정보 설정 부분
    //SOCKADDR_IN client_addr = {};
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(7777);
    InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

    while (1) {
        if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // 위에 설정한 정보에 해당하는 server로 연결!            
            cout << "***SYSTEM MESSAGE : Server Connect***" << endl;
            string msg = "auto produce";
            send(client_sock, msg.c_str(), msg.length(), 0); // 연결에 성공하면 client 가 입력한 닉네임을 서버로 전송
            //send(client_sock, my_pw.c_str(), my_pw.length(), 0);          

            break;
        }
        cout << "Connecting..." << endl;
    }
}

int main()
{
    init(); //커서 깜빡거리는거 삭제해주는 함수
    socket_init();
    while (1) {
        //std::thread th2(chat_recv);
        if (login_flag == false) {
            findID_flag = false;
            findPW_flag = false;
            join_id_flag = false;
            join_flag = false;
            register_flag = false;
            friend_list_flag = false;
            MainMenu(); // 메인 메뉴 그리기 생성자 호출
            int menuCode = MenuDraw(); // 게임시작 버튼 생성 및 y좌표 값 저장
            //printf("메뉴 코드는 %d ", menuCode); <<로 y좌표 확인 가능
            // 1:로그인 / 2:id찾기 / 3:pw찾기 / 4:회원가입 / 5:대화 / 6:기존채팅 / 7:친추 / 8:비번수정
            if (menuCode == 0) {
                login(); // 로그인 함수 실행
            }
            else if (menuCode == 1) { // 1 아이디 찾기
                findID();
            }
            else if (menuCode == 2) { // 2 비밀번호 찾기
                findPW();
            }
            else if (menuCode == 3) { // 3 회원 가입
                join();
            }
            else if (menuCode == 4) { // 4 회원 가입
                cout << "\n\n\n";
                WSACleanup();
                return 0; // 종료
            }
            system("cls"); // 콘솔창을 클린 하란 의미
        }
        //로그인 성공했을 때만 트루로 바꿔줬으므로, 로그인 됐을 때만 아이디가 출력됨.
        if (login_flag == true) {
            edit_check = "N";
            User_Edit_falg = false;
            dblist_flag = false;
            MainMenu(); // 메인 메뉴 그리기 생성자 호출
            cout << "로그인 성공! " << login_User_nick << " 님 환영합니다." << endl;
            //cout << "주석 처리 필수! 확인용! " << login_User_id << " 님 환영합니다." << endl;
            cout << edit_check << " 에이디트트 체크 " << User_Edit_falg << "플래기 " << endl;
            int menuCode = Login_MenuDraw();

            if (menuCode == 0) { // 5 대화하기
                chatting();
            }
            else if (menuCode == 1) { // 6 기존 대화방 불러오기
                //chat_list(); 
            }
            else if (menuCode == 2) { // 7 친구 추가
                friend_register();
            }
            else if (menuCode == 3) { // 8 비밀 번호 수정
                User_Edit(); 
            }
            else if (menuCode == 4) {  // 종료 하기
                cout << "\n\n\n";
                WSACleanup();
                return 0; // 종료
            }
            system("cls"); // 콘솔창을 클린 하란 의미
        }
        system("cls"); // 콘솔창을 클린 하란 의미
    }
}