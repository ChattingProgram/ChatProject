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
string my_id, my_nick, my_pw, my_name, my_phonenumber, result;
string login_User_nick, login_User_id; //�α��� ���� 
string User_edit_pw; //�α����� ���� �̸� ���̵� ����
string edit_check = "N";
string friend_id; // ģ�� �߰��� ģ�� ���̵�

WSADATA wsa;
// Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�..+
// ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
// 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
int code = WSAStartup(MAKEWORD(2, 2), &wsa);

void login(); // �α��� 
bool login_flag = false;
bool findID_flag = false;
bool findPW_flag = false;
bool join_flag = false;
bool join_id_flag = false;
bool dblist_flag = false;
bool register_flag = false;
bool friend_list_flag = false;
void socket_init(); // �������� ����
void findID(); // ���̵� ã��
void findPW(); // �н����� ã��
void join();
bool User_Edit_falg = false;
void friend_list();

void User_Edit(); // ���� ���� �޴���ư 8
void chatting(); // ä���ϱ� �޴���ư 3
//void chat_list(); 
void friend_register();
void friend_list_recv();

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
            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (�̰ɷ� �α��� ���� ����) 
            // / [3] : �޴� ��� / [4] ��� ���� �̸� ) / [5] ��� ���̵�
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
                            cout << " tokens[5] �� " << tokens[5] << endl;
                            login_User_nick = tokens[4];
                            login_User_id = tokens[5];
                            cout << " �� �г��� ���� => " << login_User_nick << endl;
                            cout << "==============�׽�Ʈ ���� ���=============" << endl;
                            // ���⿡�� ���(result)�� ����ϰų� ó��
                            cout << login_User_nick << " �� �α��� �Ǿ����ϴ�." << endl;
                            cout << " 4�� �ڿ� ���� ȭ������ ���ϴ�." << endl;
                            Sleep(5000);
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

