#include "Sockt.hpp"
#include <exception>
#include <netdb.h>
#include <stdexcept>
#include <string>

std::string getCurrentTime()
{
    time_t timer;
    time(&timer);
    return ctime(&timer);
}

int main()
{
    int ret;
    char buffer[1024];
    ft::Sockt client;
    ft::Sockt server;

    try
    {
        server.create();
        server.bind();
        server.listen();
        std::cout << server << std::endl;

        while (1)
        {
            std::cout << "listening ..." << std::endl;
            client = server.accept();

            {
                char host[NI_MAXHOST];
                char service[NI_MAXSERV];

                ret = getnameinfo(reinterpret_cast<sockaddr*>(&client.address), client.addressLen, host, NI_MAXHOST, service, NI_MAXSERV, 0);
                std::cout << "communication initiated with : " << host << " at service : " << service << std::endl;
            }

            try
            {
                ret = recv(client.fd, buffer, 1024, 0);
                if (ret < 0)
                    throw std::runtime_error("recv failure");
                std::string resp = "HTTP/1.1 200 OK\r\n"
                                   "Connection: close\r\n"
                                   "Content-Type: text/plain\r\n\r\n"
                                   "Local time is: " + getCurrentTime();
                std::cout << "msg : " << buffer << std::endl;
                ret = send(client.fd, resp.c_str(), resp.length(), 0);
                if (ret < 0)
                    throw std::runtime_error("send failure");
                client.destroy();
            }
            catch (std::exception& e)
            {
                client.destroy();
                throw;
            }
        }
        server.destroy();
    }
    catch (std::exception& e)
    {
        client.destroy();
        server.destroy();
        std::cerr << RED;
        std::cerr << e.what() << std::endl;
        std::cerr << BLACK;
        exit(EXIT_FAILURE);
    }
    return 0;
}