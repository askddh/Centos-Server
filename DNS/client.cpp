#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // 1. 创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. 连接服务器
    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // 3. 输入文件名
    char filename[BUFFER_SIZE];
    cout << "请输入要请求的文件名：";
    cin >> filename;

    // 4. 发送文件名给服务器
    send(sock, filename, strlen(filename), 0);
    cout << "已请求文件：" << filename << endl;

    // 5. 创建文件保存数据
    ofstream outfile("recv_file", ios::binary);
    char buffer[BUFFER_SIZE] = {0};
    int valread;

    // 6. 接收数据并写入文件
    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        outfile.write(buffer, valread);
        memset(buffer, 0, BUFFER_SIZE);
    }

    cout << "文件接收完成，已保存为：recv_file" << endl;

    outfile.close();
    close(sock);
    return 0;
}