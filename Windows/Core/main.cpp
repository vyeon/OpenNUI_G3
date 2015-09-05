/******************************************************************* 
FileName: main.cpp
Author  : vee
Date    : 2015-08-01 22:00:54
Desc    : Entry point of nui framework core
*******************************************************************/ 

#include <iostream>
#include <kernel/kernel.h>
#include <vee/voost/timer.h>
#include <conio.h>

void print_line()
{
    for (int i = 0; i < 80; ++i)
        printf("-");
}

void __cdecl f1(void*)
{
    puts(__FUNCSIG__);
}

void __stdcall f2(void*)
{
    puts(__FUNCSIG__);
}

void timer_callback(unsigned int timer_tick)
{
    printf("tick: %d\n", timer_tick);
}

int main()
{
    //TODO: Scheduler test
    printf("Scheduler test\n");
    xkernel::scheduler::initialize();
    xkernel::scheduler::request(f1, nullptr);
    xkernel::scheduler::request(f2, nullptr);
    getch();
    print_line();
    xkernel::scheduler::dispose();
    {
        printf("Timer test\npress any key to stop a timer...\n");
        auto timer = vee::voost::timer::create_timer();
        timer->run(500, vee::make_delegate<void(unsigned int)>(timer_callback));
        getch();
        print_line();
    }
    return 0;
}