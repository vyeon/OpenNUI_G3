/******************************************************************* 
FileName: main.cpp
Author  : vee
Date    : 2015-08-01 22:00:54
Desc    : Entry point of nui framework core
*******************************************************************/ 

#include <iostream>
#include <array>
#include <kernel/kernel.h>
#include <vee/voost/timer.h>
#include <vee/voost/net.h>
#include <vee/voost/pipe.h>
#include <conio.h>
#pragma warning(disable:4996)

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

void named_pipe_echo_server();

int main()
{
    named_pipe_echo_server();
    printf("Press any key to exit...\n");
    _getch();
    return 0;
}

void named_pipe_echo_server()
{
    {
        printf("Named pipe echo server\n");
        using vee::byte;
        using vee::operation_result;
        using vee::voost::interprocess::creation_option;
        using vee::voost::interprocess::pipe_data_transfer_mode;
        using vee::voost::interprocess::named_pipe;
        using vee::voost::interprocess::named_pipe_server;
        using vee::voost::interprocess::pipe_time_out_constant;

        auto server = vee::voost::interprocess::win32::create_named_pipe_server();
        
        try
        {
            auto session = server->accept("\\\\.\\pipe\\opennuipipe2",
                                          pipe_data_transfer_mode::iomode_message, 1024, 1024);
            printf("Client connected.\n");
            std::array<byte, 512> buffer;
            // Welcome message
            char welcome_message[] = "Welcome";
            session->write((byte*)welcome_message, strlen(welcome_message));

            while (true)
            {
                unsigned int bytes_transferred = session->read(buffer.data(), buffer.size());
                printf("data recv: %dbytes: %s\n", bytes_transferred, buffer.data());
                //bytes_transferred = session->write((byte*)buffer.data(), bytes_transferred);
                //printf("echo send: %dbytes: %s\n", bytes_transferred, buffer.data());
                session->async_write(buffer.data(),
                                     bytes_transferred,
                                     [](operation_result& result, size_t bytes_transferred) -> void
                                     {
                                        printf("echo send: %dbytes\n", bytes_transferred);
                                    });
            }
        }
        catch (vee::exception& e)
        {
            printf("exception occured!\nerror_code: %d\nmessage: %s\n", e.code, e.desc.data());
        }
    }
}

void websocket_echo_server_main()
{
    {
        printf("Websocket echo server\n");
        using vee::voost::net::byte;
        using vee::voost::net::net_stream;
        using vee::voost::net::error_code;
        using vee::voost::net::websocket::websocket_stream;
        using vee::voost::net::websocket::opcode_id;
        using vee::system::operation_result;

        auto server = vee::voost::net::websocket::create_server(1992);
        auto session1 = std::static_pointer_cast<websocket_stream>(server->accept());
        std::array<byte, 512> buffer;
        try
        {
            // Welcome message
            char welcome_message[] = "Welcome";
            session1->write(opcode_id::text_frame, (byte*)welcome_message, strlen(welcome_message));

            while (true)
            {
                unsigned int bytes_transferred = session1->read(buffer.data(), buffer.size());
                printf("data recv: %dbytes: %s\n", bytes_transferred, buffer.data());
                //bytes_transferred = (session1->write(opcode_id::text_frame, buffer.data(), bytes_transferred)).payload_size;
                //printf("echo send: %dbytes: %s\n", bytes_transferred, buffer.data());
                session1->async_write(opcode_id::text_frame,
                                      buffer.data(),
                                      bytes_transferred,
                                      [](operation_result& result, size_t bytes_transferred) -> void
                {
                    printf("echo send: %dbytes\n", bytes_transferred);
                });
            }
        }
        catch (vee::exception& e)
        {
            printf("exception occured!: %s\n", e.what());
        }
    }
}

//void scheduler_test_code()
//{
//    printf("Scheduler test\n");
//    xkernel::scheduler::initialize();
//    xkernel::scheduler::request(f1, nullptr);
//    xkernel::scheduler::request(f2, nullptr);
//    getch();
//    print_line();
//    xkernel::scheduler::dispose();
//    {
//        printf("Timer test\npress any key to stop a timer...\n");
//        auto timer = vee::voost::timer::create_timer();
//        timer->run(500, vee::make_delegate<void(unsigned int)>(timer_callback));
//        getch();
//        print_line();
//    }
//}