#pragma comment(lib, "ws2_32.lib") //명시적인 라이브러리의 링크. 윈속 라이브러리 참조
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h> // C++하고 MYSQL을 연결하기 위해 선언.
#include <sstream> // 요구사항 분리해서 저장하기 위해 필요함.
#include <WS2tcpip.h> // 소켓 전송을 위해 사용
#include <mutex>

#define MAX_SIZE 1024
#define MAX_CLIENT 3

using std::cout;
using std::cin;
using std::endl;
using std::string;

// MY SQL 관련 정보를 저장하는 선언하는 곳 //
const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "root"; // 데이터베이스 사용자
const string password = "1234"; // 데이터베이스 접속 비밀번호

// MySQL Connector/C++ 초기화
sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
sql::Connection* con;
sql::Statement* stmt;
sql::Statement* stmt2;
sql::PreparedStatement* pstmt;
sql::PreparedStatement* pstmt2;
sql::ResultSet* res; //결과값을 위해
sql::ResultSet* res2; //결과값을 위해
sql::ResultSet* res3; //결과값을 위해

SOCKET client_sock;

struct SOCKET_INFO { // 연결된 소켓 정보에 대한 틀 생성
    SOCKET sck;
    string user;
    int user_number;
    bool login_status = false;
};

std::vector<SOCKET_INFO> sck_list; // 연결된 클라이언트 소켓들을 저장할 배열 선언.
SOCKET_INFO server_sock; // 서버 소켓에 대한 정보를 저장할 변수 선언.

std::vector<std::string> tokens; // 소켓에 대한 정보를 담기 위한 토큰 배열
std::string token; //토큰 배열
std::mutex mtx;

int client_count = 0; // 현재 접속해 있는 클라이언트를 count 할 변수 선언.
string test_count;
bool login_result = false;
bool request_result = false;
bool join_result = false;
bool join_check = false;
bool user_check = false;
void server_init(); // socket 초기화 함수. socket(), bind(), listen() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void add_client(); // 소켓에 연결을 시도하는 client를 추가(accept)하는 함수. client accept() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void send_msg(const char* msg); // send() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void recv_msg(int idx); // recv() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void dm_send_msg(const string& sender, const char* msg, const string& recipientUser);
void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid);
void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue);
void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser);
int chat_recv();

// 디비 구분
void del_client(int idx); // 소켓에 연결되어 있는 client를 제거하는 함수. closesocket() 실행됨. 자세한 내용은 함수 구현부에서 확인.
// MYSQL DB 관련 쿼리문 함수 구현부 //
// 나중에 총 인원수 세기 등 함수 추가할 필요성 있음!! init 과 delete로 디비 정리 필수. //
void db_init(); // db 베이스 선택 및 한글 세팅 관련 부분 구현
void db_createQuery(); // db 크리에이트 쿼리 + 굳이 필요하지는 않다.
void db_insertQuery(); // db 인서트 쿼리 + 세분화가 필요하다.
void db_dropQuery(); // db 드롭 쿼리 + 굳이 필요하지는 않다.
void db_updateQuery(); // db 업데이트 쿼리

void db_selectQuery_ver2(); // db 셀렉트문
void db_roomUserNameQuery(); //채팅방에 있는 유저 이름 가져오기
void db_messageSend(); // 메시지 전송 저장
void db_join(); //회원가입
void db_join_check(); // 회원가입 전 아이디 체크
void db_join_check_ver2(); // 채팅 상대 입력 후 사용자 존재 여부 체크
void db_UserEdit(); // 회원 정보 수정할 때 비밀번호 확인문
void db_UserEdit_update(); // 회원 정보 수정 업데이트문
void db_selectQuery(); //db 셀랙트문
void db_login();
void db_countuser(); // (1) 유저 수 몇 명인지? (서버 공지로 활용)
void db_userlist(); // (3) 유저 목록 출력
void db_findID(); // (7) 유저 정보 찾기
void db_callMessage(); // 기존 채팅방 불러오기
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
    // 데이터베이스 선택
    con->setSchema("test");
    // db 한글 저장을 위한 셋팅
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }
}
//굳이 필요하지 않음.
void db_createQuery() {
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    stmt->execute("CREATE TABLE users (user_id VARCHAR(10) primary key not null, name VARCHAR(10) not null, pw VARCHAR(10) not null, phonenumber VARCHAR(20) not null, nickname VARCHAR(10) not null, friend_name VARCHAR(10));"); // user 테이블
    stmt->execute("CREATE TABLE chatroom (room_num int primary key auto_increment, user_id_1 VARCHAR(10) not null, user_id_2 VARCHAR(10) not null, messageDB_num int, foreign key(user_id_1) references users(user_id) on update cascade on delete cascade, foreign key(user_id_2) references users(user_id) on update cascade on delete cascade);"); // chatroom 테이블
    stmt->execute("CREATE TABLE message_room (	number int primary key auto_increment, user_id VARCHAR(10) not null, content VARCHAR(255) not null, time date not null, chatroom_num int not null, foreign key(chatroom_num) references chatroom(room_num) on update cascade on delete cascade);"); // message_room 테이블
    cout << "Finished creating table" << endl;
    delete stmt;
}
//인서트의 세분화가 필요함.
void db_insertQuery() { //일단 입력만 받아서 채워지는지 확인
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    pstmt = con->prepareStatement("INSERT INTO inventory(name, quantity) VALUES(?,?)"); // INSERT
    cout << "Finished inserting table" << endl;
    delete stmt;
    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;
}
//굳이 필요하지 않을 것 같음.
void db_dropQuery() {
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    stmt->execute("DROP TABLE IF EXISTS inventory"); // DROP
    cout << "Finished dropping table (if existed)" << endl;
    delete stmt;
    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;
}
//쿼리문 수정 필요함


