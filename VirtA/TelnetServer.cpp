//
//  TelnetServer.cpp
//  Implementation of TelnetServer
//  Sets up a telnet server listening on TCP port 2121
//
//  Created by Lionel Pinkhard on 2017/07/15.
//
//  MIT License
//
//  Copyright (c) 2017 Lionel Pinkhard
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//

#include "TelnetServer.h"

#include <thread>

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

/**
 * Sets up the telnet server
 */
TelnetServer::TelnetServer(std::shared_ptr<ASCIIKeyboard> input, std::shared_ptr<Terminal> output, const char *port)
    : input(input), output(output), port(port) {
    stopping = false;
    socketsMutex = output->getSocketsMutex();
}

/**
 * Processes telnet server sockets
 */
void TelnetServer::process() {
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage remote_addr;
    socklen_t sin_size;
    int sockfd = 0;
    int clientfd;
    int int1 = 1;
    char s[INET6_ADDRSTRLEN];
    char buf[100];
    ssize_t numbytes;
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        stopping = true;
        return;
    }
    
    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        
        fcntl(sockfd, F_SETFL, O_NONBLOCK);
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &int1,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            return;
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        
        break;
    }
    
    freeaddrinfo(servinfo);
    
    if (p == NULL)  {   // Failed to bind
        return;
    }
    
    if (listen(sockfd, 10) == -1) {
        perror("listen");
        return;
    }
    
    // Server ready
    
    while(!stopping) {
        sin_size = sizeof remote_addr;
        clientfd = accept(sockfd, (struct sockaddr *) &remote_addr, &sin_size);
        if (clientfd == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("accept");
            }
            
            // Iterate client sockets
            socketsMutex->lock();
            std::vector<int>::iterator it = sockets.begin();
            while (it != sockets.end()) {
                if ((numbytes = recv(*it, buf, 99, 0)) == -1) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {  // Close socket with error
                        perror("recv");
                        close(*it);
                        it = sockets.erase(it);
                        continue;
                    } else {
                        ++it;
                    }
                } else if (numbytes > 0) {
                    for (int i = 0; i < numbytes; i++) {
                        if (buf[i] == 0xa)  // Skip CR (LF becomes CR)
                            continue;
                        
                        // Handle input on the client socket
                        input->keypress(buf[i]);
                        socketsMutex->unlock(); // Let other things happen on the sockets
                        usleep(26667);          // Throttle to 300bps
                        socketsMutex->lock();
                    }
                }
            }
            socketsMutex->unlock();
            
            continue;
        }
        
        inet_ntop(remote_addr.ss_family,
                  &(((struct sockaddr_in*) &remote_addr)->sin_addr),
                  s, sizeof s);
        
        // Add client socket to the output device
        output->addSocket(clientfd);
        socketsMutex->lock();
        sockets.push_back(clientfd);
        socketsMutex->unlock();
    }
    
    // Remove client sockets
    std::vector<int>::iterator it = sockets.begin();
    while (it != sockets.end()) {
        close(*it);
        it = sockets.erase(it);
    }
    
    // Close server socket
    close(sockfd);
}

/**
 * Starts the telnet server thread
 */
void TelnetServer::start() {
    serverThread = std::thread(&TelnetServer::process, this);
}

/**
 * Stops the telnet server thread
 */
void TelnetServer::stop() {
    stopping = true;
    serverThread.join();
}
