// Hello_server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#pragma comment(lib, "ws2_32") // 반드시 맨위에 코딩해야함.
#include <WinSock2.h>
#include <iostream>
//#include <cstdio>
#include <string>
#include <array>

int main(int argc, const char* argv[])
{
    std::cout << "윈도우 기반 TCP-IP \n" << std::endl;
    WSADATA wsadata; // wsadata 객체 생성
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
    {
        std::cerr << "socket error" << std::endl;
        return EXIT_FAILURE;
    }
    if (argc != 2) {
        std::cerr << "사용방법은 " << argv[0] << " 9999" << std::endl;
        return EXIT_FAILURE;
    }
    SOCKET server_sock{ 0ull }; // uniform initializer
    SOCKET client_sock{ 0ull };
    SOCKADDR_IN server_addr; // server 주소랑 IP정보등을 담는 구조체
    SOCKADDR_IN client_addr; // client 주소랑 IP정보등을 담는 구조체
    memset(&server_addr, 0, sizeof server_addr);
    memset(&client_addr, 0, sizeof client_addr);
    std::array<char, BUFSIZ> buffer{ "Welcome My server!!" };

    /*
    * 서버의 라이프 사이클(함수) 만들기
    */
    server_sock = socket(PF_INET, SOCK_STREAM, 0); // TCP연결
    if (server_sock == INVALID_SOCKET)
    {
        std::cout << "socket() error" << std::endl;
        return EXIT_FAILURE;
    }
    server_addr.sin_family = AF_INET; // PF_INET : IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 내 컴퓨터의 IP : 10.10.200.70
    server_addr.sin_port = htons(std::stoi(argv[1]));  // 문자열 -> 정수값 PORT
    size_t client_size{ 0ull }; // 클라이언트의 구조체 사이즈 크기를 저장하는 변수
    if (bind(server_sock, reinterpret_cast<const struct sockaddr*>(&server_addr), sizeof server_addr) == SOCKET_ERROR)
    {
        std::cout << "bind() error" << std::endl;
        return EXIT_FAILURE;
    }
    if (listen(server_sock, 5) == SOCKET_ERROR)
    {
        std::cout << "listen() error" << std::endl;
        return EXIT_FAILURE;
    }
    client_size = sizeof client_addr; // 클라이언트 구조체의 사이즈
    client_sock = accept(server_sock, reinterpret_cast<struct sockaddr*>(&client_addr), reinterpret_cast<int*>(&client_size));
    if (client_sock == INVALID_SOCKET)


    {
        std::cout << "accept() error" << std::endl;
        return EXIT_FAILURE;
    }
    send(client_sock, buffer.data(), buffer.size(), 0);// 클라이언트에게 문자열을 보내기
    closesocket(client_sock);
    closesocket(server_sock);

    
    // 맨 마지막에 넣어 줘야 함
    WSACleanup(); // 원속 종료
    return 0;
}

//if (argc != 2) {
//    std::cerr << "사용방법은 " << argv[0] << " 9999" << std::endl;
//    return EXIT_FAILURE;
//}




