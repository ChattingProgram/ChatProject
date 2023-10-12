#pragma comment(lib, "ws2_32.lib") //명시적인 라이브러리의 링크. 윈속 라이브러리 참조
#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <mysql/jdbc.h> // C++하고 MYSQL을 연결하기 위해 선언.

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
sql::PreparedStatement* pstmt;
sql::ResultSet* res; //결과값을 위해
sql::ResultSet* res2; //결과값을 위해

struct SOCKET_INFO { // 연결된 소켓 정보에 대한 틀 생성
    SOCKET sck;
    string user;
};

std::vector<SOCKET_INFO> sck_list; // 연결된 클라이언트 소켓들을 저장할 배열 선언.
SOCKET_INFO server_sock; // 서버 소켓에 대한 정보를 저장할 변수 선언.
int client_count = 0; // 현재 접속해 있는 클라이언트를 count 할 변수 선언.
void server_init(); // socket 초기화 함수. socket(), bind(), listen() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void add_client(); // 소켓에 연결을 시도하는 client를 추가(accept)하는 함수. client accept() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void send_msg(const char* msg); // send() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void recv_msg(int idx); // recv() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.

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
void db_UserEdit(); // 회원 정보 수정부분
void db_selectQuery(); //db 셀랙트문
void db_login();
void db_countuser(); // (1) 유저 수 몇 명인지? (서버 공지로 활용)
void db_userlist(); // (3) 유저 목록 출력
void db_findID(); // (7) 유저 정보 찾기
void db_callMessage(); // 기존 채팅방 불러오기


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
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room");
    int number = 0;
    while (res->next()) {
        number += 1;
    }
    cout << " num " << number << endl; //디비 번호 확인.

    pstmt = con->prepareStatement("INSERT INTO message_room (number, user_id, content, time, chatroom_num) values(?,?,?,?,?)"); // INSERT

    pstmt->setInt(1, number + 1); //메시지 번호
    pstmt->setString(2, "abcd"); // 보낸 사람 아이디
    pstmt->setString(3, "hihihihi"); // 보낸 사람 아이디
    pstmt->setString(4, "2023-10-11"); // 날짜
    pstmt->setInt(5, 1); //챗 룸 넘버 
    pstmt->execute(); // 이거 있어야지 디비에 저장됨.

    cout << "메세지가 저장되었습니다." << endl;
    delete stmt;
    // MySQL Connector/C++ 정리
}

void db_selectQuery() {
    db_init();

    // SQL 쿼리 실행
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room"); // from 뒤에는 실제로 mysql 에서 사용하는 테이블의 이름을 써야한다.
    delete stmt;

    cout << "\n";
    cout << "SQL 쿼리 실행 (특정 PK 값에 해당하는 행 선택) \n";
    // SQL 쿼리 실행 (특정 PK 값에 해당하는 행 선택)
    string pkValue = "abcd"; // 실제 PK 값으로 대체
    //string query = "SELECT * FROM inventory WHERE id = '" + pkValue + "'";
    // 테이블 이름, PK 열 이름
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM message_room");//WHERE user_id = '" + pkValue + "'
    // 결과 출력
    while (res->next()) {
        cout << "Column1: " << res->getString("number") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("user_id") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("content") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("time") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("chatroom_num") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
    }

    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;
}

void db_login() {
    db_init();

    string user_id, pw;

    cout << "ID를 입력하세요. : ";
    cin >> user_id;
    cout << "비밀번호를 입력하세요. : ";
    cin >> pw;

    pstmt = con->prepareStatement("SELECT user_id, pw FROM users WHERE user_id = ?");
    pstmt->setString(1, user_id);
    res = pstmt->executeQuery();

    if (res->next()) {
        string db_id = res->getString(1);
        string db_pw = res->getString(2);

        if (db_id == user_id && db_pw == pw) {
            cout << " ▶ 로그인 성공! " << endl;
        }
        else {
            cout << " ▶ 로그인 실패!  " << endl;
        }
    }
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
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    pstmt = con->prepareStatement("INSERT INTO users (user_id, name, pw, nickname, friend_name) values(?,?,?,?,?)"); // INSERT

    string User_input;
    cout << "아이디를 입력하세요. : ";
    cin >> User_input;
    pstmt->setString(1, User_input); //아이디

    cout << "이름을 입력하세요. : ";
    cin >> User_input;
    pstmt->setString(2, User_input); // 이름

    cout << "비밀번호을 입력하세요. : ";
    cin >> User_input;
    pstmt->setString(3, User_input); // 비밀번호

    cout << "닉네임을 입력하세요. : ";
    cin >> User_input;
    pstmt->setString(4, User_input); // 날짜

    pstmt->setString(5, " "); //친구목록

    pstmt->execute(); // 이거 있어야지 디비에 저장됨.

    cout << "Finished inserting table" << endl;


}


// 회원 정보 수정부분
void db_UserEdit() {
    db_init();
    // 데이터베이스 쿼리 실행

    // 데이터베이스에서 현재 비밀번호를 가져오는 쿼리
    string selectQuery = "SELECT pw FROM users WHERE user_id = ?";
    pstmt = con->prepareStatement(selectQuery);
    pstmt->setString(1, "abcd");
    res = pstmt->executeQuery();

    if (res->next()) {
        string database_password = res->getString("pw");

        string user_input_password;
        cout << "비밀번호을 입력하세요. : ";
        cin >> user_input_password;

        // 사용자가 입력한 비밀번호와 데이터베이스의 비밀번호 비교
        if (user_input_password == database_password) {
            // 입력한 비밀번호와 데이터베이스 비밀번호가 일치하면 업데이트 수행
            cout << "확인 되었습니다." << endl;
            cout << "비밀번호을 입력하세요. : " << endl;
            cin >> user_input_password;

            string updateQuery = "UPDATE users SET pw = ? WHERE user_id = ?";
            pstmt = con->prepareStatement(updateQuery);
            pstmt->setString(1, user_input_password);
            pstmt->setString(2, "abcd");
            pstmt->executeUpdate();
            cout << "비밀번호가 업데이트되었습니다." << endl;
        }
        else {
            cout << "입력한 비밀번호가 일치하지 않습니다." << endl;
        }
    }
    else {
        cout << "사용자를 찾을 수 없습니다." << endl;
    }

    cout << "Finished update table" << endl;

}

