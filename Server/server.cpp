#pragma comment(lib, "ws2_32.lib") //������� ���̺귯���� ��ũ. ���� ���̺귯�� ����
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h> // C++�ϰ� MYSQL�� �����ϱ� ���� ����.
#include <sstream> // �䱸���� �и��ؼ� �����ϱ� ���� �ʿ���.
#include <WS2tcpip.h> // ���� ������ ���� ���

#define MAX_SIZE 1024
#define MAX_CLIENT 3

using std::cout;
using std::cin;
using std::endl;
using std::string;

// MY SQL ���� ������ �����ϴ� �����ϴ� �� //
const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "root"; // �����ͺ��̽� �����
const string password = "1234"; // �����ͺ��̽� ���� ��й�ȣ

// MySQL Connector/C++ �ʱ�ȭ
sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* res; //������� ����
sql::ResultSet* res2; //������� ����

SOCKET client_sock;

struct SOCKET_INFO { // ����� ���� ������ ���� Ʋ ����
    SOCKET sck;
    string user;
    int user_number;
    bool login_status = false;
};

std::vector<SOCKET_INFO> sck_list; // ����� Ŭ���̾�Ʈ ���ϵ��� ������ �迭 ����.
SOCKET_INFO server_sock; // ���� ���Ͽ� ���� ������ ������ ���� ����.

std::vector<std::string> tokens; // ���Ͽ� ���� ������ ��� ���� ��ū �迭
std::string token; //��ū �迭

int client_count = 0; // ���� ������ �ִ� Ŭ���̾�Ʈ�� count �� ���� ����.
string test_count;
bool login_result = false;
void server_init(); // socket �ʱ�ȭ �Լ�. socket(), bind(), listen() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void add_client(); // ���Ͽ� ������ �õ��ϴ� client�� �߰�(accept)�ϴ� �Լ�. client accept() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void add_client_3();
void send_msg(const char* msg); // send() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void recv_msg(int idx); // recv() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.

void dm_send_msg(const string& sender, const char* msg, const string& recipientUser);
void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser);
int chat_recv();

// ��� ����
void del_client(int idx); // ���Ͽ� ����Ǿ� �ִ� client�� �����ϴ� �Լ�. closesocket() �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
// MYSQL DB ���� ������ �Լ� ������ //
// ���߿� �� �ο��� ���� �� �Լ� �߰��� �ʿ伺 ����!! init �� delete�� ��� ���� �ʼ�. //
void db_init(); // db ���̽� ���� �� �ѱ� ���� ���� �κ� ����
void db_createQuery(); // db ũ������Ʈ ���� + ���� �ʿ������� �ʴ�.
void db_insertQuery(); // db �μ�Ʈ ���� + ����ȭ�� �ʿ��ϴ�.
void db_dropQuery(); // db ��� ���� + ���� �ʿ������� �ʴ�.
void db_updateQuery(); // db ������Ʈ ����

void db_selectQuery_ver2(); // db ����Ʈ��
void db_roomUserNameQuery(); //ä�ù濡 �ִ� ���� �̸� ��������
void db_messageSend(); // �޽��� ���� ����
void db_join(); //ȸ������
void db_UserEdit(); // ȸ�� ���� �����κ�
void db_selectQuery(); //db ����Ʈ��
void db_login();
void db_countuser(); // (1) ���� �� �� ������? (���� ������ Ȱ��)
void db_userlist(); // (3) ���� ��� ���
void db_findID(); // (7) ���� ���� ã��
void db_callMessage(); // ���� ä�ù� �ҷ�����


