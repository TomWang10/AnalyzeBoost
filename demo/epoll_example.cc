#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstdio>
#include <cstring>
#include <string>

const int gMaxEvents = 10;

int main(int argc, char ** argv)
{
    if (2 >= argc)
    {
        printf("input the ip, port\n");
        return -1;
    }
    int listenfd = 0;
    // new socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd)
    {
        printf("init listenfd failed\n");
        return -1;
    }
    struct sockaddr_in serverAddr;
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &serverAddr.sin_addr);
    serverAddr.sin_port = htons(std::stoi(argv[2]));
    // bind
    if (-1 == bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)))
    {
        printf("bind listenfd failed\n");
        return -1;
    }
    // listen
    listen(listenfd, 5);
    // new epoll
    int epollfd = epoll_create1(0);
    if (-1 == epollfd)
    {
        printf("epoll_create failed\n");
        return -1;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    
    // add to epoll ctl
    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev))
    {
        printf("epoll_ctl failed\n");
        return -1;
    }
    struct epoll_event events[gMaxEvents];
    // epoll_wait
    for (; ;)
    {
        int nfds = epoll_wait(epollfd, events, gMaxEvents, -1);
        if (-1 == nfds)
        {
            printf("epoll_wiat failed\n");
            return -1;
        }

        for (int n = 0; n < nfds; ++n)
        {
            // new client
            if (events[n].data.fd == listenfd)
            {
                struct sockaddr_in clientAddr;
                int clientAddrLen = sizeof(clientAddr);
                // accept
                printf("new client coming\n");
                int newClientfd = accept4(listenfd, (struct sockaddr*)&clientAddr, (socklen_t *)&clientAddrLen, SOCK_NONBLOCK);
                if (-1 == newClientfd)
                {
                    printf("accept failed\n");
                    return -1;
                }
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = newClientfd;
                if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, newClientfd, &ev))
                {
                    printf("epoll_ctl failed to new client\n");
                    return -1;
                }
            }
        }
    }
    return 0;
}