void db_updateQuery() {
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    stmt->execute("UPDATE TABLE IF EXISTS inventory"); // UPDATE
    cout << "Finished update table" << endl;
    delete stmt;
    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;
}


//채팅방에 있는 유저 이름 가져오기
void db_roomUserNameQuery() {
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT room_num, user_id_1, user_id_2 FROM chatroom"); // from 뒤에는 실제로 mysql 에서 사용하는 테이블의 이름을 써야한다.
    delete stmt;

    // 결과 출력
    while (res->next()) {
        cout << "현재 접속중인 방 번호 " << res->getString("room_num") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "유저 1의 ID : " << res->getString("user_id_1") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "유저 2의 ID : " << res->getString("user_id_2") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
    }

    string User_Choice = "2";
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM chatroom WHERE room_num = '" + User_Choice + "'");


    // 결과 출력
    while (res->next()) {
        cout << User_Choice << " 라는 원하는 방에 참가중인 참여자 : " << res->getString("user_id_1") << "님 과 " << res->getString("user_id_2") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
    }

    // 유저가 참여중인 대화방만 불러오기
    string login_User = "abcd";
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT room_num FROM chatroom WHERE user_id_1 = '" + login_User + "'");

    cout << " 구분선 2" << endl;
    // 결과 출력
    while (res->next()) {
        cout << login_User << " 가 참여중인 방 번호 : " << res->getString("room_num") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        string a = res->getString("room_num");

        cout << "a가 저장되었나? = " << a << endl;
        res2 = stmt->executeQuery("SELECT user_id_2 FROM chatroom WHERE room_num = '" + a + "'");
        while (res2->next()) {
            cout << a << " 에서 " << res2->getString("user_id_2") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        }

    }


}