void db_init() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        exit(1);
    }
    // �����ͺ��̽� ����
    con->setSchema("test");
    // db �ѱ� ������ ���� ����
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }
}
//���� �ʿ����� ����.
void db_createQuery() {
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    stmt->execute("CREATE TABLE users (user_id VARCHAR(10) primary key not null, name VARCHAR(10) not null, pw VARCHAR(10) not null, phonenumber VARCHAR(20) not null, nickname VARCHAR(10) not null, friend_name VARCHAR(10));"); // user ���̺�
    stmt->execute("CREATE TABLE chatroom (room_num int primary key auto_increment, user_id_1 VARCHAR(10) not null, user_id_2 VARCHAR(10) not null, messageDB_num int, foreign key(user_id_1) references users(user_id) on update cascade on delete cascade, foreign key(user_id_2) references users(user_id) on update cascade on delete cascade);"); // chatroom ���̺�
    stmt->execute("CREATE TABLE message_room (	number int primary key auto_increment, user_id VARCHAR(10) not null, content VARCHAR(255) not null, time date not null, chatroom_num int not null, foreign key(chatroom_num) references chatroom(room_num) on update cascade on delete cascade);"); // message_room ���̺�
    cout << "Finished creating table" << endl;
    delete stmt;
}
//�μ�Ʈ�� ����ȭ�� �ʿ���.
void db_insertQuery() { //�ϴ� �Է¸� �޾Ƽ� ä�������� Ȯ��
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    pstmt = con->prepareStatement("INSERT INTO inventory(name, quantity) VALUES(?,?)"); // INSERT
    cout << "Finished inserting table" << endl;
    delete stmt;
    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;
}
//���� �ʿ����� ���� �� ����.
void db_dropQuery() {
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    stmt->execute("DROP TABLE IF EXISTS inventory"); // DROP
    cout << "Finished dropping table (if existed)" << endl;
    delete stmt;
    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;
}
//������ ���� �ʿ���


void db_updateQuery() {
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    stmt->execute("UPDATE TABLE IF EXISTS inventory"); // UPDATE
    cout << "Finished update table" << endl;
    delete stmt;
    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;
}


//ä�ù濡 �ִ� ���� �̸� ��������
void db_roomUserNameQuery() {
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT room_num, user_id_1, user_id_2 FROM chatroom"); // from �ڿ��� ������ mysql ���� ����ϴ� ���̺��� �̸��� ����Ѵ�.
    delete stmt;

    // ��� ���
    while (res->next()) {
        cout << "���� �������� �� ��ȣ " << res->getString("room_num") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "���� 1�� ID : " << res->getString("user_id_1") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "���� 2�� ID : " << res->getString("user_id_2") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
    }

    string User_Choice = "2";
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM chatroom WHERE room_num = '" + User_Choice + "'");

    // ��� ���
    while (res->next()) {
        cout << User_Choice << " ��� ���ϴ� �濡 �������� ������ : " << res->getString("user_id_1") << "�� �� " << res->getString("user_id_2") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
    }

    // ������ �������� ��ȭ�游 �ҷ�����
    string login_User = "abcd";
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT room_num FROM chatroom WHERE user_id_1 = '" + login_User + "'");

    cout << " ���м� 2" << endl;
    // ��� ���
    while (res->next()) {
        cout << login_User << " �� �������� �� ��ȣ : " << res->getString("room_num") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        string a = res->getString("room_num");

        cout << "a�� ����Ǿ���? = " << a << endl;
        res2 = stmt->executeQuery("SELECT user_id_2 FROM chatroom WHERE room_num = '" + a + "'");
        while (res2->next()) {
            cout << a << " ���� " << res2->getString("user_id_2") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        }

    }


}


// �޽��� ���� ����
void db_messageSend() {
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room");
    int number = 0;
    while (res->next()) {
        number += 1;
    }
    cout << " num " << number << endl; //��� ��ȣ Ȯ��.

    pstmt = con->prepareStatement("INSERT INTO message_room (number, user_id, content, time, chatroom_num) values(?,?,?,?,?)"); // INSERT

    pstmt->setInt(1, number + 1); //�޽��� ��ȣ
    pstmt->setString(2, "abcd"); // ���� ��� ���̵�
    pstmt->setString(3, "hihihihi"); // ���� ��� ���̵�
    pstmt->setString(4, "2023-10-11"); // ��¥
    pstmt->setInt(5, 1); //ê �� �ѹ� 
    pstmt->execute(); // �̰� �־���� ��� �����.

    cout << "�޼����� ����Ǿ����ϴ�." << endl;
    delete stmt;
    // MySQL Connector/C++ ����
}

