#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
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


            if (tokens[0] != my_nick) {
                if (tokens[0] == "server") { // �����κ��� ���� �޽����� 
                    //ss >> result; // ���
                    result = tokens[1];
                    cout << "������ ������ : " << result << endl;
                    // ���⿡�� ���(result)�� ����ϰų� ó��
                }
                else {
                    cout << buf << endl;
                }
            }

            if (tokens[0] == my_nick) {
                if (tokens[0] == "server") {
                    cout << "���� ���� ���ܰ� �;���?." << endl;
                }
                else {
                    cout <<" �޼����� ���۵Ǿ����ϴ�." << endl;
                    //cout << tokens[1] << " ���� �޼����� ���۵Ǿ����ϴ�." << endl;
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

    // Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�.
    // ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
    // 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        string User_request = "1";
        cout << "�ƾƵ� �Է� >> ";
        cin >> my_nick;
        cout << "��й�ȣ �Է� >> ";
        cin >> my_pw;

        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

        // ������ ���� ���� ���� �κ�
        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

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

        while (1) {            
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string���� char* Ÿ������ ��ȯ            
            send(client_sock, buffer, strlen(buffer), 0);
        }
        th2.join();
        closesocket(client_sock);
    }

    WSACleanup();
    return 0;
}