// 메시지 전송 저장
void db_messageSend() {
    db_init();
    //mtx.lock();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1");
    int number = 0;
    while (res->next()) {
        number += 1;
    }
    //cout << " num " << number << endl; //디비 번호 확인.

    pstmt = con->prepareStatement("INSERT INTO message_room_1 (number, user_id, content, time) values(null,?,?,now())"); // INSERT
    pstmt->setString(1, tokens[1]); // 보낸 사람 아이디
    pstmt->setString(2, tokens[2]); // 보낸 메세지
    pstmt->execute(); // 이거 있어야지 디비에 저장됨.

    cout << "메세지가 저장되었습니다." << endl;
    // 
    string msg = "update";
    cout << "보낸다 메세지 2개" << endl;
    //
    std::vector<std::vector<std::string>> result;
    stmt = con->createStatement();
    res3 = stmt->executeQuery("SELECT * FROM message_room_1");
    string a = "1"; 
    string user_2;
    res2 = stmt->executeQuery("SELECT user_id_1, user_id_2 FROM chatroom WHERE room_num = '" + a + "'");
    cout << "토큰즈원 " << tokens[1] << endl;
    // 결과 출력
    while (res2->next()) {
        //cout << "현재 접속중인 방 번호 " << res2->getString("room_num") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "유저 1의 ID : " << res2->getString("user_id_1") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "유저 2의 ID : " << res2->getString("user_id_2") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
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
    // MySQL Connector/C++ 정리
}

void db_selectQuery() {
    db_init();

    // SQL 쿼리 실행
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1"); // from 뒤에는 실제로 mysql 에서 사용하는 테이블의 이름을 써야한다.
    delete stmt;

    cout << "\n";
    cout << "SQL 쿼리 실행 (특정 PK 값에 해당하는 행 선택) \n";
    // SQL 쿼리 실행 (특정 PK 값에 해당하는 행 선택)
    string pkValue = "abcd"; // 실제 PK 값으로 대체
    //string query = "SELECT * FROM inventory WHERE id = '" + pkValue + "'";
    // 테이블 이름, PK 열 이름
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1");//WHERE user_id = '" + pkValue + "'
    // 결과 출력
    while (res->next()) {
        cout << "Column1: " << res->getString("number") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("user_id") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("content") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("time") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
    }

    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;
}

void db_login() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    cout << test_count << "에 넣어요." << endl;

    // 결과가 있다면
    //dm_send_result(const string& sender, int variable, const string& recipientUser)
    if (res->next()) {
        string db_id = res->getString("user_id"); // 데이터베이스의 id 저장
        string db_pw = res->getString("pw"); // 데이터베이스의 비밀번호 저장
        string db_name = res->getString("name"); // 데이터베이스의 이름 저장

        // 데이터베이스에 저장된 데이터와 입력받은 데이터가 동일하다면
        if (db_id == tokens[1] && db_pw == tokens[2]) {
            string msg = "※로그인 성공!";
            login_result = true;
            int result = 1;
            int server_request = 1;
            int str_test_count = stoi(test_count);
            sck_list[str_test_count].user = db_name;
            dm_send_result(server_request, "server", result, test_count, db_name, db_id);

            cout << sck_list[str_test_count].user << "확인@@@@@@@@@@@@@@@@@@@" << endl;
            cout << msg << " 랑 " << db_name << endl;
        }
        else if (db_id != tokens[1] || db_pw != tokens[2]) {
            string msg = " 636 line ※ 로그인 실패 ! 아이디 또는 비밀번호를 확인해주세요.";
            int result = 0;
            int server_request = 1;
            dm_send_result(server_request, "server", result, test_count, "임시유저", "temp");
            cout << msg << endl;
        }
    }
    else {
        string msg = " ※ 642line 로그인 실패 ! 아이디 또는 비밀번호를 확인해주세요.";
        int result = 54321;
        int server_request = 1;
        dm_send_result(server_request, "server", result, test_count, "임시유저", "temp");
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
        cout << "[공지] 현재 가입된 유저 수 : " << res->getString("count(*)") << endl;
    }

    delete pstmt;
    delete con;
}

//회원가입
void db_join() {

    pstmt = con->prepareStatement("SELECT user_id, name, pw, phonenumber, nickname, friend_name FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();
    cout << "#323 여기까지는?" << endl;

    if (res->next()) {
        cout << "#326 입력한 아이디 존재" << endl;
        join_result = false;
        int result = 3;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[2],"temp");
    }
    else {
        cout << "#334 입력한 아이디 없음" << endl;

        stmt = con->createStatement();
        pstmt = con->prepareStatement("INSERT INTO users (user_id, name, pw, phonenumber, nickname, friend_name) values(?,?,?,?,?,?)"); // INSERT
        cout << "#336 여기?" << endl;

        pstmt->setString(1, tokens[1]); //아이디
        pstmt->setString(2, tokens[2]); // 이름
        pstmt->setString(3, tokens[3]); // 비밀번호
        pstmt->setString(4, tokens[4]); // 전화번호
        pstmt->setString(5, tokens[5]); // 닉네임
        pstmt->setString(6, " "); //친구목록

        pstmt->execute(); // 이거 있어야지 디비에 저장됨.

        cout << "#349 here?" << endl;

        string msg = "※ 회원가입 성공!";
        join_result = true;
        int result = 1;
        int server_request = 4;
        int str_test_count = stoi(test_count);
        sck_list[str_test_count].user = tokens[1];
        dm_send_result(server_request, "server", result, test_count, tokens[2],"temp");
        cout << "신규 계정 생성이 완료되었습니다." << endl;
    }
}