void db_selectQuery() {
    db_init();

    // SQL ���� ����
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room"); // from �ڿ��� ������ mysql ���� ����ϴ� ���̺��� �̸��� ����Ѵ�.
    delete stmt;

    cout << "\n";
    cout << "SQL ���� ���� (Ư�� PK ���� �ش��ϴ� �� ����) \n";
    // SQL ���� ���� (Ư�� PK ���� �ش��ϴ� �� ����)
    string pkValue = "abcd"; // ���� PK ������ ��ü
    //string query = "SELECT * FROM inventory WHERE id = '" + pkValue + "'";
    // ���̺� �̸�, PK �� �̸�
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room");//WHERE user_id = '" + pkValue + "'
    // ��� ���
    while (res->next()) {
        cout << "Column1: " << res->getString("number") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("user_id") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("content") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("time") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("chatroom_num") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
    }

    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;
}

void db_login() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    // ����� �ִٸ�
    //dm_send_result(const string& sender, int variable, const string& recipientUser)
    if (res->next()) {
        string db_id = res->getString("user_id"); // �����ͺ��̽��� id ����
        string db_pw = res->getString("pw"); // �����ͺ��̽��� ��й�ȣ ����
        string db_name = res->getString("name"); // �����ͺ��̽��� �̸� ����

        // �����ͺ��̽��� ����� �����Ϳ� �Է¹��� �����Ͱ� �����ϴٸ�
        if (db_id == tokens[1] && db_pw == tokens[2]) {
            string msg = "�طα��� ����!";
            login_result = true;
            int result = 1;
            int server_request = 1;
            dm_send_result(server_request, "server", result, test_count);
            cout << msg << endl;
        }
        else if (db_id != tokens[1] || db_pw != tokens[2]) {
            string msg = " 636 line �� �α��� ���� ! ���̵� �Ǵ� ��й�ȣ�� Ȯ�����ּ���.";
            int result = 0;
            int server_request = 1;
            dm_send_result(server_request, "server", result, tokens[1]);
            cout << msg << endl;
        }
    }
    else {
        string msg = " �� 642line �α��� ���� ! ���̵� �Ǵ� ��й�ȣ�� Ȯ�����ּ���.";
        int result = 54321;
        int server_request = 1;
        dm_send_result(server_request, "server", result, test_count);
        cout << msg << endl;
    }
}

//�ӽ� ó��
void db_login_ver2() { 
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };
    ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ
    SOCKET_INFO client = {};
    client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(client.sck, buf, MAX_SIZE, 0);
    // Winsock2�� recv �Լ�. client�� ���� �г����� ����.
    cout << "buf" << buf << endl;

    // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
    std::istringstream iss(buf);
    std::vector<std::string> login_tokens;
    std::string login_token;

    while (iss >> login_token) {
        login_tokens.push_back(login_token);
    }

    string user_request = login_tokens[0]; // ��û �۾� ��ȣ
    string user_id = login_tokens[1]; // ���̵�
    string pw = login_tokens[2]; // ��й�ȣ

    db_init();

    pstmt = con->prepareStatement("SELECT user_id, name, pw FROM users WHERE user_id = ?");
    pstmt->setString(1, user_id);
    res = pstmt->executeQuery();

    // ����� �ִٸ�
    if (res->next()) {
        string db_id = res->getString("user_id"); // �����ͺ��̽��� id ����
        string db_pw = res->getString("pw"); // �����ͺ��̽��� ��й�ȣ ����
        string db_name = res->getString("name"); // �����ͺ��̽��� �̸� ����

        // �����ͺ��̽��� ����� �����Ϳ� �Է¹��� �����Ͱ� �����ϴٸ�
        if (db_id == user_id && db_pw == pw) {
            string msg = " �� �α��� ���� ! ";
            cout << msg << endl;
            int result = 12345;
            int server_request = 1;
            dm_send_result(server_request, "server", result, login_tokens[1]);
            send_msg(msg.c_str());
        }
        else if (db_id != user_id || db_pw != pw) {
            string msg = " ��330 �α��� ���� ! ���̵� �Ǵ� ��й�ȣ�� Ȯ�����ּ���.";
            cout << msg << endl;
            send_msg(msg.c_str());
            Sleep(2000);
        }
    }
    else {
        string msg = " �� 336 �α��� ���� ! ���̵� �Ǵ� ��й�ȣ�� Ȯ�����ּ���.";
        cout << msg << endl;
        send_msg(msg.c_str());
        Sleep(2000);
    }
}

