#pragma comment(lib, "ws2_32.lib") //������� ���̺귯���� ��ũ. ���� ���̺귯�� ����
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h> // C++�ϰ� MYSQL�� �����ϱ� ���� ����.
#include <sstream> // �䱸���� �и��ؼ� �����ϱ� ���� �ʿ���.
#include <WS2tcpip.h> // ���� ������ ���� ���
#include <mutex>

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
sql::Statement* stmt2;
sql::PreparedStatement* pstmt;
sql::PreparedStatement* pstmt2;
sql::ResultSet* res; //������� ����
sql::ResultSet* res2; //������� ����
sql::ResultSet* res3; //������� ����

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
std::mutex mtx;

int client_count = 0; // ���� ������ �ִ� Ŭ���̾�Ʈ�� count �� ���� ����.
string test_count;
bool login_result = false;
bool request_result = false;
bool join_result = false;
bool join_check = false;
bool user_check = false;
void server_init(); // socket �ʱ�ȭ �Լ�. socket(), bind(), listen() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void add_client(); // ���Ͽ� ������ �õ��ϴ� client�� �߰�(accept)�ϴ� �Լ�. client accept() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void send_msg(const char* msg); // send() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void recv_msg(int idx); // recv() �Լ� �����. �ڼ��� ������ �Լ� �����ο��� Ȯ��.
void dm_send_msg(const string& sender, const char* msg, const string& recipientUser);
void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid);
void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue);
void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser);
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
void db_join_check(); // ȸ������ �� ���̵� üũ
void db_join_check_ver2(); // ä�� ��� �Է� �� ����� ���� ���� üũ
void db_UserEdit(); // ȸ�� ���� ������ �� ��й�ȣ Ȯ�ι�
void db_UserEdit_update(); // ȸ�� ���� ���� ������Ʈ��
void db_selectQuery(); //db ����Ʈ��
void db_login();
void db_countuser(); // (1) ���� �� �� ������? (���� ������ Ȱ��)
void db_userlist(); // (3) ���� ��� ���
void db_findID(); // (7) ���� ���� ã��
void db_callMessage(); // ���� ä�ù� �ҷ�����
void send_msg_2(const string& msg); 
void db_friend_list();
void db_friend_register();
void db_chat_list();
void db_chat_room();
void db_create_chatroom(string user_id_1, string user_id_2);

void dm_send_db(int server_request, const string& sender, const std::string& recipientUser, const std::string& user_2, const std::vector<std::vector<std::string>>& result);
void dm_send_dbup(int server_request, const string& sender, const std::string& recipientUser, const std::string& msg, const std::string& msg2);

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
    //mtx.lock();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1");
    int number = 0;
    while (res->next()) {
        number += 1;
    }
    //cout << " num " << number << endl; //��� ��ȣ Ȯ��.

    pstmt = con->prepareStatement("INSERT INTO message_room_1 (number, user_id, content, time) values(null,?,?,now())"); // INSERT
    pstmt->setString(1, tokens[1]); // ���� ��� ���̵�
    pstmt->setString(2, tokens[2]); // ���� �޼���
    pstmt->execute(); // �̰� �־���� ��� �����.

    cout << "�޼����� ����Ǿ����ϴ�." << endl;
    // 
    string msg = "update";
    cout << "������ �޼��� 2��" << endl;
    //
    std::vector<std::vector<std::string>> result;
    stmt = con->createStatement();
    res3 = stmt->executeQuery("SELECT * FROM message_room_1");
    string a = "1"; 
    string user_2;
    res2 = stmt->executeQuery("SELECT user_id_1, user_id_2 FROM chatroom WHERE room_num = '" + a + "'");
    cout << "��ū��� " << tokens[1] << endl;
    // ��� ���
    while (res2->next()) {
        //cout << "���� �������� �� ��ȣ " << res2->getString("room_num") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "���� 1�� ID : " << res2->getString("user_id_1") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "���� 2�� ID : " << res2->getString("user_id_2") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        string temp1 = res2->getString("user_id_1");
        if (tokens[1] == temp1) {
            user_2 = res2->getString("user_id_2");
        }
        else if (tokens[1] != temp1) {
            user_2 = res2->getString("user_id_1");
        }
    }


    while (res3->next()) {
        std::vector<std::string> row;
        row.push_back(res3->getString("number"));
        row.push_back(res3->getString("user_id"));
        row.push_back(res3->getString("content"));
        row.push_back(res3->getString("time"));
        result.push_back(row);
    }
    dm_send_db(5, "server", "0", user_2, result);
    Sleep(200);
    dm_send_db(5, "server", "1", user_2, result);
    //dm_send_dbup(52, "server", "0", msg, msg);
    //Sleep(1000);
    //dm_send_dbup(53, "server", "1", msg, msg);
    //mtx.unlock();
    delete stmt;
    // MySQL Connector/C++ ����
}

