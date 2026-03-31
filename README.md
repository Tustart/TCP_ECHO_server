# TCP_ECHO_server
一个使用C++和Linux Socket API编写的简单的TCP回声服务器。该项目旨在演示网络编程的基础概念，包括创建套接字、绑定端口、监听连接以及数据收发。

## 技术栈
-**语言**：C++
-**平台**：Linux
-**API**：POSIX Socket API(sys/socket.h,netinet/in.h等）

##编译与运行
###1.编译
打开终端，进入项目目录，使用g++进行编译
'''bash
g++ main.cpp -o echo_server -std=c++11
###2.启动服务器
编译成功后，运行生成的可执行文件：
./echo_server
程序启动后，会监听本地的8080端口
###3.测试连接
telnet 127.0.0.1 8080
连接成功后，输入任意文本，服务器会将该文本原样返回给你


#核心知识点
1.Socket创建:socket(AF_INET,SOCK_STREAM,0)
2.地址绑定：bind()将套接字与IP和端口关联
3.监听连接：listen()使服务器进入监听状态
4.接受连接：accept()阻塞等待客户端连接
5.数据收发：使用read()和write()(或send()和recv()进行通信).