int join_recv() {
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

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ȸ������ ���� ����) / [3] : �޴� ��� / [4] : ���� �̸� )
            if (tokens[1] == "server") {
                if (tokens[0] == "4") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " �� ȸ�� ���� ����!" << endl;
                        cout << " �� " << tokens[3] << " �� ȯ���մϴ�. " << endl;
                        cout << "5�� �� ���� ȭ������ ���ư��ϴ�. " << endl;
                        join_flag = true; // ȸ������ ���� ����
                        Sleep(5000);
                        break;
                    }
                    else if (result == "3") {
                        join_flag = false;
                        cout << "#276 �Է��� ���̵� �������� �ʽ��ϴ�." << endl;
                        cout << "ȸ�������� �����մϴ�." << endl;
                        join_id_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (result == "4") {
                        cout << "#263 ȸ������ ����! �̹� �����ϴ� ���̵��Դϴ�. " << endl;
                        join_flag = false;
                        join_id_flag = false;
                        Sleep(5000);
                        join();
                        break;
                    }
                    else {
                        join_flag = false; // ȸ������ ���� ����
                        cout << " #274 // ȸ������ ����! �Է��� ������ �ٽ� Ȯ�����ּ���. " << endl;
                        Sleep(5000);
                        join();
                        break;
                    }
                }
                else {
                    join_flag = false; // ȸ������ ���� ����
                    cout << " #281 // ȸ������ ����! �Է��� ������ �ٽ� Ȯ�����ּ���. " << endl;
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

void findID() {
    system("cls");

    while (!code) {

        if (findID_flag == true) {
            break;
        }

        string User_request = "2"; // ID ã�� ��ȣ?
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

        string msg = User_request + " " + my_name + " " + my_phonenumber;
        send(client_sock, msg.c_str(), msg.length(), 0);


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

        string msg = User_request + " " + my_id + " " + my_name + " " + my_phonenumber;
        send(client_sock, msg.c_str(), msg.length(), 0);

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

void join() {
    system("cls");


    while (!code) {

        if (join_flag == true) {
            break;
        }

        if (join_id_flag == false) {

            while (1) {
                cout << "���̵� �Է��ϼ���. (���� 10�ڸ� ����) : ";
                cin >> my_id;

                if (my_id.length() > 10) {
                    cout << "�Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            string User_request = "41"; // ȸ������ �� ���̵� Ȯ�� ��û ��ȣ
            string msg_id = User_request + " " + my_id;
            send(client_sock, msg_id.c_str(), msg_id.length(), 0);
        }

        else if (join_id_flag == true) {
            while (1) {
                cout << "�̸��� �Է��ϼ���. : ";
                cin >> my_name;

                if (my_name.length() > 10) {
                    cout << "�Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << "��й�ȣ�� �Է��ϼ���. (10�ڸ� ����) : ";
                cin >> my_pw;

                if (my_pw.length() > 10) {
                    cout << "�Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << "��ȭ��ȣ�� �Է��ϼ���. (- ����) : "; // ���� - �Է� ���ص� �ڵ� �ԷµǰԲ�?
                cin >> my_phonenumber;

                if (my_phonenumber.length() > 13) {
                    cout << "�߸� �Է��ϼ̽��ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            while (1) {
                cout << "�г����� �Է��ϼ���. (10�ڸ� ����) : ";
                cin >> my_nick;

                if (my_nick.length() > 10) {
                    cout << "�Է� ������ �ڸ����� �Ѿ����ϴ�. �ٽ� �Է����ּ���. ";
                    continue;
                }
                else { break; }
            }

            string User_request = "4"; // ȸ������ �� ���̵� Ȯ�� ��û ��ȣ
            string msg = User_request + " " + my_id + " " + my_name + " " + my_pw + " " + my_phonenumber + " " + my_nick; // ģ�� ����� ����?
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
            cout << "���� buf = " << buf << endl;

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
                if (tokens[0] == "8") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << " �Է��Ͻ� ��й�ȣ�� ��ġ�մϴ�. 2���� ���� �信��." << endl;
                        Sleep(2000);
                        system("cls");

                        cout << "�����Ͻ� ��й�ȣ�� �Է��ϼ���. ";
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

                        // ���⿡�� ���(result)�� ����ϰų� ó��
                    }

                    else if (result == "2") {
                        cout << "��й�ȣ�� �߸� �Է��ϼ̽��ϴ�. " << endl;

                        Sleep(2000);
                        break;
                        User_Edit();

                    }
                    else if (result == "3") {
                        cout << "3 ��й�ȣ�� ����Ǿ����ϴ�." << endl;
                        User_Edit_falg = true;
                        tokens.clear();
                        Sleep(2000);
                        break;

                    }
                    else {

                        cout << "# 159 // �������." << endl;
                        Sleep(5000);
                        //User_Edit();
                    }
                }
                else {

                    cout << "# 166 // �������." << endl;
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
        //edit_check �� �⺻�� N
        string User_input;
        string User_request = "8";


        cout << login_User_nick << " ���� ȸ�� ���� ��й�ȣ ���� ������" << endl;
        cout << "[����] ���� ��й�ȣ�� �Է��ϼ���. ";
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
            // ������û ��ȣ / ������� : server / �޴»�� : �� / ���� ��ȭ�ϴ� ��� / ��ȭ ����
            
            std::vector<std::string> tokens;
            std::vector<std::string> DB_contents;

            std::stringstream ss(buf);
            std::string token;

            while (std::getline(ss, token, '/')) {
                // "/"�� ������ ù ��° �κ��� ������ �������� �и��Ͽ� first ���Ϳ� ����
                if (tokens.empty()) {
                    std::stringstream tokens_iss(token);
                    while (tokens_iss >> token) {
                        tokens.push_back(token);
                    }
                }
                else {
                    // "/"�� ������ ������ �κ��� "\n"�� �������� �и��Ͽ� ����
                    std::stringstream DB_contents_iss(token);
                    std::string line;
                    while (std::getline(DB_contents_iss, line, '\n')) {
                        DB_contents.push_back(line);
                    }
                }
            }

            if (tokens[0] == "update") {
                cout << "������Ʈ�ϼ���" << endl;
                break;
            }

            // tokens ���� �� ���
            //std::cout << "tokens[0]: " << tokens[0] << std::endl;
            //std::cout << "tokens[1]: " << tokens[1] << std::endl;
            //std::cout << "tokens[2]: " << tokens[2] << std::endl;


            std::cout << tokens[3] << "�� ��ȭ�ϰ� �ֽ��ϴ�." << endl;
            cout << " ============================================ " << endl;

            // second ���� �� ���
            for (size_t i = 0; i < DB_contents.size(); ++i) {
                std::cout << "DB_contents[" << i << "]: " << DB_contents[i] << std::endl;
            }

            if (tokens[0] == "5") {
                msg = buf;
                std::stringstream ss(msg);  // ���ڿ��� ��Ʈ��ȭ                
                cout << buf << endl; // ���� ���� �� �ƴ� ��쿡�� ����ϵ���
            }

            cout << " ============================================ " << endl;
            cout << " ���� �޼����� �Է��ϼ���." << endl;
            while (1) {
                string User_request = "51";
                string user_msg;
                cin >> user_msg;
                if (user_msg == "exit") {
                    dblist_flag = true;
                    break;
                }
                string msg = User_request + " " + login_User_id + " " + user_msg;
                //���� �ʿ�
                send(client_sock, msg.c_str(), msg.length(), 0);
                break;
            }
            cout << " ============================================ " << endl;
            //system("cls");
            
            //cout << " �����̽��ٸ� ������ (�α��ε�) ���� ȭ�� �̵� " << endl;

            //while (1) {
            //    if (keyControl() == SUBMIT) { //�����̽��� ������ �� ������ �� ����â�� �ӹ����ϴ�.
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
        //cout << "ģ�� ����� ��û�մϴ�." << endl;
        string User_request = "5"; //ä���ϱ� �ʹݺ�

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
                cout << "�߰��� ģ���� ���̵� �Է��ϼ���. : ";
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
            cout << "���� buf = " << buf << endl;

            // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
            std::istringstream iss(buf);
            std::vector<std::string> tokens;
            std::string token;

            while (iss >> token) {
                tokens.push_back(token);
            }

            // ��ū �迭 ���� �� Ȯ�ο�
            for (int i = 0; i < tokens.size(); i++) {
                cout << "tokens[" << i << "]" << tokens[i] << endl;
            }
            cout << "��ū ũ�� : " << tokens.size() << endl;

            // ( [0] : ��û ��� (1=�α��� ��) / [1] : ���� ��� ( �ظ��ؼ� "server") / [2] : ����� (ID ã�� ���� ����) / [3] : �޴� ��� / [4] : ã�� ģ�� ����Ʈ(����) )
            if (tokens[1] == "server") { // �����κ��� ���� �޽����� 
                //ss >> result; // ���
                if (tokens[0] == "71") {
                    result = tokens[2];
                    if (result == "1" && tokens.size() > 4) {
                        cout << "# 707" << endl;
                        cout << " �� ���� ��ϵǾ� �ִ� ģ�� ����Դϴ�. " << endl;
                        for (int i = 4; i < tokens.size(); i++) {
                            cout << i-3 << "ģ�� ID : " << tokens[i] << endl;
                        }
                        friend_list_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (result == "2" ) {
                        cout << "���� ��ϵǾ� �ִ� ģ���� �����ϴ�." << endl;
                        cout << "��ȭ�� ���ϸ� ģ���� �߰����ּ���." << endl;
                        friend_list_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (tokens.size() < 5) {
                        cout << "���� ��ϵǾ� �ִ� ģ���� �����ϴ�." << endl;
                        cout << "��ȭ�� ���ϸ� ģ���� �߰����ּ���." << endl;
                        friend_list_flag = true;
                        Sleep(5000);
                        break;
                    }
                }
                else if (tokens[0] == "7") {
                    result = tokens[2];
                    if (result == "1") {
                        cout << "#746" << endl;
                        cout << "ģ�� �߰��� �Ϸ� �Ǿ����ϴ�. " << endl;
                        register_flag = true;
                        Sleep(5000);
                        break;
                    }
                    else if (result == "2") {
                        cout << "#751" << endl;
                        cout << "�̹� �����ϴ� ģ���Դϴ�." << endl;
                        register_flag = false;
                        Sleep(5000);
                        friend_register();
                    }
                    else if (result == "3") {
                        cout << "#757" << endl;
                        cout << "�������� �ʴ� ������Դϴ�." << endl;
                        cout << "ID�� �ٽ� Ȯ�����ּ���." << endl;
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
        if (login_flag == false) {
            findID_flag = false;
            findPW_flag = false;
            join_id_flag = false;
            join_flag = false;
            register_flag = false;
            friend_list_flag = false;
            MainMenu(); // ���� �޴� �׸��� ������ ȣ��
            int menuCode = MenuDraw(); // ���ӽ��� ��ư ���� �� y��ǥ �� ����
            //printf("�޴� �ڵ�� %d ", menuCode); <<�� y��ǥ Ȯ�� ����
            // 1:�α��� / 2:idã�� / 3:pwã�� / 4:ȸ������ / 5:��ȭ / 6:����ä�� / 7:ģ�� / 8:�������
            if (menuCode == 0) {
                login(); // �α��� �Լ� ����
            }
            else if (menuCode == 1) { // 1 ���̵� ã��
                findID();
            }
            else if (menuCode == 2) { // 2 ��й�ȣ ã��
                findPW();
            }
            else if (menuCode == 3) { // 3 ȸ�� ����
                join();
            }
            else if (menuCode == 4) { // 4 ȸ�� ����
                cout << "\n\n\n";
                WSACleanup();
                return 0; // ����
            }
            system("cls"); // �ܼ�â�� Ŭ�� �϶� �ǹ�
        }
        //�α��� �������� ���� Ʈ��� �ٲ������Ƿ�, �α��� ���� ���� ���̵� ��µ�.
        if (login_flag == true) {
            edit_check = "N";
            User_Edit_falg = false;
            dblist_flag = false;
            MainMenu(); // ���� �޴� �׸��� ������ ȣ��
            cout << "�α��� ����! " << login_User_nick << " �� ȯ���մϴ�." << endl;
            //cout << "�ּ� ó�� �ʼ�! Ȯ�ο�! " << login_User_id << " �� ȯ���մϴ�." << endl;
            cout << edit_check << " ���̵�ƮƮ üũ " << User_Edit_falg << "�÷��� " << endl;
            int menuCode = Login_MenuDraw();

            if (menuCode == 0) { // 5 ��ȭ�ϱ�
                chatting();
            }
            else if (menuCode == 1) { // 6 ���� ��ȭ�� �ҷ�����
                //chat_list(); 
            }
            else if (menuCode == 2) { // 7 ģ�� �߰�
                friend_register();
            }
            else if (menuCode == 3) { // 8 ��� ��ȣ ����
                User_Edit(); 
            }
            else if (menuCode == 4) {  // ���� �ϱ�
                cout << "\n\n\n";
                WSACleanup();
                return 0; // ����
            }
            system("cls"); // �ܼ�â�� Ŭ�� �϶� �ǹ�
        }
        system("cls"); // �ܼ�â�� Ŭ�� �϶� �ǹ�
    }
}