void db_selectQuery() {
    db_init();

    // SQL ���� ����
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1"); // from �ڿ��� ������ mysql ���� ����ϴ� ���̺��� �̸��� ����Ѵ�.
    delete stmt;

    cout << "\n";
    cout << "SQL ���� ���� (Ư�� PK ���� �ش��ϴ� �� ����) \n";
    // SQL ���� ���� (Ư�� PK ���� �ش��ϴ� �� ����)
    string pkValue = "abcd"; // ���� PK ������ ��ü
    //string query = "SELECT * FROM inventory WHERE id = '" + pkValue + "'";
    // ���̺� �̸�, PK �� �̸�
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1");//WHERE user_id = '" + pkValue + "'
    // ��� ���
    while (res->next()) {
        cout << "Column1: " << res->getString("number") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("user_id") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("content") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "Column2: " << res->getString("time") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
    }

    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;
}

void db_login() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    cout << test_count << "�� �־��." << endl;

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
            int str_test_count = stoi(test_count);
            sck_list[str_test_count].user = db_name;
            dm_send_result(server_request, "server", result, test_count, db_name, db_id);

            cout << sck_list[str_test_count].user << "Ȯ��@@@@@@@@@@@@@@@@@@@" << endl;
            cout << msg << " �� " << db_name << endl;
        }
        else if (db_id != tokens[1] || db_pw != tokens[2]) {
            string msg = " 636 line �� �α��� ���� ! ���̵� �Ǵ� ��й�ȣ�� Ȯ�����ּ���.";
            int result = 0;
            int server_request = 1;
            dm_send_result(server_request, "server", result, test_count, "�ӽ�����", "temp");
            cout << msg << endl;
        }
    }
    else {
        string msg = " �� 642line �α��� ���� ! ���̵� �Ǵ� ��й�ȣ�� Ȯ�����ּ���.";
        int result = 54321;
        int server_request = 1;
        dm_send_result(server_request, "server", result, test_count, "�ӽ�����", "temp");
        cout << msg << endl;
    }
    return;
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

    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber, nickname, friend_name FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();
    cout << "#323 ���������?" << endl;

    if (res->next()) {
        cout << "#326 �Է��� ���̵� ����" << endl;
        join_result = false;
        int result = 3;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[2],"temp");
    }
    else {
        cout << "#334 �Է��� ���̵� ����" << endl;

        stmt = con->createStatement();
        pstmt = con->prepareStatement("INSERT INTO users (user_id, name, pw, phonenumber, nickname, friend_name) values(?,?,?,?,?,?)"); // INSERT
        cout << "#336 ����?" << endl;

        pstmt->setString(1, tokens[1]); //���̵�
        pstmt->setString(2, tokens[2]); // �̸�
        pstmt->setString(3, tokens[3]); // ��й�ȣ
        pstmt->setString(4, tokens[4]); // ��ȭ��ȣ
        pstmt->setString(5, tokens[5]); // �г���
        pstmt->setString(6, " "); //ģ�����

        pstmt->execute(); // �̰� �־���� ��� �����.

        cout << "#349 here?" << endl;

        string msg = "�� ȸ������ ����!";
        join_result = true;
        int result = 1;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        sck_list[str_test_count].user = tokens[1];
        dm_send_result(server_request, "server", result, test_count, tokens[2],"temp");
        cout << "�ű� ���� ������ �Ϸ�Ǿ����ϴ�." << endl;
    }
}