void db_countuser() {
    db_init();
    cout << "\n";
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT count(*) FROM users");

    while (res->next()) {
        cout << "[����] ���� ���Ե� ���� �� : " << res->getString("count(*)") << endl;
    }

    delete pstmt;
    delete con;
}

//ȸ������
void db_join() {
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    pstmt = con->prepareStatement("INSERT INTO users (user_id, name, pw, nickname, friend_name) values(?,?,?,?,?)"); // INSERT

    string User_input;
    cout << "���̵� �Է��ϼ���. : ";
    cin >> User_input;
    pstmt->setString(1, User_input); //���̵�

    cout << "�̸��� �Է��ϼ���. : ";
    cin >> User_input;
    pstmt->setString(2, User_input); // �̸�

    cout << "��й�ȣ�� �Է��ϼ���. : ";
    cin >> User_input;
    pstmt->setString(3, User_input); // ��й�ȣ

    cout << "�г����� �Է��ϼ���. : ";
    cin >> User_input;
    pstmt->setString(4, User_input); // ��¥

    pstmt->setString(5, " "); //ģ�����

    pstmt->execute(); // �̰� �־���� ��� �����.

    cout << "Finished inserting table" << endl;


}


// ȸ�� ���� �����κ�
void db_UserEdit() {
    db_init();
    // �����ͺ��̽� ���� ����

    // �����ͺ��̽����� ���� ��й�ȣ�� �������� ����
    string selectQuery = "SELECT pw FROM users WHERE user_id = ?";
    pstmt = con->prepareStatement(selectQuery);
    pstmt->setString(1, "abcd");
    res = pstmt->executeQuery();

    if (res->next()) {
        string database_password = res->getString("pw");

        string user_input_password;
        cout << "��й�ȣ�� �Է��ϼ���. : ";
        cin >> user_input_password;

        // ����ڰ� �Է��� ��й�ȣ�� �����ͺ��̽��� ��й�ȣ ��
        if (user_input_password == database_password) {
            // �Է��� ��й�ȣ�� �����ͺ��̽� ��й�ȣ�� ��ġ�ϸ� ������Ʈ ����
            cout << "Ȯ�� �Ǿ����ϴ�." << endl;
            cout << "��й�ȣ�� �Է��ϼ���. : " << endl;
            cin >> user_input_password;

            string updateQuery = "UPDATE users SET pw = ? WHERE user_id = ?";
            pstmt = con->prepareStatement(updateQuery);
            pstmt->setString(1, user_input_password);
            pstmt->setString(2, "abcd");
            pstmt->executeUpdate();
            cout << "��й�ȣ�� ������Ʈ�Ǿ����ϴ�." << endl;
        }
        else {
            cout << "�Է��� ��й�ȣ�� ��ġ���� �ʽ��ϴ�." << endl;
        }
    }
    else {
        cout << "����ڸ� ã�� �� �����ϴ�." << endl;
    }

    cout << "Finished update table" << endl;

}

void db_selectQuery_ver2() {
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM inventory"); // from �ڿ��� ������ mysql ���� ����ϴ� ���̺��� �̸��� ����Ѵ�.

    delete stmt;

    // ��� ���
    while (res->next()) {
        cout << "Column1: " << res->getString("name") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("quantity") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
    }

    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;
}

void db_userlist() {
    db_init();
    cout << "\n";
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM users");

    cout << "[���� ���Ե� ���� ���]" << endl;
    while (res->next()) {
        cout << "ID : " << res->getString("user_id") << endl;
    }
}

