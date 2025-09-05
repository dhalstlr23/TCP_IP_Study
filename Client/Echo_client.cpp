#pragma comment(lib, "ws2_32") // 반드시 맨위에 코딩해야함.
#include <WinSock2.h>
#include <iostream>
//#include <cstdio>
#include <string>
#include <array>
#include <WS2tcpip.h>

int main(int argc, const char* argv[])
{
    std::cout << "윈도우 기반 TCP-IP \n" << std::endl;
    WSADATA wsadata; // wsadata 객체 생성
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        std::cerr << "socket error" << std::endl;
        return EXIT_FAILURE;
    }
    if (argc != 3)
    {
        std::cerr << "사용방법 " << argv[0] << "IP_NUMBER " << "PORT" << std::endl;
        return EXIT_FAILURE;

    }

    SOCKET server_socket;
    SOCKADDR_IN server_addr;    // 서버의 구조를 담는 구조체
    server_socket = socket(PF_INET /*IPv4*/, SOCK_STREAM/*TCP*/, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET)
    {
        std::cout << "socket() error" << std::endl;
        return EXIT_FAILURE;
    }
    memset(&server_addr, 0, sizeof server_addr); // 초기화
    server_addr.sin_family = AF_INET; // IPv4 PF_INET
    //server_addr.sin_addr.s_addr = inet_addr(argv[1]); // "192.168.100.200"  Deprecated
    if (inet_pton(AF_INET, argv[1], &(server_addr.sin_addr)) == 0)
    {
        std::cout << "inet_pton() error" << std::endl;
        return EXIT_FAILURE;
    }
    server_addr.sin_port = htons(std::stoi(argv[2])); // "9999" -> 9999
    if (connect(server_socket, (const SOCKADDR*)&server_addr, sizeof server_addr) == SOCKET_ERROR)
    {
        std::cout << "connect() error" << std::endl;
        return EXIT_FAILURE;
    }
    char buffer[BUFSIZ]{ '\0' };
    int str_length = 0;
    while (true)
    {
        std::string message; // string
        std::cout << "전송할 문자를 입력해주세요  ";
        std::getline(std::cin, message);
        send(server_socket, (const char*)message.c_str(), message.size(), 0);
        if ((str_length = recv(server_socket, buffer, sizeof buffer, 0)) > 0)
        {
            std::cout << "서버에서 보낸 문자 :   " << buffer << std::endl;
        }
    }



    /*int read_length = recv(server_socket, reinterpret_cast<char*>(buffer), sizeof buffer, 0);
    if (read_length == -1)
    {
        std::cout << "recv() error" << std::endl;
    }*/
    //std::cout << buffer << std::endl;
    closesocket(server_socket);



    // 맨 마지막에 넣어 줘야 함
    WSACleanup(); // 원속 종료
    return 0;
}