void db_join_check() {
   
    pstmt = con->prepareStatement("SELECT user_id FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    if (res->next()) {
        cout << "#374 �Է��� ���̵� ����" << endl;
        join_check = false;
        user_check = true;
        int result = 4; // ���̵� üũ ����
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[1],"temp");
    }

    else {
        join_check = true;
        user_check = false;
        int result = 3; // ���̵� üũ ���� �����
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[1],"temp");
    }
}

void db_join_check_ver2() {

    pstmt = con->prepareStatement("SELECT user_id FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    if (res->next()) {
        cout << "#408 �Է��� ���̵� ����" << endl;
        user_check = true;
        int result = 4; 
        int server_request = 6;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, tokens[2]); // findValue = user_id
        
        db_chat_room();
    }

    else {
        cout << "�׹�° Ȯ��" << endl;
        user_check = false;
        int result = 3; // 
        int server_request = 6;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, tokens[2]); // findValue = user_id
    }
}

// ȸ�� ���� �����κ�
void db_UserEdit() {  
    // �����ͺ��̽����� ���� ��й�ȣ�� �������� ����
    string selectQuery = "SELECT pw FROM users WHERE user_id = ?";
    pstmt = con->prepareStatement(selectQuery);
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    if (res->next()) {
        string database_password = res->getString("pw");

        cout << database_password << " <- ���� ��й�ȣ�� " << endl;
        cout << tokens[1] << " <- ���� ��й�ȣ�� " << endl;

        // ����ڰ� �Է��� ��й�ȣ�� �����ͺ��̽��� ��й�ȣ ��
        if (tokens[1] == database_password) {
            // �Է��� ��й�ȣ�� �����ͺ��̽� ��й�ȣ�� ��ġ�ϸ� ������Ʈ ����
            cout << "Ȯ�� �Ǿ����ϴ�." << endl;
            //dm_send_result(server_request, "server", result, test_count, db_name, db_id);
            int result = 1;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
            cout << " 400���� " << result << " and " << test_count << endl;
        }
        else {
            cout << "�Է��� ��й�ȣ�� ��ġ���� �ʽ��ϴ�." << endl;
            int result = 2;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
            cout << " 400���� " << result << " and " << test_count << endl;

        }
    }
    else {
        cout << "���� : ������� �α��� ������ Ȯ���� �� �����ϴ�. (����� ���̵� ����)" << endl;
    }

    //cout << "Finished update table" << endl;

}

void db_UserEdit_update() {
    

	string updateQuery = "UPDATE users SET pw = ? WHERE user_id = ?";
	pstmt = con->prepareStatement(updateQuery);
	pstmt->setString(1, tokens[1]); //��ū��1�� �ٲܺ�й�ȣ
	pstmt->setString(2, tokens[2]); //��ū��2�� �������̵�
    pstmt->executeUpdate();
	
    cout << tokens[2] << " �� ��й�ȣ�� ���� �Ǿ����ϴ�." << endl;
    //dm_send_result(server_request, "server", result, test_count, db_name, db_id);
    int result = 3;
    int str_test_count = stoi(test_count);
    dm_send_resultEdit(8, "server", result, test_count);
    cout << " 400���� " << result << " and " << test_count << endl;

}

