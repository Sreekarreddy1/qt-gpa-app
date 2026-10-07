#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    int srv = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(srv, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5000);
    bind(srv, (sockaddr*)&addr, sizeof(addr));
    listen(srv, 5);
    std::cout << "Server listening on 5000\n" << std::flush;

    while (true) {
        int c = accept(srv, nullptr, nullptr);
        char buf[1024] = {0};
        int n = read(c, buf, sizeof(buf) - 1);
        if (n > 0) {
            std::istringstream in(buf);
            double x, sum = 0; int count = 0;
            while (in >> x) { sum += x; count++; }
            std::string reply = count ? "Average GPA: " + std::to_string(sum / count) + "\n"
                                      : "No valid numbers\n";
            write(c, reply.c_str(), reply.size());
            std::cout << "Handled request: " << buf << std::flush;
        }
        close(c);
    }
}