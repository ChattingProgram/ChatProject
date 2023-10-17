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
string my_nick;
string login_User_nick; //�α����� ���� �̸� ���̵� ����
string my_pw;
string result;

WSADATA wsa;
// Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�.
// ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
// 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
int code = WSAStartup(MAKEWORD(2, 2), &wsa);

void login(); // �α��� 
bool login_flag = false;
void socket_init(); // �������� ����

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (!login_flag) { //�̰� ���� �ʼ�!!
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            cout << "������ ���� buf = " << buf << endl;

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
            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (�̰ɷ� �α��� ���� ����) / [3] : �޴� ��� / [4] ��� ���� �̸� )
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
                            cout << " tokens[4] �� " << tokens[4] << endl;
                            login_User_nick = tokens[4];
                            cout << " �� �г��� ���� => " << login_User_nick << endl;
                            cout << "==============�׽�Ʈ ���� ���=============" << endl;
                            // ���⿡�� ���(result)�� ����ϰų� ó��
                            cout << login_User_nick << " �� �α��� �Ǿ����ϴ�." << endl;
                            cout << " 4�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                            Sleep(2000);
                            login_flag = true; //�̰� �ؾ��� �α��� ���� ������ �� ����.
                            break;
                                                       
                        }
                        else {
                            cout << "2�α��� ����" << endl;
                            Sleep(2000);
                            login();
                        }
                    }
                    else {
                        cout << "3�α��� ���� 2�ʴ��" << endl;
                        Sleep(2000);
                        login();
                    }
                }
                else {
                    cout << buf << endl;
                }
            }

            if (tokens[1] == login_User_nick) {
                    //cout <<" �޼����� ���۵Ǿ����ϴ�." << endl;
                    //cout << tokens[1] << " ���� �޼����� ���۵Ǿ����ϴ�." << endl;
                    cout << login_User_nick << " �� �α��� �Ǿ����ϴ�." << endl;
                    cout << " 4�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                    Sleep(4000);
                    login_flag = true; //�̰� �ؾ��� �α��� ���� ������ �� ����.
                    break;
                
            }


        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}

void login() {
    system("cls");
    
    while (!login_flag) {
        
        // �̰� �����ؾ� �ٽ� �α��νõ� ����. 
        if (login_flag == true) {
            break;
        }

        string User_input;

        string User_request = "1";
        cout << "�ƾƵ� �Է� >> ";
        cin >> User_input;
        my_nick = User_input;
        cout << "��й�ȣ �Է� >> ";
        cin >> User_input;
        my_pw = User_input;

        cout << "���̵�� : " << my_nick << endl;
        
        while (1) {
            string msg = User_request + " " + my_nick + " " + my_pw;
            send(client_sock, msg.c_str(), msg.length(), 0);
            break;
        }

        std::thread th2(chat_recv);
        //while(1)

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

    while (1) {
        if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // ���� ������ ������ �ش��ϴ� server�� ����!            
            cout << "***SYSTEM MESSAGE : Server Connect***" << endl;
            string msg = "auto produce";
            send(client_sock, msg.c_str(), msg.length(), 0); // ���ῡ �����ϸ� client �� �Է��� �г����� ������ ����
            //send(client_sock, my_pw.c_str(), my_pw.length(), 0);          
            
            break;
        }
        cout << "Connecting..." << endl;
    }
}

int main()
{
    init(); //Ŀ�� �����Ÿ��°� �������ִ� �Լ�
    socket_init();
    
    while (1) {
        //std::thread th2(chat_recv);
        MainMenu(); // ���� �޴� �׸��� ������ ȣ��

        //�α��� �������� ���� Ʈ��� �ٲ������Ƿ�, �α��� ���� ���� ���̵� ��µ�.
        if (login_flag == true) { 
            cout << "�α��� ����!" << login_User_nick << " �� ȯ���մϴ�." << endl;
        }
        int menuCode = MenuDraw(); // ���ӽ��� ��ư ���� �� y��ǥ �� ����
        //printf("�޴� �ڵ�� %d ", menuCode); <<�� y��ǥ Ȯ�� ����

        if (menuCode == 0) {
            login(); // �α��� �Լ� ����
        }
        else if (menuCode == 1) {
            infoGame();// ��������
        }
        else if (menuCode == 2) {
            cout << "\n\n\n";
            //th2.join();
            WSACleanup();
            return 0; // ���� ����
        }
        system("cls"); // �ܼ�â�� Ŭ�� �϶� �ǹ�
    }
}