void db_selectQuery_ver2() {
    //mtx.lock();
    std::vector<std::vector<std::string>> result;
    cout << "ģ�� ����� �����ϴ�." << endl;
    db_init();
    // �����ͺ��̽� ���� ����
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1");
    string a = "1";
    string user_2;  

    res2 = stmt->executeQuery("SELECT user_id_1, user_id_2 FROM chatroom WHERE room_num = '" + a + "'");
    cout << "��ū��� " <<  tokens[1] << endl;
    // ��� ���
    while (res2->next()) {
        //cout << "���� �������� �� ��ȣ " << res2->getString("room_num") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "���� 1�� ID : " << res2->getString("user_id_1") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        cout << "���� 2�� ID : " << res2->getString("user_id_2") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
        string temp1 = res2->getString("user_id_1");
        if (tokens[1] == temp1) {
            user_2 = res2->getString("user_id_2");
        }
        else if (tokens[1] != temp1) {
            user_2 = res2->getString("user_id_1");
        }
    }
    

    while (res->next()) {
        std::vector<std::string> row;
        row.push_back(res->getString("number"));
        row.push_back(res->getString("user_id"));
        row.push_back(res->getString("content"));
        row.push_back(res->getString("time"));
        result.push_back(row);
    }

    /*for (const auto& row : result) {
        for (const std::string& value : row) {
            std::cout << value << " ";
        }
        std::cout << "���� �׽�Ʈ " << std::endl;
    }*/

    //cout << "\n\n" << endl;
    //int a = result.size();
    //const std::vector<std::string
    // >& row = result[3];
    //for (const std::string& value : row) {
    //    std::cout << value << " ";
    //}
    //cout << "\n" << endl;
    //cout << "����Ʈ ���ʹ� ũ�Ⱑ " << a << " �Դϴ�." << endl;
    //dm_send_findResult(server_request, "server", result, tokens[1], db_pw);\

    
    dm_send_db(5, "server", test_count, user_2, result);
    //dm_send_db(5, "server", "1", user_2, result);
    //mtx.unlock();
    delete stmt;
}

void dm_send_dbup(int server_request, const string& sender, const std::string& recipientUser, const std::string& msg2, const std::string& msg3) {
    string serv_request = std::to_string(server_request);
    
    string msg = serv_request + " " + sender + " " + recipientUser + " " + msg2 + "/" + msg3;
    for (int i = 0; i < client_count; i++) {
        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_dbup " << msg << endl;
            send(sck_list[i].sck, msg.c_str(), msg.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
            //}
        }
    }

    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}

void dm_send_db(int server_request, const string& sender, const std::string & recipientUser, const std::string& user_2, const std::vector<std::vector<std::string>>&result) {
    string serv_request = std::to_string(server_request);
    std::string resultStr;
    for (const std::vector<std::string>& row : result) {
        for (const std::string& value : row) {
            resultStr += value + " "; // ������ �����ڷ� ���
        }
        resultStr += "\n"; // �� ���� ���� ���ڷ� ����
    }
    string msg = serv_request + " " + sender + " " + recipientUser + " " + user_2 + "/" + resultStr;
    for (int i = 0; i < client_count; i++) {
        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_db " << msg << endl;
            send(sck_list[i].sck, msg.c_str(), msg.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
            //}
        }
    }

    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
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
// ���� �۾��� �Լ�
void db_findID() {
    cout << "tokens[0] : " << tokens[0] << endl;
    cout << "tokens[0] : " << tokens[1] << endl;
    cout << "tokens[0] : " << tokens[2] << endl;

    pstmt = con->prepareStatement("SELECT user_id, name, phonenumber FROM users WHERE phonenumber = ?");
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    // ����� �ִٸ�
    //dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser)
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_phonenumber = res->getString("phonenumber");

        // �����ͺ��̽��� ����� �����Ϳ� �Է¹��� �����Ͱ� �����ϴٸ�
        if (db_name == tokens[1] && db_phonenumber == tokens[2]) {
            int result = 1; // ���� �� �����
            int server_request = 2; // ID ã�� ��ȣ
            cout << " 485" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_id); // findValue = user_id
        }
        else {
            int result = 2; // ���� �� �����
            int server_request = 2; // ID ã�� ��ȣ
            string fail = "����";
            cout << " 492" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // ���� �� �����
        int server_request = 2; // ID ã�� ��ȣ
        string fail = "����";
        cout << " 500" << endl;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
    }
}

