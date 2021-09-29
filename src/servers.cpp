#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <iostream>
#include <string>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#define PORT 7000
#define QUEUE 20

struct NotDigitOperator {
    bool operator()(const char c) {
        return c != ' ' && !std::isdigit(c);
    }
};

struct ReturnData {
    std::string sunOfNums;
    std::string sortedArray;
};


class SocketServer {
public:
    virtual void workServ() = 0;
    ReturnData sumOfNumbersInString(std::string &str) {

        NotDigitOperator notADigit;
        std::string::iterator end = std::remove_if(str.begin(), str.end(), notADigit);
        std::string all_numbers(str.begin(), end);
        std::stringstream ss(all_numbers);
        std::vector<int> numbers;


        for(int i = 0; ss >> i; ) {
            numbers.push_back(i);
            std::cout << i << " ";
        }

        std::sort(numbers.begin(), numbers.end());

        if (numbers.size() == 0) {
            return ReturnData();
        }

        int numSums = 0;

        std::vector<int>::iterator iter = numbers.begin();  
        while(iter!=numbers.end()) 
        {
            numSums += *iter; 
            iter++;
        }

        std::ostringstream os;
        for (int i = 0; i < numbers.size(); i++) {
            if (i == numbers.size() - 1) {
                os << numbers[i];
            } else {
                os << numbers[i] << " ";
            }
        }

        std::string sNumSums = std::to_string(numSums);

        os << "\n" << sNumSums;
    
        std::string retStr(os.str());

        ReturnData returnData{sNumSums, retStr};
        return returnData;
    }
};

class TcpServer: public SocketServer {
public:
    int port;

    TcpServer(int _port): port(_port) {};
    TcpServer(): TcpServer(PORT) {};

    void workServ() {
        fd_set rfds;
        struct timeval tv;
        int retval, maxfd;
        int ss = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in server_sockaddr;
        server_sockaddr.sin_family = AF_INET;
        server_sockaddr.sin_port = htons(this->port);
        
        server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(ss, (struct sockaddr* ) &server_sockaddr, sizeof(server_sockaddr))==-1) {
            // std::cout << ""
            // perror("bind");
            exit(1);
        }

        if(listen(ss, QUEUE) == -1) {
            // perror("listen");
            exit(1);
        }

        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        /// Successful return of non-negative descriptor, error Return-1
        int conn = accept(ss, (struct sockaddr*)&client_addr, &length);

        if( conn < 0 ) {
            perror("connect");
            exit(1);
        }

        while(1) {
            /*Clear the collection of readable file descriptors*/
            FD_ZERO(&rfds);
            /*Add standard input file descriptors to the collection*/
            FD_SET(0, &rfds);
            maxfd = 0;
            /*Add the currently connected file descriptor to the collection*/
            FD_SET(conn, &rfds);
            /*Find the largest file descriptor in the file descriptor set*/    
            if(maxfd < conn)
                maxfd = conn;
            /*Setting timeout time*/
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            /*Waiting for chat*/
            retval = select(maxfd+1, &rfds, NULL, NULL, &tv);

            if(retval == -1){
                std::cout << "select Error, client program exit\n";
                break;
            }else if(retval == 0){
                std::cout << "The server does not have any input information, and the client does not have any information coming. waiting...\n";
                continue;
            }else{
                /*The client sent a message.*/
                if(FD_ISSET(conn, &rfds)){
                    char buffer[1024];    
                    memset(buffer, 0 ,sizeof(buffer));
                    int len = recv(conn, buffer, sizeof(buffer), 0);
                    // if(strcmp(buffer, "exit\n") == 0) break;
                    // printf("%s", buffer);
                    std::cout << "Got new message: " << buffer << std::endl;
                    std::string str(buffer);
                    std::cout << "Checking... " << str.size() << std::endl;
                    ReturnData retData = sumOfNumbersInString(str);
                    // std::cout << "sortedArray: " << retData.sortedArray << ", sunOfNums: " << retData.sunOfNums << std::endl;
                    // char buf[1024];
                    // const char *buf = retData.sortedArray.c_str();
                    std::cout << "sortedArray: " << retData.sortedArray << ", sunOfNums: " << retData.sunOfNums << std::endl;
                    send(conn, &retData.sortedArray[0], retData.sortedArray.size(), 0); 
                    // send(conn, &retData.sunOfNums[0], retData.sunOfNums.size(), 0); 
                    //Send (conn, buffer, len, 0); send back the data to the client
                }
                /*When the user enters the information, he begins to process the information and send it.*/
                // if(FD_ISSET(0, &rfds)){
                //     char buf[1024];
                //     fgets(buf, sizeof(buf), stdin);
                //     //printf("you are send %s", buf);
                //     send(conn, buf, sizeof(buf), 0);    
                // }
            }
        }
        close(conn);
        close(ss);
        return;
    }
};