void db_join_check() {
   
    pstmt = con->prepareStatement("SELECT user_id FROM users WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    if (res->next()) {
        cout << "#374 입력한 아이디 존재" << endl;
        join_check = false;
        user_check = true;
        int result = 4; // 아이디 체크 실패
        int server_request = 4;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, tokens[1],"temp");
    }

    else {
        join_check = true;
        user_check = false;
        int result = 3; // 아이디 체크 성공 결과값
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
        cout << "#408 입력한 아이디 존재" << endl;
        user_check = true;
        int result = 4; 
        int server_request = 6;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, tokens[2]); // findValue = user_id
        
        db_chat_room();
    }

    else {
        cout << "네번째 확인" << endl;
        user_check = false;
        int result = 3; // 
        int server_request = 6;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, tokens[2]); // findValue = user_id
    }
}

// 회원 정보 수정부분
void db_UserEdit() {  
    // 데이터베이스에서 현재 비밀번호를 가져오는 쿼리
    string selectQuery = "SELECT pw FROM users WHERE user_id = ?";
    pstmt = con->prepareStatement(selectQuery);
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    if (res->next()) {
        string database_password = res->getString("pw");

        cout << database_password << " <- 현재 비밀번호임 " << endl;
        cout << tokens[1] << " <- 현재 비밀번호임 " << endl;

        // 사용자가 입력한 비밀번호와 데이터베이스의 비밀번호 비교
        if (tokens[1] == database_password) {
            // 입력한 비밀번호와 데이터베이스 비밀번호가 일치하면 업데이트 수행
            cout << "확인 되었습니다." << endl;
            //dm_send_result(server_request, "server", result, test_count, db_name, db_id);
            int result = 1;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
            cout << " 400라인 " << result << " and " << test_count << endl;
        }
        else {
            cout << "입력한 비밀번호가 일치하지 않습니다." << endl;
            int result = 2;
            int str_test_count = stoi(test_count);
            dm_send_resultEdit(8, "server", result, test_count);
            cout << " 400라인 " << result << " and " << test_count << endl;

        }
    }
    else {
        cout << "에러 : 사용자의 로그인 정보를 확인할 수 없습니다. (사용자 아이디값 변질)" << endl;
    }

    //cout << "Finished update table" << endl;

}

void db_UserEdit_update() {
    

	string updateQuery = "UPDATE users SET pw = ? WHERE user_id = ?";
	pstmt = con->prepareStatement(updateQuery);
	pstmt->setString(1, tokens[1]); //토큰즈1이 바꿀비밀번호
	pstmt->setString(2, tokens[2]); //토큰즈2가 유저아이디
    pstmt->executeUpdate();
	
    cout << tokens[2] << " 의 비밀번호가 변경 되었습니다." << endl;
    //dm_send_result(server_request, "server", result, test_count, db_name, db_id);
    int result = 3;
    int str_test_count = stoi(test_count);
    dm_send_resultEdit(8, "server", result, test_count);
    cout << " 400라인 " << result << " and " << test_count << endl;

}

void db_selectQuery_ver2() {
    //mtx.lock();
    std::vector<std::vector<std::string>> result;
    cout << "친구 목록을 보냅니다." << endl;
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room_1");
    string a = "1";
    string user_2;  

    res2 = stmt->executeQuery("SELECT user_id_1, user_id_2 FROM chatroom WHERE room_num = '" + a + "'");
    cout << "토큰즈원 " <<  tokens[1] << endl;
    // 결과 출력
    while (res2->next()) {
        //cout << "현재 접속중인 방 번호 " << res2->getString("room_num") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "유저 1의 ID : " << res2->getString("user_id_1") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "유저 2의 ID : " << res2->getString("user_id_2") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
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
        std::cout << "백터 테스트 " << std::endl;
    }*/

    //cout << "\n\n" << endl;
    //int a = result.size();
    //const std::vector<std::string
    // >& row = result[3];
    //for (const std::string& value : row) {
    //    std::cout << value << " ";
    //}
    //cout << "\n" << endl;
    //cout << "리절트 백터는 크기가 " << a << " 입니다." << endl;
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
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
            //}
        }
    }

    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}

void dm_send_db(int server_request, const string& sender, const std::string & recipientUser, const std::string& user_2, const std::vector<std::vector<std::string>>&result) {
    string serv_request = std::to_string(server_request);
    std::string resultStr;
    for (const std::vector<std::string>& row : result) {
        for (const std::string& value : row) {
            resultStr += value + " "; // 공백을 구분자로 사용
        }
        resultStr += "\n"; // 각 행을 개행 문자로 구분
    }
    string msg = serv_request + " " + sender + " " + recipientUser + " " + user_2 + "/" + resultStr;
    for (int i = 0; i < client_count; i++) {
        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_db " << msg << endl;
            send(sck_list[i].sck, msg.c_str(), msg.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
            //}
        }
    }

    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}