void db_findPW() {
    cout << "tokens[0] : " << tokens[0] << endl;
    cout << "tokens[1] : " << tokens[1] << endl;
    cout << "tokens[2] : " << tokens[2] << endl;
    cout << "tokens[3] : " << tokens[3] << endl;

    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    // ����� �ִٸ�
    //dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser)
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_pw = res->getString("pw");
        string db_phonenumber = res->getString("phonenumber");

        // �����ͺ��̽��� ����� �����Ϳ� �Է¹��� �����Ͱ� �����ϴٸ�
        if (db_id == tokens[1] && db_name == tokens[2] && db_phonenumber == tokens[3]) {
            int result = 1; // ���� �� �����
            int server_request = 3; // PW ã�� ��ȣ
            cout << " 565" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_pw); // findValue = user_id
        }
        else {
            int result = 2; // ���� �� �����
            int server_request = 3; // PW ã�� ��ȣ
            string fail = "����";
            cout << " 572" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // ���� �� �����
        int server_request = 2; // ID ã�� ��ȣ
        string fail = "����";
        cout << " 500" << endl;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
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

void db_friend_list() {

    cout << "tokens[1] : " << tokens[1] << endl;
    
    std::vector<std::string> f_lists; // ģ�� ��� ������ �迭
    std::string f_list;

    pstmt = con->prepareStatement("SELECT GROUP_CONCAT(friend_id SEPARATOR ' ') FROM friend_list WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    cout << "# 666 ���� ��?" << endl;
    if (res->next()) {
        cout << res->getString(1) << endl;
        f_list = res->getString(1); // ������ ��� ����

        if (f_list.empty() == false) {
            cout << f_list << endl;
            f_lists.push_back(f_list); // �迭�� ������ ��� ����

            cout << f_lists[0] << endl;
            cout << f_lists.size() << endl;

            int result = 1;
            int server_request = 71;
            cout << "# 679" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, f_lists[0]);
        }
        else if (f_list.empty() == true) {
            cout << "# 684" << endl;
            int result = 2;
            int server_request = 71;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, "temp");
        }
    }
}

void db_friend_register() {

    pstmt = con->prepareStatement("SELECT user_id FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    // ���� ���̺� �Է��� ģ�� ���̵� �����ϸ�
    if (res->next()) {
        pstmt = con->prepareStatement("SELECT friend_id FROM friend_list WHERE user_id = ? and friend_id = ?");
        pstmt->setString(1, tokens[1]);
        pstmt->setString(2, tokens[2]);
        res = pstmt->executeQuery();

        cout << "#645 ���� ?" << endl;
        // ģ�� ����Ʈ ���̺� ģ�� ���̵� �̹� �����Ѵٸ�
        if (res->next()) {
            int result = 2;
            int server_request = 7;
            cout << "#649" << endl;
            int str_test_count = stoi(test_count);
            dm_send_result(server_request, "server", result, test_count, "temp", tokens[1]);
        }
        else { // ������ ������� �����ư��� �� �� ����
            pstmt = con->prepareStatement("INSERT INTO friend_list (user_id, friend_id) VALUES (?, ?)");
            pstmt->setString(1, tokens[1]);
            pstmt->setString(2, tokens[2]);
            res = pstmt->executeQuery();
            pstmt2 = con->prepareStatement("INSERT INTO friend_list (user_id, friend_id) VALUES (?, ?)");
            pstmt2->setString(1, tokens[2]);
            pstmt2->setString(2, tokens[1]);
            res2 = pstmt2->executeQuery();
            cout << "#664" << endl;
            
			int result = 1;
			int server_request = 7;
			cout << "#662" << endl;
			int str_test_count = stoi(test_count);
			dm_send_result(server_request, "server", result, test_count, "temp", tokens[1]);

        }
    }
    // ���� ���̺� �Է��� ģ�� ���̵� �������� ���� ��
    else {
        int result = 3;
        int server_request = 7;
        cout << "#671" << endl;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, "temp", tokens[1]);
    }

}

void db_chat_list() {

    std::vector<std::string> chat_lists; // ģ�� ��� ������ �迭
    std::string chat_list_1, chat_list_2, msg;

    pstmt = con->prepareStatement("SELECT GROUP_CONCAT(user_id_2 SEPARATOR ' ') FROM chatroom WHERE user_id_1 = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();
    cout << "#751" << endl;

    pstmt2 = con->prepareStatement("SELECT GROUP_CONCAT(user_id_1 SEPARATOR ' ') FROM chatroom WHERE user_id_2 = ?");
    pstmt2->setString(1, tokens[1]);
    res2 = pstmt2->executeQuery();
    cout << "#756" << endl;

    if (res->next()) {
        chat_list_1 = res->getString(1);
        cout << "user_id_1 ���� : " << chat_list_1 << endl;
        if (chat_list_1.empty() != true) {
            chat_lists.push_back(chat_list_1);
        }
    }

    if (res2->next()) {
        chat_list_2 = res2->getString(1);
        cout << "user_id_2 ���� : " << chat_list_2 << endl;
        if (chat_list_2.empty() != true) {
            chat_lists.push_back(chat_list_2);
        }
    }
    
    if (chat_list_1.empty() == true && chat_list_2.empty() == true) {
        int result = 2;
        int server_request = 6;
        cout << " # 790 " << endl;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, "temp");
    }

    else {
        for (int i = 0; i < chat_lists.size(); i++) {
            msg += chat_lists[i] + " ";
        }

        int result = 1;
        int server_request = 6;
        cout << "#785" << endl;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, msg); // 
    }
}