void db_selectQuery_ver2() {
    db_init();
    // 데이터베이스 쿼리 실행
    stmt = con->createStatement();
    res = stmt->executeQuery("SELECT * FROM inventory"); // from 뒤에는 실제로 mysql 에서 사용하는 테이블의 이름을 써야한다.

    delete stmt;

    // 결과 출력
    while (res->next()) {
        cout << "Column1: " << res->getString("name") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
        cout << "Column2: " << res->getString("quantity") << endl; // ("필드이름")을 써야함. 필드이름 원하는거!
    }

    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;
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

void db_findID() {
    db_init();
    string name, phonenumber;

    cout << "ID를 찾을 이름을 입력하세요. : ";
    cin >> name;
    
    while (true) {
        cout << "전화번호를 입력하세요. : ";
        cin >> phonenumber;
        if (phonenumber.length() != 13) {
            cout << "▶ 전화번호를 다시 입력해주세요. (- 포함)" << endl;
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
            cout << name << "님의 아이디는 " << db_id << " 입니다." << endl;
        }
        else {
            cout << "입력하신 정보를 찾을 수 없습니다." << endl;
        }
    }
    else {
        cout << "입력하신 정보를 찾을 수 없습니다." << endl;
    }
}

void db_findPW() {
    db_init();
    string user_id, name, phonenumber;

    cout << "ID를 입력하세요. : ";
    cin >> user_id;
    cout << "ID를 찾을 이름을 입력하세요. : ";
    cin >> name;

    while (true) {
        cout << "전화번호를 입력하세요. : ";
        cin >> phonenumber;
        if (phonenumber.length() != 13) {
            cout << "▶ 전화번호를 다시 입력해주세요. (- 포함)" << endl;
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
            cout << user_id << "님의 비밀번호는 " << db_pw << " 입니다." << endl;
        }
        else {
            cout << "입력하신 정보를 찾을 수 없습니다." << endl;
            cout << "dddd" << endl;
        }
    }
    else {
        cout << "입력하신 정보를 찾을 수 없습니다." << endl;
        cout << "djdk" << endl;
    }
}

//
//    // 데이터베이스 쿼리 실행
//    // 데이터베이스에서 현재 비밀번호를 가져오는 쿼리
//    string selectQuery = "SELECT pw FROM users WHERE user_id = ?";
//    pstmt = con->prepareStatement(selectQuery);
//    pstmt->setString(1, "kms");
//    res = pstmt->executeQuery();
//    if (res->next()) {
//        string database_password = res->getString("pw");
//        string user_input_password;
//        cout << "비밀번호을 입력하세요. : ";
//        cin >> user_input_password;
//        // 사용자가 입력한 비밀번호와 데이터베이스의 비밀번호 비교
//        if (user_input_password == database_password) {
//            // 입력한 비밀번호와 데이터베이스 비밀번호가 일치하면 업데이트 수행
//            cout << "확인 되었습니다." << endl;
//            cout << "비밀번호을 입력하세요. : " << endl;
//            cin >> user_input_password;
//            string updateQuery = "UPDATE users SET pw = ? WHERE user_id = ?";
//            pstmt = con->prepareStatement(updateQuery);
//            pstmt->setString(1, user_input_password);
//            pstmt->setString(2, "kms");
//            pstmt->executeUpdate();
//            cout << "비밀번호가 업데이트되었습니다." << endl;
//        }
//        else {
//            cout << "입력한 비밀번호가 일치하지 않습니다." << endl;
//        }
//    }
//    else {
//        cout << "사용자를 찾을 수 없습니다." << endl;
//    }
//    cout << "Finished update table" << endl;
//}

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

int main() {
    WSADATA wsa;
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
        //std::thread th1(add_client); // 이렇게 하면 하나의 client만 받아짐...
        while (1) { // 무한 반복문을 사용하여 서버가 계속해서 채팅 보낼 수 있는 상태를 만들어 줌. 반복문을 사용하지 않으면 한 번만 보낼 수 있음.
            string text, msg = "";
            std::getline(cin, text);
            const char* buf = text.c_str();
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
    new_client.user = string(buf);
    cout << "buf" << buf << endl;

    string msg = "[공지] " + new_client.user + " 님이 입장했습니다.";
    cout << msg << endl;
    sck_list.push_back(new_client); // client 정보를 답는 sck_list 배열에 새로운 client 추가
    std::thread th(recv_msg, client_count);
    // 다른 사람들로부터 오는 메시지를 계속해서 받을 수 있는 상태로 만들어 두기.
    client_count++; // client 수 증가.
    //cout << "[공지] 현재 접속자 수 : " << client_count << "명" << endl;
    send_msg(msg.c_str()); // c_str : string 타입을 const chqr* 타입으로 바꿔줌.
    th.join();
}
void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}
void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";
    //cout << sck_list[idx].user << endl;
    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // 오류가 발생하지 않으면 recv는 수신된 바이트 수를 반환. 0보다 크다는 것은 메시지가 왔다는 것.
            msg = sck_list[idx].user + " : " + buf;
            cout << msg << endl;
            send_msg(msg.c_str());
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