void db_userlist() {
    db_init();
    cout << "\n";
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM users");

    cout << "[현재 가입된 유저 목록]" << endl;
    while (res->next()) {
        cout << "ID : " << res->getString("user_id") << endl;
    }
}
// 새로 작업한 함수
void db_findID() {
    cout << "tokens[0] : " << tokens[0] << endl;
    cout << "tokens[0] : " << tokens[1] << endl;
    cout << "tokens[0] : " << tokens[2] << endl;

    pstmt = con->prepareStatement("SELECT user_id, name, phonenumber FROM users WHERE phonenumber = ?");
    pstmt->setString(1, tokens[2]);
    res = pstmt->executeQuery();

    // 결과가 있다면
    //dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser)
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_phonenumber = res->getString("phonenumber");

        // 데이터베이스에 저장된 데이터와 입력받은 데이터가 동일하다면
        if (db_name == tokens[1] && db_phonenumber == tokens[2]) {
            int result = 1; // 성공 시 결과값
            int server_request = 2; // ID 찾기 번호
            cout << " 485" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_id); // findValue = user_id
        }
        else {
            int result = 2; // 실패 시 결과값
            int server_request = 2; // ID 찾기 번호
            string fail = "실패";
            cout << " 492" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // 실패 시 결과값
        int server_request = 2; // ID 찾기 번호
        string fail = "실패";
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

    // 결과가 있다면
    //dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser)
    if (res->next()) {
        string db_id = res->getString("user_id");
        string db_name = res->getString("name");
        string db_pw = res->getString("pw");
        string db_phonenumber = res->getString("phonenumber");

        // 데이터베이스에 저장된 데이터와 입력받은 데이터가 동일하다면
        if (db_id == tokens[1] && db_name == tokens[2] && db_phonenumber == tokens[3]) {
            int result = 1; // 성공 시 결과값
            int server_request = 3; // PW 찾기 번호
            cout << " 565" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, db_pw); // findValue = user_id
        }
        else {
            int result = 2; // 실패 시 결과값
            int server_request = 3; // PW 찾기 번호
            string fail = "실패";
            cout << " 572" << endl;
            int str_test_count = stoi(test_count);
            dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
        }
    }
    else {
        int result = 2; // 실패 시 결과값
        int server_request = 2; // ID 찾기 번호
        string fail = "실패";
        cout << " 500" << endl;
        int str_test_count = stoi(test_count);
        dm_send_findResult(server_request, "server", result, test_count, fail); // findValue = fail
    }
}

