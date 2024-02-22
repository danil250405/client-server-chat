/* ========================================================================== */
/*                                                                            */
/*   echoServer.c                                                             */
/*   (c) 2012 Tomas Palenik                                                   */
/*                                                                            */
/*   Server opens a LOCAL-type socket and listens for a message from client.  */
/* 		The message should be a lo-case string.                               */
/*		After receiving the message, 
		the server replies with a response message - client's message 
		converted to upper case letters.							          */
/* ========================================================================== */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include "settings.h"
#include <arpa/inet.h>
#include <netinet/in.h>

char Buff[ MAX_MSG_SIZE ] ;

int server_sockfd = -1 ;
int client_sockfd = -1 ;

void cleanup( void ){
	if( server_sockfd != -1 ){
		close( server_sockfd ) ;
		server_sockfd = -1 ;
	}
	if( client_sockfd != -1 ){
		close( client_sockfd ) ;
		client_sockfd = -1 ;
	}
}

//converts string to upper case
int strToUpper( char *str , int size ){
	int i = 0 ;
	
	while( *str != '\0' &&  i++ < size ){
		*str = toupper( *str ) ;
		str++ ;		
	}
	return i ;
}

int main( int argc, const char *argv[] ){

	int len ;
	int client_len ;

   // char *ip = "127.0.0.1";
   const char *ip = argv[1];
   int port = 5000;
    //local sockets

    //for TCP (IPV6) server
    struct sockaddr_in6 server_addr, client_addr;
    struct in6_addr ipv4_mapped_ipv6_addr;

    //for TCP (IPV4) server
    // struct sockaddr_in server_addr, client_addr;


    //for usual server (SC)
	//struct sockaddr_un server_address ;
	//struct sockaddr_un client_address ;

    socklen_t addr_size;

	//if( argc < 2 ){
   		//printf("server error: no socket name specified.") ;
   		//return 1 ;
  // }

   atexit( cleanup ) ;


    /* Create socket for listening (client requests) */
    server_sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if(server_sockfd == -1) {
        perror("socket");
        return 1;
    }

    /* Set socket to reuse address */
    /*IPV6*/
    int flag = 1, reu;
    reu = setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    if(reu == -1) {
        perror("setsockopt");
        return 1;
    }

    /*IPV4*/
//    server_sockfd = socket(AF_INET6, SOCK_STREAM, 0);
//    if (server_sockfd == -1) {
//        perror("socket");
//        return 1;
//    }

    printf("TCP server socked created!!!\n");
    /*IPV6*/
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, ip, &(server_addr.sin6_addr)) == -1) {
        perror("inet_pton");
        return 1;
    }
    server_addr.sin6_port = htons(port);



    //заполняем массив елементами
    /* For IPV4*/
    //memset(&server_addr, '\0', sizeof(server_addr));
    //server_addr.sin_family = AF_INET;
    //server_addr.sin_port = port;
    //server_addr.sin_addr.s_addr = inet_addr(ip);
    /*bind a name to socket (second step IPV4)*/
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }
    printf("Bind to the port number: %d\n", port);

	//bind a name to socket (CS)
//    server_address.sun_family = AF_UNIX;
//    strcpy(server_address.sun_path, SERVER_ADDRESS);
//    unlink(server_address.sun_path); // Удаляем существующий файл с таким же именем, если есть
//    if (bind(server_sockfd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
//        perror("bind");
//        exit(1);
//    }

	//start listening on the named socket
    if (listen(server_sockfd, 5) == -1) { // Размер очереди входящих соединений - 5
        perror("listen");
        exit(1);
    }
	while( 1 ) {

		printf("Server waiting.\n") ;
		
		//accept incomming client connection
        client_len = sizeof(client_addr);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_sockfd == -1) {
            perror("accept");
            exit(1);
        }
        printf("Client connected.\n");

		//read message from client
        int bytes_received = read(client_sockfd, Buff, sizeof(Buff));
        if (bytes_received == -1) {
            perror("read");
            exit(1);
        }
        // Добавление завершающего нулевого символа к принятой строке
        Buff[bytes_received] = '\0';

        // Вывод принятого сообщения на стороне сервера
        printf("Server received message: %s\n", Buff);

        //convert client's message to upper case and reply to client
        int converted_length = strToUpper(Buff, bytes_received);
        printf("Server received message (uppercase): %s\n", Buff);
        int bytes_sent = write(client_sockfd, Buff, converted_length);
        if (bytes_sent == -1) {
            perror("write");
            exit(1);
        }
		//close temporary socket
		close( client_sockfd ) ;
		client_sockfd = -1 ;
	}
	
	//program will never reach this point
	return 0 ;
}
