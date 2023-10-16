#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <vector>

#include "main.h" // �⺻���� ���� �����س��� ��� �ҷ�����
#include "util.h" // Ŀ�� ���� ���
#include "menu.h" // �޴� ���� �׸���, Ŀ�� ��ǥ�� �� 

#define MAX_SIZE 1024

SOCKET client_sock;
SOCKADDR_IN client_addr = {};
string my_id, my_nick, my_pw, my_name, my_phonenumber;
string login_User_nick; //�α����� ���� �̸� ���̵� ����
string result;

WSADATA wsa;
// Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�.
// ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
// 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
int code = WSAStartup(MAKEWORD(2, 2), &wsa);

void login(); // �α��� 
bool login_flag = false;
bool findID_flag = false;
bool findPW_flag = false;
void socket_init(); // �������� ����
void findID(); // ���̵� ã��
void findPW(); // �н����� ã��

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            //cout << "buf = " << buf << endl;

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            /*cout << " tokens[0] �� " << tokens[0] << endl;
            cout << " tokens[1] �� " << tokens[1] << endl;
            cout << " tokens[2] �� " << tokens[2] << endl;*/


            //msg = buf;
            //std::stringstream ss(msg);  // ���ڿ��� ��Ʈ��ȭ
            //string user;
            //ss >> user; // ��Ʈ���� ����, ���ڿ��� ���� �и��� ������ �Ҵ�. ���� ����� �̸��� user�� �����.
            //if (user != my_nick) cout << buf << endl; // ���� ���� �� �ƴ� ��쿡�� ����ϵ���.
            //cout << "�� �ƿ� ���� : " << buf << endl;;

            //void dm_send_result(int server_request, const string & sender, int variable, const string & recipientUser)
            //�����κ��� ���� �޽��� ����
            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (�̰ɷ� �α��� ���� ����) / [3] : �޴� ��� )
            if (tokens[1] != my_nick) {
                if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                    //ss >> result; // ���
                    if (tokens[0] == "1") {
                        result = tokens[2];
                        if (result == "1") {
                            cout << "==============�׽�Ʈ ���� ���=============" << endl;
                            cout << "tokens[o] : " << tokens[0] << endl;
                            cout << "�α��� ����" << endl;
                            cout << " tokens[0] �� " << tokens[0] << endl;
                            cout << " tokens[1] �� " << tokens[1] << endl;
                            cout << " tokens[2] �� " << tokens[2] << endl;
                            cout << " tokens[3] �� " << tokens[3] << endl;
                            login_User_nick = tokens[3];
                            cout << " �� �г��� ���� => " << login_User_nick << endl;
                            cout << "==============�׽�Ʈ ���� ���=============" << endl;
                            // ���⿡�� ���(result)�� ����ϰų� ó��
                                                       
                        }
                        else {
                            login_flag = false; //�̰� �ؾ��� �α��� ���� ������ �� ����.
                            cout << "�α��� ����" << endl;
                        }
                    }
                    else {
                        login_flag = false; //�̰� �ؾ��� �α��� ���� ������ �� ����.
                        cout << "�α��� ����" << endl;
                    }
                }
                else {
                    cout << buf << endl;
                }
            }

            if (tokens[1] == my_nick) {
                    //cout <<" �޼����� ���۵Ǿ����ϴ�." << endl;
                    //cout << tokens[1] << " ���� �޼����� ���۵Ǿ����ϴ�." << endl;
                    cout << login_User_nick << " �� �α��� �Ǿ����ϴ�." << endl;
                    cout << " 4�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                    Sleep(4000);
                    login_flag = true; //�̰� �ؾ��� �α��� ���� ������ �� ����.
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

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ID ã�� ���� ����) / [3] : �޴� ��� / [4] : ���(ã�� ID) )
            if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                //ss >> result; // ���
                if (tokens[0] == "2") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " �� ���̵� ã�� ����!" << endl;
                        string find_User_id = tokens[4];
                        cout << tokens[3] << " ���� ���̵�� : " << find_User_id << " �Դϴ�." << endl;
                        cout << "5�� �� ���� ȭ������ ���ư��ϴ�." << endl;
                        findID_flag = true;
                        Sleep(5000);
                        break;
                        // ���⿡�� ���(result)�� ����ϰų� ó��
                    }
                    else {
                        findID_flag = false; // ID ã�� ���� ����
                        cout << "# 159 // ID ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
                        Sleep(5000);
                        break;
                    }
                }
                else {
                    findID_flag = false; // ID ã�� ���� ����
                    cout << "# 166 // ID ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
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

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ID ã�� ���� ����) / [3] : �޴� ��� / [4] : ���(ã�� ID) )
            if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                //ss >> result; // ���
                if (tokens[0] == "3") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " �� ��й�ȣ ã�� ����!" << endl;
                        string find_User_pw = tokens[4];
                        cout << tokens[3] << " ���� ��й�ȣ�� : " << find_User_pw << " �Դϴ�." << endl;
                        cout << "5�� �� ���� ȭ������ ���ư��ϴ�." << endl;
                        findPW_flag = true;
                        Sleep(5000);
                        break;
                        // ���⿡�� ���(result)�� ����ϰų� ó��
                    }
                    else {
                        findPW_flag = false; // ID ã�� ���� ����
                        cout << "# 170 // PW ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
                        Sleep(5000);
                        break;
                    }
                }
                else {
                    findPW_flag = false; // ID ã�� ���� ����
                    cout << "# 176 // PW ã�� ����. �Է��� ������ �ٽ� Ȯ�����ּ���." << endl;
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

        // �̰� �����ؾ� �ٽ� �α��νõ� ����. 
        if (login_flag == true) {
            break;
        }

        string User_request = "1";
        cout << "���̵� �Է� >> ";
        cin >> my_nick;
        cout << "��й�ȣ �Է� >> ";
        cin >> my_pw;       

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // ���� ������ ������ �ش��ϴ� server�� ����!
                cout << "Server Connect" << endl;
                //send(client_sock, my_nick.c_str(), my_nick.length(), 0); // ���ῡ �����ϸ� client �� �Է��� �г����� ������ ����
                //send(client_sock, my_pw.c_str(), my_pw.length(), 0);
                string msg = User_request + " " + my_nick + " " + my_pw;
                send(client_sock, msg.c_str(), msg.length(), 0);
                break;
            }
            cout << "Connecting..." << endl;
        }

        std::thread th2(chat_recv);
        //if (keyControl() == SUBMIT) { //�����̽��� ������ �� ������ �� ����â�� �ӹ����ϴ�.
        //    login_flag = false;
        //}

        while (1) {        
            break;
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string���� char* Ÿ������ ��ȯ       
            send(client_sock, buffer, strlen(buffer), 0);
        }

        th2.join();        
        //closesocket(client_sock); //�̰� �ּ�ó�� ���ϸ� ������ ��ȯ�� (��������) �α� �ƿ� ó����
     
    }

    //WSACleanup(); //�̰� �ּ�ó�� ���ϸ� ������ ��ȯ�� (��������) �α� �ƿ� ó����

}