void db_callMessage() {
    db_init();

    string num;
    cout << "불러올 채팅방 번호를 입력하세요. : "; // 채팅방 번호도 유저 정보에서 출력해줘야할 듯
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
    
    std::vector<std::string> f_lists; // 친구 목록 저장할 배열
    std::string f_list;

    pstmt = con->prepareStatement("SELECT GROUP_CONCAT(friend_id SEPARATOR ' ') FROM friend_list WHERE user_id = ?");
    pstmt->setString(1, tokens[1]);
    res = pstmt->executeQuery();

    cout << "# 666 여기 됨?" << endl;
    if (res->next()) {
        cout << res->getString(1) << endl;
        f_list = res->getString(1); // 쿼리문 결과 저장

        if (f_list.empty() == false) {
            cout << f_list << endl;
            f_lists.push_back(f_list); // 배열에 쿼리문 결과 삽입

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

    // 유저 테이블에 입력한 친구 아이디가 존재하면
    if (res->next()) {
        pstmt = con->prepareStatement("SELECT friend_id FROM friend_list WHERE user_id = ? and friend_id = ?");
        pstmt->setString(1, tokens[1]);
        pstmt->setString(2, tokens[2]);
        res = pstmt->executeQuery();

        cout << "#645 여기 ?" << endl;
        // 친구 리스트 테이블에 친구 아이디가 이미 존재한다면
        if (res->next()) {
            int result = 2;
            int server_request = 7;
            cout << "#649" << endl;
            int str_test_count = stoi(test_count);
            dm_send_result(server_request, "server", result, test_count, "temp", tokens[1]);
        }
        else { // 동일한 사람들을 번갈아가며 두 번 저장
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
    // 유저 테이블에 입력한 친구 아이디가 존재하지 않을 때
    else {
        int result = 3;
        int server_request = 7;
        cout << "#671" << endl;
        int str_test_count = stoi(test_count);
        dm_send_result(server_request, "server", result, test_count, "temp", tokens[1]);
    }

}

void db_chat_list() {

    std::vector<std::string> chat_lists; // 친구 목록 저장할 배열
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
        cout << "user_id_1 기준 : " << chat_list_1 << endl;
        if (chat_list_1.empty() != true) {
            chat_lists.push_back(chat_list_1);
        }
    }

    if (res2->next()) {
        chat_list_2 = res2->getString(1);
        cout << "user_id_2 기준 : " << chat_list_2 << endl;
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
		cout << " # 861 : 값 확인용 : " << chatroom_num << endl;

		std::vector<std::vector<std::string>> result;
		string user_2;

		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT user_id_1, user_id_2 FROM chatroom WHERE room_num = '" + chatroom_num + "'");
		cout << " # 870" << endl;
		// 결과 출력
		while (res->next()) {
			//cout << "현재 접속중인 방 번호 " << res2->getString("room_num") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
			cout << "유저 1의 ID : " << res->getString("user_id_1") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
			cout << "유저 2의 ID : " << res->getString("user_id_2") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
			string temp1 = res->getString("user_id_1");
			if (tokens[1] == temp1) {
				user_2 = res->getString("user_id_2");
			}
			else if (tokens[1] != temp1) {
				user_2 = res->getString("user_id_1");
			}
		}

		string query_msg = "message_room_" + chatroom_num;
		cout << "확인용 : " << query_msg << endl;

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
			std::cout << "백터 테스트 " << std::endl;
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

		int result = 4; // DB에 대화방 생성 성공 결과값
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
        cout << " # 924 결과 : " << create_num << endl;
        string msg = "message_room_" + create_num;
        string query = "CREATE TABLE " + msg + " (number int primary key auto_increment, user_id VARCHAR(10) not null, content VARCHAR(255) not null, time datetime not null)";

        stmt = con->createStatement();
        stmt->execute(query);

        delete stmt;
        delete con;

        cout << " # 934 두 번째 확인" << endl;
    }
}

int main() {
    WSADATA wsa;
    system("title 서버");
    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        server_init();
        std::thread th1[MAX_CLIENT];
        for (int i = 0; i < MAX_CLIENT; i++) {
            // 인원 수 만큼 thread 생성해서 각각의 클라이언트가 동시에 소통할 수 있도록 함.
            th1[i] = std::thread(add_client);
        }
        //recv_msg(sck_list[client_count].user_number);
        //std::thread th1(add_client); // 이렇게 하면 하나의 client만 받아짐...
        while (1) { // 무한 반복문을 사용하여 서버가 계속해서 채팅 보낼 수 있는 상태를 만들어 줌. 반복문을 사용하지 않으면 한 번만 보낼 수 있음.            
            string text, msg = "";
            std::getline(cin, text);
            const char* buf = text.c_str();
            //msg = buf;
            msg = server_sock.user + " : " + buf;
            send_msg(msg.c_str());
        }
        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i].join();
            //join : 해당하는 thread 들이 실행을 종료하면 리턴하는 함수.
            //join 함수가 없으면 main 함수가 먼저 종료되어서 thread가 소멸하게 됨.
            //thread 작업이 끝날 때까지 main 함수가 끝나지 않도록 해줌.
        }
        //th1.join();
        //th.join();
        closesocket(server_sock.sck);
    }
    else {
        cout << "프로그램 종료. (Error code : " << code << ")";
    }

    
    WSACleanup();
    return 0;

}
void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Internet의 Stream 방식으로 소켓 생성
   // SOCKET_INFO의 소켓 객체에 socket 함수 반환값(디스크립터 저장)
   // 인터넷 주소체계, 연결지향, TCP 프로토콜 쓸 것.
    SOCKADDR_IN server_addr = {}; // 소켓 주소 설정 변수
    // 인터넷 소켓 주소체계 server_addr
    server_addr.sin_family = AF_INET; // 소켓은 Internet 타입
    server_addr.sin_port = htons(7777); // 서버 포트 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 서버이기 때문에 local 설정한다.
    //Any인 경우는 호스트를 127.0.0.1로 잡아도 되고 localhost로 잡아도 되고 양쪽 다 허용하게 할 수 있따. 그것이 INADDR_ANY이다.
    //ip 주소를 저장할 땐 server_addr.sin_addr.s_addr -- 정해진 모양?
    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // 설정된 소켓 정보를 소켓에 바인딩한다.
    listen(server_sock.sck, SOMAXCONN); // 소켓을 대기 상태로 기다린다.
    server_sock.user = "server";
    cout << "Server On" << endl;
    //db_selectQuery_ver2();

    //db_roomUserNameQuery();

}

