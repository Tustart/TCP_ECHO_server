//必须的头文件，C++标准库+Linux系统API头文件
#include <iostream>
#include <cstring>   //用于memset字符串操作
#include <sys/socket.h> //socket核心API
#include <netinet/in.h> //网络地址结构体,字节序转换API
#include <arpa/inet.h> //用于IP地址的转换
#include <unistd.h> //用于close,read,write等系统调用
//定义服务器端口和缓冲区大小，用constexpr替代宏，符合C++11规范
constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;
int main()
{
	//1.创建socket文件描述符,AF_INET:使用IPv4协议；SOCK_STREAM:使用TCP流式协议；0:固定默认协议
	int listen_fd = socket(AF_INET,SOCK_STREAM,0);
	//错误处理:socket创建失败会返回-1,perror会打印系统错误信息，方便排查
	if(listen_fd == -1)
		{
			perror("socket create failed");
			return -1;
		}
	std::cout << "1.创建socket成功,fd: "<< listen_fd << std::endl;
	//解决端口占用问题
	int opt = 1;
	//设置端口复用,允许TIME_WAIT状态的端口被重新绑定
	if(setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)) == -1)
		{
			perror("setsockopt failed");
			close(listen_fd);
			return -1;
		}
	//2.绑定IP和端口
	//定义服务器地址结构体，初始化全0
	struct sockaddr_in server_addr;
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family = AF_INET; //使用IPv4
	server_addr.sin_addr.s_addr =  INADDR_ANY; //监听本机所有网卡的IP(0.0.0.0)
	//htons:主机字节序转网络字节序,必须加!否则端口绑定错误
	server_addr.sin_port = htons(PORT);
	//执行bind绑定
	if(bind(listen_fd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
		{
			perror("bind failed");
			close(listen_fd); //绑定失败，必须关闭已创建的fd，避免资源泄漏
			return -1;
		}
	std::cout << "2.端口绑定成功，端口: "<< PORT <<std::endl;
	//3.开始监听端口
	//第二个参数backlog: 全连接队列的最大长度，这里设为10
	if(listen(listen_fd,10) == -1)
		{
			perror("listen failed");
			close(listen_fd);
			return -1;
		}
	std::cout<< "3.服务器开始监听，等待客户端连接..." <<std::endl;
	//4.循环接收客户连接
	while(true)
		{
			//定义客户端地址结构体，用于存储连接进来的客户端IP和端口
			struct sockaddr_in client_addr;
			socklen_t client_addr_len = sizeof(client_addr);
			//accept阻塞等待客户端连接，连接成功返回新的fd，专门用于和该客户端通信
			int client_fd = accept(listen_fd,(struct sockaddr*)&client_addr,&client_addr_len);
			if(client_fd == -1)
				{
					perror("accept failed");
				 	continue; //单个客户端连接失败不影响服务器整体运行，继续等待下一个
				}
			//inet_ntoa:把网络字节序的IP转成字符串,ntohs: 网络字节序转主机字节序的端口
			std::cout<< "4.新客户端连接成功，客户端IP: "<<inet_ntoa(client_addr.sin_addr)<<",端口:"<<ntohs(client_addr.sin_port)<<",客户端fd: "<<client_fd<<std::endl;
			//5.收发数据:Echo逻辑
			char buffer[BUFFER_SIZE] = {0};//定义缓冲区，存储客户端发来的数据
			while(true)
				{
					//清空缓冲区，避免上次的数据残留
					memset(buffer,0,BUFFER_SIZE);
					//recv阻塞接收客户端数据，返回值为实际接收到的字节数
					ssize_t recv_len = recv(client_fd,buffer,BUFFER_SIZE-1,0);
					//处理接受结果
					if(recv_len == -1)
						{
							perror("recv failed");
							break;
						}
					else if(recv_len == 0)
						{
							//recv返回0,带表客户端主动关闭了连接
							std::cout <<"客户端断开连接,fd "<<client_fd <<std::endl;
							break;
						}
					//打印客户端发来的信息
					std::cout<<"收到客户端消息: "<<buffer<<std::endl;
					//Echo核心:把收到的数据原封不动发回给客户端
					ssize_t send_len = send(client_fd,buffer,strlen(buffer),0);
					if(send_len == -1)
						{
							perror("send failed");
							break;
						}
					std::cout<<"成功返回消息给客户端,长度: "<<send_len<<"字节"<<std::endl;
				}
				//客户端端开后，必须关闭对应的fd,避免资源泄露
				close(client_fd);
				std::cout<<"已关闭客户端fd: "<<client_fd <<std::endl;
		}
	//服务器退出时，关闭监听fd
	close(listen_fd);
	return 0;
}