void db_chat_room() {

    string chatroom_num, num, create_num;
    int a;

	pstmt = con->prepareStatement("SELECT room_num FROM chatroom WHERE user_id_1 = ? and user_id_2 = ? or user_id_1 = ? and user_id_2 = ?");
	pstmt->setString(1, tokens[1]);
	pstmt->setString(2, tokens[2]);
	pstmt->setString(3, tokens[2]);
	pstmt->setString(4, tokens[1]);
	res = pstmt->executeQuery();
	cout << " # 856 " << endl;

	if (res->next()) {
		chatroom_num = res->getString(1);
		cout << " # 861 : �� Ȯ�ο� : " << chatroom_num << endl;

		std::vector<std::vector<std::string>> result;
		string user_2;

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT user_id_1, user_id_2 FROM chatroom WHERE room_num = '" + chatroom_num + "'");
		cout << " # 870" << endl;
		// ��� ���
		while (res->next()) {
			//cout << "���� �������� �� ��ȣ " << res2->getString("room_num") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
			cout << "���� 1�� ID : " << res->getString("user_id_1") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
			cout << "���� 2�� ID : " << res->getString("user_id_2") << endl; // ("�ʵ��̸�")�� �����. �ʵ��̸� ���ϴ°�!
			string temp1 = res->getString("user_id_1");
			if (tokens[1] == temp1) {
				user_2 = res->getString("user_id_2");
			}
			else if (tokens[1] != temp1) {
				user_2 = res->getString("user_id_1");
			}
		}

		string query_msg = "message_room_" + chatroom_num;
		cout << "Ȯ�ο� : " << query_msg << endl;

		pstmt2 = con->prepareStatement("SELECT number, user_id, content, time FROM ?");
		pstmt2->setString(1, query_msg);
		res2 = pstmt2->executeQuery();
		cout << "# 891" << endl;

		while (res2->next()) {
			std::vector<std::string> row;
			row.push_back(res2->getString("number"));
			row.push_back(res2->getString("user_id"));
			row.push_back(res2->getString("content"));
			row.push_back(res2->getString("time"));
			result.push_back(row);
		}

		for (const auto& row : result) {
			for (const std::string& value : row) {
				std::cout << value << " ";
			}
			std::cout << "���� �׽�Ʈ " << std::endl;
		}

		dm_send_db(5, "server", test_count, user_2, result);

		delete stmt;

	}
	else {
        stmt = con->createStatement();
        pstmt = con->prepareStatement("INSERT INTO chatroom VALUES (null, ?, ?)");
        pstmt->setString(1, tokens[1]);
        pstmt->setString(2, tokens[2]);
        pstmt->executeQuery();
		cout << " # 902" << endl;

        db_create_chatroom(tokens[1], tokens[2]);

		int result = 4; // DB�� ��ȭ�� ���� ���� �����
		int server_request = 5;
		cout << "# 908" << endl;
		int str_test_count = stoi(test_count);
		dm_send_result(server_request, "server", result, test_count, "temp", tokens[1]);
	}

}

void db_create_chatroom(string user_id_1, string user_id_2) {
    pstmt = con->prepareStatement("SELECT room_num FROM chatroom WHERE user_id_1 = ? and user_id_2 = ?");
    pstmt->setString(1, user_id_1);
    pstmt->setString(2, user_id_2);

    res = pstmt->executeQuery();
    
    if (res->next()) {
        string create_num = res->getString(1);
        cout << " # 924 ��� : " << create_num << endl;
        string msg = "message_room_" + create_num;
        string query = "CREATE TABLE " + msg + " (number int primary key auto_increment, user_id VARCHAR(10) not null, content VARCHAR(255) not null, time datetime not null)";

        stmt = con->createStatement();
        stmt->execute(query);

        delete stmt;
        delete con;

        cout << " # 934 �� ��° Ȯ��" << endl;
    }
}