void add_client() {

    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };
    ZeroMemory(&addr, addrsize); // addr의 메모리 영역을 0으로 초기화

    SOCKET_INFO new_client = {};
    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    // Winsock2의 recv 함수. client가 보낸 닉네임을 받음.
    //new_client.user = string(buf);
    cout << "buf = > " << buf << endl;

    sck_list.push_back(new_client); // client 정보를 답는 sck_list 배열에 새로운 client 추가        
    std::thread th(recv_msg, client_count);


    sck_list[client_count].login_status = false;
    sck_list[client_count].user_number = client_count;
    cout << "========================" << endl;
    cout << "새로운 유저가 접근했습니다." << endl;
    cout << "sck_list[client_count].login_status = " << sck_list[client_count].login_status << endl;
    cout << "sck_list[client_count].user_number = " << sck_list[client_count].user_number << endl;
    cout << "========================" << endl;
    client_count++; // client 수 증가 
    th.join();
}

void dm_send_result(int server_request, const string& sender, int variable, const string& recipientUser, const string& username, const string& userid) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + username + " " + userid;
    for (int i = 0; i < client_count; i++) {

        /*cout << std::to_string(sck_list[i].user_number) << " 에게 보내요" << endl;
        cout << sck_list[i].user_number << " 값 확인용 " << endl;
        test_count = std::to_string(sck_list[i].user_number);*/
        cout << test_count << " 값 확인용 " << endl;

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_result " << result << endl;
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
            //}
        }
    }
    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}


void dm_send_resultEdit(int server_request, const string& sender, int variable, const string& recipientUser) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser;
    for (int i = 0; i < client_count; i++) {

        /*cout << std::to_string(sck_list[i].user_number) << " 에게 보내요" << endl;
        cout << sck_list[i].user_number << " 값 확인용 " << endl;
        test_count = std::to_string(sck_list[i].user_number);*/
        cout << test_count << " 값 확인용 " << endl;

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_resultEdit " << result << endl;
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
            //}
        }
    }
    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}

