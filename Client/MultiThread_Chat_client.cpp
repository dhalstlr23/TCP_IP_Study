// MultiThread_Chat_client.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma comment(lib, "ws2_32")
#include <iostream>
#include <string>
#include <format>
#include <thread>
#include <WS2tcpip.h>
#include <WinSock2.h>
static void received_message(SOCKET);
static void sending_message(SOCKET);
static std::string nick_name;
static char buffer[BUFSIZ]{ '\0' };
int main(int argc, const char* argv[])
{
    std::cout << "TCP-IP 멀티스레드 채팅 클라이언트!\n";
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata)) {
        std::cerr << "WSAStartup() error" << std::endl;
        return EXIT_FAILURE;
    }
    
    if (argc != 4)
    {
        std::cerr << "사용방법 :  " << "IP NUMBER " << "PORT " << "NICKNAME" << std::endl;
        return EXIT_FAILURE;
    }

    nick_name = static_cast<std::string>(argv[3]);
    SOCKET server_socket{ 0ull };
    SOCKADDR_IN server_addr;
    memset(&server_addr, 0, sizeof server_addr);

    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        std::cout << "socket() error" << std::endl;
        return EXIT_FAILURE;
    }
    server_addr.sin_family = AF_INET;   //IPv4
    if ((inet_pton(AF_INET, argv[1], &server_addr.sin_addr)) == 0)
    {
        std::cout << "inet_pton() error" << std::endl;
        return EXIT_FAILURE;
    }
    server_addr.sin_port = htons(std::stoi(argv[2]));   // "9999" -> 9999
    if (connect(server_socket, (const SOCKADDR*)&server_addr, sizeof server_addr) == SOCKET_ERROR)
    {
        std::cout << "connect() error" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Server is connected ... " << std::endl;
    std::jthread thread1(received_message, server_socket);
    std::jthread thread2(sending_message, server_socket);
    thread1.join(); // thread1 끝날때 까지 기다려라.
    thread2.join(); // thread2 끝날때 까지 기다려라.

    // closesocket(/*server_socket*/);
    WSACleanup();
}

void received_message(SOCKET _server_socket)
{
    char buffer[BUFSIZ]{ '\0' };
    while (true)
    {
        int message_length = recv(_server_socket, buffer, sizeof buffer, 0);
        std::cout << "Received message :  " << buffer << std::endl;
        if (message_length <= 0)
        {
            std::cout << "종료" << std::endl;
            memset(&buffer, '\0', BUFSIZ);
            closesocket(_server_socket);
            break;
        }
        memset(&buffer, '\0', BUFSIZ);
    }
}

void sending_message(SOCKET _server_socket)
{
    std::string sending_message;
    while (true)
    {
        std::getline(std::cin, sending_message);
        if (!std::strcmp(sending_message.data(), "QUIT"))
        {
            closesocket(_server_socket);
            break;
        }
        std::string nick_name_sending_message = std::string(nick_name) + " " + sending_message;
        send(_server_socket, nick_name_sending_message.data(), nick_name_sending_message.size(), 0);
        sending_message = "";
    }
}