void findID() {
    system("cls");

    while (!code) {

        if (findID_flag == true) {
            break;
        }

        string User_request = "2"; // ID ã�� ��ȣ?
        cout << "�̸� �Է� >> ";
        cin >> my_name;

        while(true) {
            cout << "��ȭ��ȣ �Է�(- ����) >> ";
            cin >> my_phonenumber;

            if (my_phonenumber.length() != 13) {
                cout << "�� ��ȭ��ȣ�� �ٽ� �Է����ּ���. (- ����)" << endl;
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
        //if (keyControl() == SUBMIT) { //�����̽��� ������ �� ������ �� ����â�� �ӹ����ϴ�.
        //    login_flag = false;
        //}

        while (1) {
            break;
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string���� char* Ÿ������ ��ȯ       
            send(client_sock, buffer, strlen(buffer), 0);
        }

        th2.join();
        //closesocket(client_sock); //�̰� �ּ�ó�� ���ϸ� ������ ��ȯ�� (��������) �α� �ƿ� ó����

    }

    //WSACleanup(); //�̰� �ּ�ó�� ���ϸ� ������ ��ȯ�� (��������) �α� �ƿ� ó����
}

void findPW() {
    system("cls");

    while (!code) {

        if (findPW_flag == true) {
            break;
        }

        string User_request = "3"; // PW ã�� ��ȣ?
        cout << "ID �Է� >> ";
        cin >> my_id;        
        cout << "�̸� �Է� >> ";
        cin >> my_name;

        while (true) {
            cout << "��ȭ��ȣ �Է�(- ����) >> ";
            cin >> my_phonenumber;

            if (my_phonenumber.length() != 13) {
                cout << "�� ��ȭ��ȣ�� �ٽ� �Է����ּ���. (- ����)" << endl;
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
        //if (keyControl() == SUBMIT) { //�����̽��� ������ �� ������ �� ����â�� �ӹ����ϴ�.
        //    login_flag = false;
        //}

        while (1) {
            break;
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string���� char* Ÿ������ ��ȯ       
            send(client_sock, buffer, strlen(buffer), 0);
        }

        th2.join();
        //closesocket(client_sock); //�̰� �ּ�ó�� ���ϸ� ������ ��ȯ�� (��������) �α� �ƿ� ó����

    }

    //WSACleanup(); //�̰� �ּ�ó�� ���ϸ� ������ ��ȯ�� (��������) �α� �ƿ� ó����
}
void socket_init() {

    client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

    // ������ ���� ���� ���� �κ�
    //SOCKADDR_IN client_addr = {};
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(7777);
    InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);
}

int main()
{
    init(); //Ŀ�� �����Ÿ��°� �������ִ� �Լ�
    socket_init();

    while (1) {
        MainMenu(); // ���� �޴� �׸��� ������ ȣ��

        //�α��� �������� ���� Ʈ��� �ٲ������Ƿ�, �α��� ���� ���� ���̵� ��µ�.
        if (login_flag == true) { 
            cout << login_User_nick << " �� ȯ���մϴ�." << endl;
        }
        int menuCode = MenuDraw(); // ���ӽ��� ��ư ���� �� y��ǥ �� ����
        //printf("�޴� �ڵ�� %d ", menuCode); <<�� y��ǥ Ȯ�� ����

        if (menuCode == 0) {
            login(); // �α��� �Լ� ����
        }
        else if (menuCode == 1) {
            findID();// ���̵� ã��
        }
        else if (menuCode == 2) {
            findPW();// ��й�ȣ ã��
        }
        else if (menuCode == 3) {
            cout << "\n\n\n";
            WSACleanup();
            return 0; // ���� ����
        }
        system("cls"); // �ܼ�â�� Ŭ�� �϶� �ǹ�
    }
}