class UdpServer: public SocketServer {
public:
    int port;

    UdpServer(int _port): port(_port) {};
    UdpServer(): UdpServer(PORT) {};

    void workServ() {
        fd_set rfds;
        struct timeval tv;
        int retval, maxfd;
        int ss = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in server_sockaddr, cliAddr;
        server_sockaddr.sin_family = AF_INET;
        server_sockaddr.sin_port = htons(this->port);
        
        server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(ss, (struct sockaddr* ) &server_sockaddr, sizeof(server_sockaddr))==-1) {
            // std::cout << ""
            perror("bind");
            exit(1);
        }

        socklen_t cliAddrLen;
        cliAddrLen = sizeof(cliAddr);
        // readStatus = recvfrom(serSockDes, buff, 1024, 0, (struct sockaddr*)&cliAddr, &cliAddrLen);
        int nbytes;
        while(1) {
            /*Clear the collection of readable file descriptors*/
            FD_ZERO(&rfds);
            /*Add standard input file descriptors to the collection*/
            // FD_SET(0, &rfds);
            maxfd = 0;
            /*Add the currently connected file descriptor to the collection*/
            FD_SET(ss, &rfds);
            /*Find the largest file descriptor in the file descriptor set*/    
            if(maxfd < ss)
                maxfd = ss;
            /*Setting timeout time*/
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            /*Waiting for chat*/
            retval = select(maxfd+1, &rfds, NULL, NULL, &tv);

            if(retval == -1){
                std::cout << "select Error, client program exit\n";
                break;
            }else if(retval == 0){
                std::cout << "The server does not have any input information, and the client does not have any information coming. waiting...\n";
                continue;
            }else{
                /*The client sent a message.*/
                if(FD_ISSET(ss, &rfds)){
                    char buffer[1024];    
                    memset(buffer, 0 ,sizeof(buffer));
			        nbytes = recvfrom(ss, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliAddr, &cliAddrLen);
                    if (nbytes > 0) {

                        std::cout << "Got new message: " << buffer << std::endl;
                        std::string str(buffer);
                        std::cout << "Checking... " << str.size() << std::endl;
                        ReturnData retData = sumOfNumbersInString(str);
                        // std::cout << "sortedArray: " << retData.sortedArray << ", sunOfNums: " << retData.sunOfNums << std::endl;
                        // char buf[1024];
                        // const char *buf = retData.sortedArray.c_str();
                        std::cout << "sortedArray: " << retData.sortedArray << ", sunOfNums: " << retData.sunOfNums << std::endl;

                        nbytes = sendto(ss, &retData.sortedArray[0], retData.sortedArray.size(), 0, (struct sockaddr*) &cliAddr, sizeof(cliAddr));
                        if(nbytes < 0) {
                            perror("ERROR in sendto");
                            close(ss);
                            exit(1);
                        }
                    }
                }   
            }
        }
        // close(conn);
        close(ss);
        return;
    }
};


int main(int argc, char* argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "udp")) {
            UdpServer udp;

            udp.workServ();

        } else if (strcmp(argv[1], "tcp")) {
            TcpServer tcp;

            tcp.workServ();

        } else {

        }
    } else {
        std::cout << "Wrong number of arguments: " << argc << std::endl; 
    }

    return 0;
}