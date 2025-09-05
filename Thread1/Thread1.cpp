// Thread1.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
static unsigned WINAPI thread_func(int); // 함수의 원형
int main()
{
    std::cout << "Thread 생성하기" << std::endl;
    std::jthread thread1(thread_func, 10);  // jthread 생성자
    std::jthread thread2(thread_func, 20);  // jthread 생성자
    std::jthread thread3(thread_func, 30);  // jthread 생성자
}

unsigned WINAPI thread_func(int id)
{
    for (int i = 0; i < id; ++i)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));   // 1초 지연
        std::cout << "Running thread : " << id << std::endl;
    }
    return 0u;
}