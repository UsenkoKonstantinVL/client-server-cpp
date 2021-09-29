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
#include <errno.h>

#define MYPORT  7000
#define BUFFER_SIZE 1024

int tcpClient()
{
    /// Define sockfd
    int sock_cli = socket(AF_INET, SOCK_STREAM, 0);

    /// Define sockaddr_in
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  /// Server Port
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  /// server ip

    //Connect to the server, successfully return 0, error return - 1
    if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        exit(1);
    }

    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        send(sock_cli, sendbuf, strlen(sendbuf),0); /// Send
        if(strcmp(sendbuf,"exit\n")==0)
            break;

        recv(sock_cli, recvbuf, BUFFER_SIZE, 0); /// Receiving

        std::string recStr(recvbuf);
        std::cout << recStr << std::endl;

        // memset(recvbuf, 0, sizeof(recvbuf));

        // recv(sock_cli, recvbuf, BUFFER_SIZE, 0); /// Receiving
        // std::string recStr2(recvbuf);
        // std::cout << recStr2 << std::endl;
        // fputs(recvbuf, stdout);

        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock_cli);
    return 0;
}

int udpClient()
{
    /// Define sockfd
    int sock_cli = socket(AF_INET, SOCK_DGRAM, 0);

    /// Define sockaddr_in
	struct sockaddr_in client;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(MYPORT);  /// Server Port
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  /// server ip

    //Connect to the server, successfully return 0, error return - 1
    // if (connect(sock_cli, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    // {
    //     perror("connect");
    //     exit(1);
    // }

    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];

    int rc;
    socklen_t cliAddrLen = sizeof(struct sockaddr_in);
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        if( rc = sendto(sock_cli, sendbuf, strlen(sendbuf),0,
			 (struct sockaddr*)&servaddr,sizeof (servaddr)) < 0 ) {
			/* buffers aren't available locally at the moment,
			 * try again.
			 */
			if (errno == ENOBUFS)
				continue;
			perror("sending datagram");
			exit(1);
		}

		/* read acknowledgement from remote system
		*/
		if (recvfrom(sock_cli, recvbuf, BUFFER_SIZE, 0, (struct sockaddr*)&servaddr, &cliAddrLen) < 0 ) {
			printf("server error: errno %d\n",errno);
			perror("reading datagram");
			exit(1);
		}

        std::string recStr(recvbuf);
        std::cout << recStr << std::endl;

        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock_cli);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        if (strcmp(argv[1], "udp")) {
            udpClient();

        } else if (strcmp(argv[1], "tcp")) {
            tcpClient();

        } else {

        }
    } else {
        std::cout << "Wrong number of arguments: " << argc << std::endl; 
    }

    return 0;
}