void db_findID() {
    db_init();
    string name, phonenumber;

    cout << "ID�� ã�� �̸��� �Է��ϼ���. : ";
    cin >> name;

    while (true) {
        cout << "��ȭ��ȣ�� �Է��ϼ���. : ";
        cin >> phonenumber;
        if (phonenumber.length() != 13) {
            cout << "�� ��ȭ��ȣ�� �ٽ� �Է����ּ���. (- ����)" << endl;
            continue;
        }
        break;
    }

    pstmt = con->prepareStatement("SELECT user_id, name, phonenumber FROM users WHERE phonenumber = ?");
    pstmt->setString(1, phonenumber);
    res = pstmt->executeQuery();

    if (res->next()) {
        string db_id = res->getString(1);
        string db_name = res->getString(2);
        string db_phonenumber = res->getString(3);

        if (db_name == name && db_phonenumber == phonenumber) {
            cout << name << "���� ���̵�� " << db_id << " �Դϴ�." << endl;
        }
        else {
            cout << "�Է��Ͻ� ������ ã�� �� �����ϴ�." << endl;
        }
    }
    else {
        cout << "�Է��Ͻ� ������ ã�� �� �����ϴ�." << endl;
    }
}

void db_findPW() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };
    ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ
    SOCKET_INFO client = {};
    client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(client.sck, buf, MAX_SIZE, 0);
    // Winsock2�� recv �Լ�. client�� ���� �г����� ����.
    cout << "buf" << buf << endl;

    // ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����
    std::istringstream iss(buf);
    std::vector<std::string> findID_tokens;
    std::string findID_token;

    while (iss >> findID_token) {
        findID_tokens.push_back(findID_token);
    }

    string user_request = findID_tokens[0]; // ��û �۾� ��ȣ
    string user_id = findID_tokens[1]; // ���̵�
    string pw = findID_tokens[2]; // ��й�ȣ

    db_init();
    string name, phonenumber;

    cout << "ID�� �Է��ϼ���. : ";
    cin >> user_id;
    cout << "ID�� ã�� �̸��� �Է��ϼ���. : ";
    cin >> name;

    while (true) {
        cout << "��ȭ��ȣ�� �Է��ϼ���. : ";
        cin >> phonenumber;
        if (phonenumber.length() != 13) {
            cout << "�� ��ȭ��ȣ�� �ٽ� �Է����ּ���. (- ����)" << endl;
            continue;
        }
        break;
    }

    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber FROM users WHERE phonenumber = ?");
    pstmt->setString(1, phonenumber);
    res = pstmt->executeQuery();

    if (res->next()) {
        string db_id = res->getString(1);
        string db_name = res->getString(2);
        string db_pw = res->getString(3);
        string db_phonenumber = res->getString(4);

        if (db_id == user_id && db_name == name && db_phonenumber == phonenumber) {
            cout << user_id << "���� ��й�ȣ�� " << db_pw << " �Դϴ�." << endl;
        }
        else {
            cout << "�Է��Ͻ� ������ ã�� �� �����ϴ�." << endl;
            cout << "dddd" << endl;
        }
    }
    else {
        cout << "�Է��Ͻ� ������ ã�� �� �����ϴ�." << endl;
        cout << "djdk" << endl;
    }
}

void db_callMessage() {
    db_init();

    string num;
    cout << "�ҷ��� ä�ù� ��ȣ�� �Է��ϼ���. : "; // ä�ù� ��ȣ�� ���� �������� ���������� ��
    cin >> num;
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room where chatroom_num = '" + num + "'");

    while (res->next()) {
        cout << "[" << res->getString("user_id") << "]" << " ";
        cout << "'" << res->getString("content") << "'" << " ";
        cout << "(" << res->getString("time") << ")" << endl;
    }


}

