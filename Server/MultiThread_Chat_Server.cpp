// MultiThread_Chat_Server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <mutex>
#include <format>
#include <WS2tcpip.h>
#include <WinSock2.h>

static std::mutex mtx;  // mutex 객체를 생성 : mtx
static auto received_data(SOCKET) -> void;  // 함수의 원형 (thread 가 될 함수)
const int LISTEN_CLIENT{ 10U }; // 10명 동시에 채팅
static std::vector<SOCKET> client_sockets;  // 클라이언트의 소켓을 저장하는 vector
static std::vector<std::jthread> jthreads;
// static int index{ 0 };
static int chatter{ 0 };    // 채팅하는 사람 체크
int main(int argc, const char * argv[])
{
    std::cout << "TCP-IP 멀티스레드 채팅 서버" << std::endl;
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata))
    {
        std::cerr << "SOCKET ERROR" << std::endl;
        return EXIT_FAILURE;
    }

    SOCKET server_sock{ 0ull };
    SOCKET client_sock{ 0ull };

    SOCKADDR_IN server_addr;
    SOCKADDR_IN client_addr;

    memset(&server_addr, 0, sizeof server_addr);
    memset(&client_addr, 0, sizeof client_addr);

    size_t client_addr_size{ 0ull };

    /*
    * 서버창에 접속한 클라이언트들의 IP를 확인하는 코드
    */
    char client_ip[INET_ADDRSTRLEN];    // ip를 저장하는 char 배열

    if (argc != 2)
    {
        std::cerr << "사용방법 :  " << argv[0] << " 9999" << std::endl;
        return EXIT_FAILURE;
    }
    /*
    * Server LifeCycle 만들기
    */
    server_sock = socket(PF_INET, SOCK_STREAM, 0); // IPv4, TCP, 0
    if (server_sock == INVALID_SOCKET)
    {
        std::cout << "socket() error" << std::endl;
        return EXIT_FAILURE;
    }
    server_addr.sin_family = AF_INET;   // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 127.0.0.1
    server_addr.sin_port = htons(std::stoi(argv[1]));   // "9999" -> 9999
    if (bind(server_sock, (const struct sockaddr*)&server_addr, sizeof server_addr) == SOCKET_ERROR)
    {
        std::cout << "bind() error" << std::endl;
        return EXIT_FAILURE;
    }
    if (listen(server_sock, LISTEN_CLIENT) == SOCKET_ERROR)
    {
        std::cout << "listen() error" << std::endl;
        return EXIT_FAILURE;
    }
    while (true)
    {
        if (chatter > LISTEN_CLIENT) continue;  // 10명 까지만 받음.
        client_addr_size = sizeof client_addr;
        client_sock = accept(server_sock, (SOCKADDR*)&client_addr, (int*)&client_addr_size);
        if (client_sock == INVALID_SOCKET)
        {
            std::cout << "accept() error" << std::endl;
            return EXIT_FAILURE;
        }
        // 클라이언트 IP 출력
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        std::cout << "클라이언트 연결 :  " << client_ip << std::endl;

        // 클라이언트 소켓을 벡터에 넣기
        client_sockets.emplace_back(client_sock);
        // thread 연결
        jthreads.emplace_back(received_data, client_sock);  // id -> client_sock

        /*
        * 채팅을 종료하고 나가는 사람의 thread를 종료.
        */
        for (auto iter = jthreads.cbegin(); iter != jthreads.cend();)
        {
            if (!iter->joinable()) // joinable 상태가 아니라면, 종료
            {
                iter = jthreads.erase(iter);    // 삭제 시켜야함
            }
            else ++iter;
        }
        ++chatter;

    }






    closesocket(client_sock); // Server는 close() 하지 않겠다.
    WSACleanup();
    return 0;
}

void received_data(SOCKET client_socket)
{
    int str_length{ 0 };
    char buffer[BUFSIZ]{ '\0' };
    while ((str_length = recv(client_socket, buffer, sizeof buffer, 0)) > 0)
    {
        for (SOCKET s : client_sockets) 
        {
            std::lock_guard<std::mutex> lock(mtx);
            send(s, buffer, sizeof buffer, 0);
        }
        memset(&buffer, '\0', BUFSIZ);
    }
    /*
    * 클라이언트 나갔을때 클라이언트를 벡터에서 삭제
    */
    for (auto it = client_sockets.cbegin(); it != client_sockets.cend(); ++it)
    {
        if (*it == client_socket)
        {
            client_sockets.erase(it);
            --chatter; // 사람을 삭제
            break;
        }
    }
}