int main() {
    WSADATA wsa;
    system("title ����");
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
            //msg = buf;
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
    //db_selectQuery_ver2();

    //db_roomUserNameQuery();

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


    sck_list[client_count].login_status = false;
    sck_list[client_count].user_number = client_count;
    cout << "========================" << endl;
    cout << "���ο� ������ �����߽��ϴ�." << endl;
    cout << "sck_list[client_count].login_status = " << sck_list[client_count].login_status << endl;
    cout << "sck_list[client_count].user_number = " << sck_list[client_count].user_number << endl;
    cout << "========================" << endl;
    client_count++; // client �� ���� 
    th.join();
}

void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + username + " " + userid;
    for (int i = 0; i < client_count; i++) {

        /*cout << std::to_string(sck_list[i].user_number) << " ���� ������" << endl;
        cout << sck_list[i].user_number << " �� Ȯ�ο� " << endl;
        test_count = std::to_string(sck_list[i].user_number);*/
        cout << test_count << " �� Ȯ�ο� " << endl;

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_result " << result << endl;
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
            //}
        }
    }
    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}


void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser;
    for (int i = 0; i < client_count; i++) {

        /*cout << std::to_string(sck_list[i].user_number) << " ���� ������" << endl;
        cout << sck_list[i].user_number << " �� Ȯ�ο� " << endl;
        test_count = std::to_string(sck_list[i].user_number);*/
        cout << test_count << " �� Ȯ�ο� " << endl;

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_resultEdit " << result << endl;
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
            //}
        }
    }
    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}

void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + findValue;
    for (int i = 0; i < client_count; i++) {

        /*cout << std::to_string(sck_list[i].user_number) << " ���� ������" << endl;
        cout << sck_list[i].user_number << " �� Ȯ�ο� " << endl;
        test_count = std::to_string(sck_list[i].user_number);*/
        cout << test_count << " �� Ȯ�ο� " << endl;

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_findResult " << result << endl;
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
            cout << "dm_send_msg " << msg << endl;
            send(sck_list[i].sck, msg, MAX_SIZE, 0);
            return; // Ư�� ����ڿ��� �޽����� ������ �Լ� ����
            //}
        }
    }

    // ����ڸ� ã�� ���� ���, ���� �޽��� ��� �Ǵ� �ٸ� ó���� �߰��� �� �ֽ��ϴ�.
}

void send_msg_2(const string& msg) {
    for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
        cout << sck_list[i].sck << " ���� �������������" << endl;
        send(sck_list[i].sck, msg.c_str(), msg.length(), 0);
    }
    return;
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) { // ������ �ִ� ��� client���� �޽��� ����
        cout << "send_msg " << msg << endl;
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}

