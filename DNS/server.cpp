#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    // 1. 创建socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 2. 绑定地址
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    bind(server_fd, (sockaddr*)&address, sizeof(address));

    // 3. 监听
    listen(server_fd, 1);
    cout << "服务器等待客户端连接..." << endl;

    // 4. 接受连接
    int addrlen = sizeof(address);
    int new_socket = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen);
    cout << "客户端已连接" << endl;

    // 5. 接收客户端发来的文件名
    char filename[BUFFER_SIZE] = {0};
    read(new_socket, filename, BUFFER_SIZE);
    cout << "客户端请求文件：" << filename << endl;

    // 6. 打开文件
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "文件不存在！" << endl;
        const char* err = "NO FILE";
        send(new_socket, err, strlen(err), 0);
        close(new_socket);
        close(server_fd);
        return 0;
    }

    // 7. 发送文件内容
    char buffer[BUFFER_SIZE] = {0};
    while (file.read(buffer, BUFFER_SIZE)) {
        send(new_socket, buffer, file.gcount(), 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
    send(new_socket, buffer, file.gcount(), 0);

    cout << "文件发送完成！" << endl;

    file.close();
    close(new_socket);
    close(server_fd);
    return 0;
}