int main() {
    WSADATA wsa;
    // Winsock�� �ʱ�ȭ�ϴ� �Լ�. MAKEWORD(2, 2)�� Winsock�� 2.2 ������ ����ϰڴٴ� �ǹ�.
    // ���࿡ �����ϸ� 0��, �����ϸ� �� �̿��� ���� ��ȯ.
    // 0�� ��ȯ�ߴٴ� ���� Winsock�� ����� �غ� �Ǿ��ٴ� �ǹ�.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        server_init();
        std::thread th1[MAX_CLIENT];
        
        for (int i = 0; i < MAX_CLIENT; i++) {
            // �ο� �� ��ŭ thread �����ؼ� ������ Ŭ���̾�Ʈ�� ���ÿ� ������ �� �ֵ��� ��.
            th1[i] = std::thread(add_client);
        }
        //recv_msg(sck_list[client_count].user_number);
        //std::thread th1(add_client); // �̷��� �ϸ� �ϳ��� client�� �޾���...
        while (1) { // ���� �ݺ����� ����Ͽ� ������ ����ؼ� ä�� ���� �� �ִ� ���¸� ����� ��. �ݺ����� ������� ������ �� ���� ���� �� ����.            
            string text, msg = "";
            std::getline(cin, text);
            const char* buf = text.c_str();
            msg = server_sock.user + " : " + buf;
            send_msg(msg.c_str());
        }
        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i].join();
            //join : �ش��ϴ� thread ���� ������ �����ϸ� �����ϴ� �Լ�.
            //join �Լ��� ������ main �Լ��� ���� ����Ǿ thread�� �Ҹ��ϰ� ��.
            //thread �۾��� ���� ������ main �Լ��� ������ �ʵ��� ����.
        }
        //th1.join();
        //th.join();
        closesocket(server_sock.sck);
    }
    else {
        cout << "���α׷� ����. (Error code : " << code << ")";
    }


    WSACleanup();
    return 0;

}
void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Internet�� Stream ������� ���� ����
   // SOCKET_INFO�� ���� ��ü�� socket �Լ� ��ȯ��(��ũ���� ����)
   // ���ͳ� �ּ�ü��, ��������, TCP �������� �� ��.
    SOCKADDR_IN server_addr = {}; // ���� �ּ� ���� ����
    // ���ͳ� ���� �ּ�ü�� server_addr
    server_addr.sin_family = AF_INET; // ������ Internet Ÿ��
    server_addr.sin_port = htons(7777); // ���� ��Ʈ ����
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // �����̱� ������ local �����Ѵ�.
    //Any�� ���� ȣ��Ʈ�� 127.0.0.1�� ��Ƶ� �ǰ� localhost�� ��Ƶ� �ǰ� ���� �� ����ϰ� �� �� �ֵ�. �װ��� INADDR_ANY�̴�.
    //ip �ּҸ� ������ �� server_addr.sin_addr.s_addr -- ������ ���?
    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // ������ ���� ������ ���Ͽ� ���ε��Ѵ�.
    listen(server_sock.sck, SOMAXCONN); // ������ ��� ���·� ��ٸ���.
    server_sock.user = "server";
    cout << "Server On" << endl;
    
}

void add_client() {

    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };
    ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ

    SOCKET_INFO new_client = {};
    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    // Winsock2�� recv �Լ�. client�� ���� �г����� ����.
    //new_client.user = string(buf);
    cout << "buf = > " << buf << endl;
  
    sck_list.push_back(new_client); // client ������ ��� sck_list �迭�� ���ο� client �߰�        
    std::thread th(recv_msg, client_count);

    client_count++; // client �� ���� 
    sck_list[client_count].login_status = false;
    sck_list[client_count].user_number = client_count;

    cout << "sck_list[client_count].login_status = " << sck_list[client_count].login_status << endl;
    cout << "sck_list[client_count].user_number = " << sck_list[client_count].user_number << endl;

    th.join();
}