void recv_msg(int idx) {
    //char buf[MAX_SIZE] = { };
    string msg = "";
    //cout << sck_list[idx].user << endl;
    while (1) {
        char buf[MAX_SIZE] = { };
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // ������ �߻����� ������ recv�� ���ŵ� ����Ʈ ���� ��ȯ. 0���� ũ�ٴ� ���� �޽����� �Դٴ� ��.
            cout << "========================" << endl;
            //mtx.lock();
            msg = buf; //sck_list[idx].user
            cout << sck_list[idx].user_number << " �� buf �� = " << msg << endl;
            //send_msg(msg.c_str());
            std::istringstream iss(buf);
            tokens.clear(); // ���� ��ū�� ����� ���� ���۾��ϸ� �� �����Ǽ� ����� �ν� ���� �ФФФФ�
            test_count = std::to_string(sck_list[idx].user_number);
            while (iss >> token) {
                tokens.push_back(token);
            }
            // ��ū0 �������� 1:�α��� / 2:idã�� / 3:pwã�� / 4:ȸ������ / 5:��ȭ / 6:����ä�� / 7:ģ�� / 8:�������
            // tokens[0] == 1 �̸� �α��� ��û
            if (tokens[0] == "1") {
                cout << tokens[1] << " ��ū[1]�� ���̵����� �������� �α��� ��û�� ���Խ��ϴ�." << endl;
                db_init();
                db_login();
            };
            // tokens[0] == 2 �̸� ���̵� ã�� ��û
            if (tokens[0] == "2") {
                cout << tokens[1] << " ȸ���� ���̵� ã�� ����� ��û�߽��ϴ�." << endl;
                db_init();
                db_findID();
            };
            // tokens[0] == 3 �̸� ��й�ȣ ã�� ��û
            if (tokens[0] == "3") {
                cout << tokens[1] << " ȸ���� ��й�ȣ ã�� ����� ��û�߽��ϴ�." << endl;
                db_init();
                db_findPW();
            };
            // tokens[0] == 4 �̸� ȸ������ ��û
            if (tokens[0] == "4") {
                cout << " ȸ������ ��û�� ���Խ��ϴ�." << endl;
                db_init();
                db_join();
            };
            if (tokens[0] == "41") {
                cout << " ���̵� Ȯ�� ��û�� ���Խ��ϴ�." << endl;
                db_init();
                db_join_check();                
            };

            // tokens[0] == 5 �̸� ��ȭ��� ��û
            if (tokens[0] == "5") {
                Sleep(300);
                cout << tokens[1] << " ȸ���� ģ�� ����� ��û�߽��ϴ�." << endl;
                db_init();
                db_selectQuery_ver2();
            };
            // tokens[0] == 51 �̸� ���� �޽��� ���� ��û
            if (tokens[0] == "51") {
                Sleep(300);
                cout << tokens[1] << " ȸ���� �޼��� ������ ��û�߽��ϴ�." << endl;
                db_messageSend();
                //Sleep(2000);
                //db_selectQuery_ver2();
            };

            // tokens[0] == 6 �̸� ���� ä�ù� ��û
            if (tokens[0] == "6") {
                cout << tokens[1] << " ȸ���� ������ �⺻ ä�ù��� ��û�߽��ϴ�." << endl;
                db_init();
                db_chat_list();
            };

            if (tokens[0] == "61") {
                cout << tokens[1] << " ȸ���� ������ ä�ù��� ��û�߽��ϴ�." << endl;
                db_init();
                db_join_check_ver2();
                if (!user_check) {
                    db_chat_room();
                }
            };

            // tokens[0] == 7 �̸� ģ�� �߰� ��� ��û
            if (tokens[0] == "7") {
                cout << tokens[1] << " ȸ���� ģ�� �߰� ����� ��û�߽��ϴ�." << endl;
                db_init();
                db_friend_register();
            };

            if (tokens[0] == "71") {
                cout << tokens[1] << " ȸ���� ģ�� ��� Ȯ�� ����� ��û�߽��ϴ�." << endl;
                db_init();
                db_friend_list();
            };
            // tokens[0] == 8 �̸� ��й�ȣ ���� ��û
            if (tokens[0] == "8") {
                if (tokens[3] == "N") { //tokens[0] �� 8 �̸鼭 tokens[3] �� ���� Y�� N���� ��й�ȣ Ȯ�� ����� �����մϴ�.
                    cout << tokens[1] << " [��й�ȣ Ȯ�� ��û] ��ū[1]�� ��й�ȣ������ �������� ��й�ȣ ���� ��û�� ���Խ��ϴ�." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit(); // �����ͺ��̽� ���� ����
                    tokens.clear(); // ���� ��ū�� ����� ���� ���۾��ϸ� �� �����Ǽ� ����� �ν� ���� �ФФФФ�
                }
                if (tokens[3] == "Y") {
                    cout << tokens[1] << " [��й�ȣ Ȯ�� �Ϸ�] ��ū[1]�� ��й�ȣ������ �������� ��й�ȣ ���� ��û�� ���Խ��ϴ�." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit_update();
                    tokens.clear(); // ���� ��ū�� ����� ���� ���۾��ϸ� �� �����Ǽ� ����� �ν� ���� �ФФФФ�
                }
            }
            //mtx.unlock();
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