void dm_send_findResult(int server_request, const string& sender, int variable, const string& recipientUser, string findValue) {
    string vari = std::to_string(variable);
    string serv_request = std::to_string(server_request);
    string result = serv_request + " " + sender + " " + vari + " " + recipientUser + " " + findValue;
    for (int i = 0; i < client_count; i++) {

        /*cout << std::to_string(sck_list[i].user_number) << " 에게 보내요" << endl;
        cout << sck_list[i].user_number << " 값 확인용 " << endl;
        test_count = std::to_string(sck_list[i].user_number);*/
        cout << test_count << " 값 확인용 " << endl;

        if (std::to_string(sck_list[i].user_number) == recipientUser) {
            //if (sck_list[i].login_status == true) {
            cout << "dm_send_findResult " << result << endl;
            send(sck_list[i].sck, result.c_str(), result.length(), 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
            //}
        }
    }

    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}

void dm_send_msg(const string& sender, const char* msg, const string& recipientUser) {
    for (int i = 0; i < client_count; i++) {
        if (sck_list[i].user == recipientUser) {
            //if (sck_list[i].login_status == true){
            cout << "dm_send_msg " << msg << endl;
            send(sck_list[i].sck, msg, MAX_SIZE, 0);
            return; // 특정 사용자에게 메시지를 보내면 함수 종료
            //}
        }
    }

    // 사용자를 찾지 못한 경우, 에러 메시지 출력 또는 다른 처리를 추가할 수 있습니다.
}

void send_msg_2(const string& msg) {
    for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
        cout << sck_list[i].sck << " 에게 보내여여어어어어" << endl;
        send(sck_list[i].sck, msg.c_str(), msg.length(), 0);
    }
    return;
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
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
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // 오류가 발생하지 않으면 recv는 수신된 바이트 수를 반환. 0보다 크다는 것은 메시지가 왔다는 것.
            cout << "========================" << endl;
            //mtx.lock();
            msg = buf; //sck_list[idx].user
            cout << sck_list[idx].user_number << " 랑 buf 는 = " << msg << endl;
            //send_msg(msg.c_str());
            std::istringstream iss(buf);
            tokens.clear(); // 이전 토큰을 지우고 새로 시작안하면 값 변질되서 제대로 인식 못함 ㅠㅠㅠㅠㅠ
            test_count = std::to_string(sck_list[idx].user_number);
            while (iss >> token) {
                tokens.push_back(token);
            }
            // 토큰0 기준으로 1:로그인 / 2:id찾기 / 3:pw찾기 / 4:회원가입 / 5:대화 / 6:기존채팅 / 7:친추 / 8:비번수정
            // tokens[0] == 1 이면 로그인 요청
            if (tokens[0] == "1") {
                cout << tokens[1] << " 토큰[1]을 아이디값으로 바탕으로 로그인 요청이 들어왔습니다." << endl;
                db_init();
                db_login();
            };
            // tokens[0] == 2 이면 아이디 찾기 요청
            if (tokens[0] == "2") {
                cout << tokens[1] << " 회원이 아이디 찾기 기능을 요청했습니다." << endl;
                db_init();
                db_findID();
            };
            // tokens[0] == 3 이면 비밀번호 찾기 요청
            if (tokens[0] == "3") {
                cout << tokens[1] << " 회원이 비밀번호 찾기 기능을 요청했습니다." << endl;
                db_init();
                db_findPW();
            };
            // tokens[0] == 4 이면 회원가입 요청
            if (tokens[0] == "4") {
                cout << " 회원가입 요청이 들어왔습니다." << endl;
                db_init();
                db_join();
            };
            if (tokens[0] == "41") {
                cout << " 아이디 확인 요청이 들어왔습니다." << endl;
                db_init();
                db_join_check();                
            };

            // tokens[0] == 5 이면 대화기능 요청
            if (tokens[0] == "5") {
                Sleep(300);
                cout << tokens[1] << " 회원이 친구 목록을 요청했습니다." << endl;
                db_init();
                db_selectQuery_ver2();
            };
            // tokens[0] == 51 이면 보낸 메시지 저장 요청
            if (tokens[0] == "51") {
                Sleep(300);
                cout << tokens[1] << " 회원이 메세지 저장을 요청했습니다." << endl;
                db_messageSend();
                //Sleep(2000);
                //db_selectQuery_ver2();
            };

            // tokens[0] == 6 이면 기존 채팅방 요청
            if (tokens[0] == "6") {
                cout << tokens[1] << " 회원이 유저의 기본 채팅방을 요청했습니다." << endl;
                db_init();
                db_chat_list();
            };

            if (tokens[0] == "61") {
                cout << tokens[1] << " 회원이 유저의 채팅방을 요청했습니다." << endl;
                db_init();
                db_join_check_ver2();
                if (!user_check) {
                    db_chat_room();
                }
            };

            // tokens[0] == 7 이면 친구 추가 기능 요청
            if (tokens[0] == "7") {
                cout << tokens[1] << " 회원이 친구 추가 기능을 요청했습니다." << endl;
                db_init();
                db_friend_register();
            };

            if (tokens[0] == "71") {
                cout << tokens[1] << " 회원이 친구 목록 확인 기능을 요청했습니다." << endl;
                db_init();
                db_friend_list();
            };
            // tokens[0] == 8 이면 비밀번호 수정 요청
            if (tokens[0] == "8") {
                if (tokens[3] == "N") { //tokens[0] 이 8 이면서 tokens[3] 의 값은 Y와 N으로 비밀번호 확인 결과를 결정합니다.
                    cout << tokens[1] << " [비밀번호 확인 요청] 토큰[1]을 비밀번호값으로 바탕으로 비밀번호 변경 요청이 들어왔습니다." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit(); // 데이터베이스 쿼리 실행
                    tokens.clear(); // 이전 토큰을 지우고 새로 시작안하면 값 변질되서 제대로 인식 못함 ㅠㅠㅠㅠㅠ
                }
                if (tokens[3] == "Y") {
                    cout << tokens[1] << " [비밀번호 확인 완료] 토큰[1]을 비밀번호값으로 바탕으로 비밀번호 변경 요청이 들어왔습니다." << endl;
                    test_count = std::to_string(sck_list[idx].user_number);
                    int result = 0;
                    db_init();
                    db_UserEdit_update();
                    tokens.clear(); // 이전 토큰을 지우고 새로 시작안하면 값 변질되서 제대로 인식 못함 ㅠㅠㅠㅠㅠ
                }
            }
            //mtx.unlock();
        }
        else { //그렇지 않을 경우 퇴장에 대한 신호로 생각하여 퇴장 메시지 전송
            msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx); // 클라이언트 삭제
            return;
        }
    }
}

void del_client(int idx) {
    closesocket(sck_list[idx].sck);
    //sck_list.erase(sck_list.begin() + idx); // 배열에서 클라이언트를 삭제하게 될 경우 index가 달라지면서 런타임 오류 발생....ㅎ
    client_count--;
}