void add_client_3() {
    //recv_msg(sck_list[client_count].user_number);

	SOCKADDR_IN addr = {};
	int addrsize = sizeof(addr);
	char buf[MAX_SIZE] = { };
	ZeroMemory(&addr, addrsize); // addr�� �޸� ������ 0���� �ʱ�ȭ

	SOCKET_INFO new_client = {};
	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
	recv(new_client.sck, buf, MAX_SIZE, 0);
	// Winsock2�� recv �Լ�. client�� ���� �г����� ����.
	//new_client.user = string(buf);
	cout << "buf = > " << buf << endl;

	// ���ڿ��� ��Ʈ���� �ְ� ������ �������� �����Ͽ� ���Ϳ� ����

	std::istringstream iss(buf);

	while (iss >> token) {
		tokens.push_back(token);
	}

	//cout << " tokens[0] �� " << tokens[0] << endl;


	if (tokens[0] == "1") { cout << tokens[1] << " �� ���� �α��� ��û�� ���Խ��ϴ�." << endl; };
	if (tokens[0] == "2") { cout << tokens[2] << " �� ���� [���� �ʿ�] ��û�� ���Խ��ϴ�." << endl; };
	if (tokens[0] == "3") { cout << tokens[3] << " �� ���� [���� �ʿ�] ��û�� ���Խ��ϴ�." << endl; };
	if (tokens[0] == "4") { cout << tokens[4] << " �� ���� [���� �ʿ�] ��û�� ���Խ��ϴ�." << endl; };
	if (tokens[0] == "5") { cout << tokens[5] << " �� ���� [���� �ʿ�] ��û�� ���Խ��ϴ�." << endl; };


	new_client.user = tokens[1];
	sck_list.push_back(new_client); // client ������ ��� sck_list �迭�� ���ο� client �߰�        
	std::thread th(recv_msg, client_count);
	client_count++; // client �� ����               

	db_init();
	db_login();
	//dm_send_result(1, "server", 1555, tokens[1]);        
	//del_client(client_count);
	cout << "690: " << buf << endl;

	//if (login_result == true) {

	//    string msg = "[����] " + new_client.user + " ���� �����߽��ϴ�.";
	//    cout << msg << endl;

	//    cout << "���� ���� �׽�Ʈ2" << endl;

	//    // �ٸ� �����κ��� ���� �޽����� ����ؼ� ���� �� �ִ� ���·� ����� �α�.

	//    cout << "[����] ���� ������ �� : " << client_count << "��" << endl;
	//    send_msg(msg.c_str()); // c_str : string Ÿ���� const chqr* Ÿ������ �ٲ���.            
	//    th.join();
	//}
	//th.join();

}
//string msg = User_request + " " + my_nick + " " + my_pw;

void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser) { 
    string vari  = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser;
    for (int i = 1; i < client_count+1; i++) {

        cout << std::to_string(sck_list[i].user_number) << " ���� ������" << endl;
        cout << sck_list[i].user_number << " �� Ȯ�ο� " << endl;
        test_count = std::to_string(sck_list[i].user_number);

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
                send(sck_list[i].sck, result.c_str(), result.length(), 0);
                return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
            //}
        }
    }

    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}

void dm_send_msg(const string& sender, const char* msg, const string& recipientUser) {
    for (int i = 0; i < client_count; i++) {
        if (sck_list[i].user == recipientUser) {
            //if (sck_list[i].login_status == true){
                send(sck_list[i].sck, msg, MAX_SIZE, 0);
                return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
            //}
        }
    }

    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}


void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}

void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";
    //cout << sck_list[idx].user << endl;
    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // ������ �߻����� ������ recv�� ���ŵ� ����Ʈ ���� ��ȯ. 0���� ũ�ٴ� ���� �޽����� �Դٴ� ��.
            msg = sck_list[idx].user + " : " + buf; //
            cout << "793 = " << msg << endl;
            //send_msg(msg.c_str());
                        
            std::istringstream iss(buf);

            while (iss >> token) {
                tokens.push_back(token);
            }
            if (tokens[0] == "1") { cout << tokens[1] << " ��ū[1]�� ���̵����� �������� �α��� ��û�� ���Խ��ϴ�." << endl; };

            db_init();
            db_login();

        }
        else { //�׷��� ���� ��� ���忡 ���� ��ȣ�� �����Ͽ� ���� �޽��� ����
            msg = "[����] " + sck_list[idx].user + " ���� �����߽��ϴ�.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx); // Ŭ���̾�Ʈ ����
            return;
        }
    }
}

void del_client(int idx) {
    closesocket(sck_list[idx].sck);
    //sck_list.erase(sck_list.begin() + idx); // �迭���� Ŭ���̾�Ʈ�� �����ϰ� �� ��� index�� �޶����鼭 ��Ÿ�� ���� �߻�....��
    client_count--;
}
