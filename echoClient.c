/* ========================================================================== */
/*                                                                            */
/*   echoClient.c                                                             */
/*   (c) 2012 Tomas Palenik                                                   */
/*                                                                            */
/*   Client opens a LOCAL-type socket and sends a message to server.          */
/* 		The message should be a lo-case string.
/*		After sending the message, 
		client expects the server to reply with a response message.           */
/* ========================================================================== */
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <netinet/in.h>
#include "settings.h"
#include <arpa/inet.h>

char Buff[ MAX_MSG_SIZE ] ;		// output message buffer
char Fubb[ MAX_MSG_SIZE ] ;		// input message buffer


int Sock_fd = -1 ;

void cleanup( void ){
	if( Sock_fd != -1 ){
		close( Sock_fd ) ;
		Sock_fd = -1 ;
	}
}

//client code
int main( int argc, const char *argv[] ){
    srand(time(NULL)); // Инициализация генератора случайных чисел

   int len ;
  // struct sockaddr_un address ;//usual (CS)
//   struct sockaddr_in addr; // IPV4
    struct sockaddr_in6 addr;
    struct in6_addr ipv4_mapped_ipv6_addr;

    socklen_t addr_size;
   int result ;
   // char *ip = "127.0.0.1";
//    const char *filename = "chat.txt";
//    const char *message = "Hello i am Daniil!";
    int port = 5000;
    const char *ip = argv[1];
      const char *filename = argv[2];
    const char *message = argv[3];


    if (strlen(message) > MAX_MSG_SIZE){
        printf("size message is bigger than max size");
        return 1;
    }
  // if( argc < 2 ){
   	//	printf("client error: no socket name specified.") ;
   	//	return 1 ;
   //}
   atexit( cleanup ) ;
   //create unnamed socket

    Sock_fd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (Sock_fd == -1) {

        perror("socket");
        return 1;
    }
    /*IPV4*/
//    memset(&addr, '\0', sizeof(addr));
//    addr.sin_family = AF_INET;
//    addr.sin_port = port;
//    addr.sin_addr.s_addr = inet_addr(ip);

    /*IPV6*/
    addr.sin6_family = AF_INET6;
    // Преобразование IPv4 в IPv4-mapped IPv6 адрес
    if (inet_pton(AF_INET6, ip, &(addr.sin6_addr)) == -1) {
        perror("inet_pton");
        return 1;
    }
    addr.sin6_port = htons(port);


    //set server socket name (CS)
//    strcpy(address.sun_path, SERVER_ADDRESS);
//    address.sun_family = AF_UNIX;
//    len = sizeof(address);

    //connect to server
    result = connect(Sock_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (result == -1) {
        perror("connect");
        return 1;
    }
    printf("Connected to server\n");
   printf("client sending message:%s\n", message ) ;

   //SENDING message to server
// Наполнение буфера данными для отправки
    strcpy(Buff, message);
// Отправка сообщения на сервер
    result = write(Sock_fd, Buff, strlen(Buff));
    if (result == -1) {
        perror("write");
        return 1;
    }
   //read answer from socket
    int bytes_received = read(Sock_fd, Fubb, sizeof(Fubb));
    if (bytes_received == -1) {
        perror("read");
        exit(1);
    }
    Fubb[bytes_received] = '\0';

    printf("client received reply from server:%s\n", Fubb);

    //write message in te file
    //int fd = open("chat.txt", O_WRONLY | O_CREAT | O_APPEND);
    int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND);
    if (fd == -1) {
        perror("open");
        return 1;
    }
    // end of file
    off_t file_size = lseek(fd, 0, SEEK_END);
// Сгенерировать псевдослучайный числовой префикс
    int random_prefix = rand() % 1000; // Пример псевдослучайного числа от 0 до 999
    char output_buffer[MAX_MSG_SIZE + 10]; // Префикс может содержать до 4 цифр и символ '\0'
    sprintf(output_buffer, "%03d %s\n", random_prefix, Fubb); // Форматирование с префиксом
// Записать данные в файл
    printf("\nmessage which writing in the file: %s", output_buffer);
    int bytes_written = write(fd, output_buffer, strlen(output_buffer));
    if (bytes_written == -1) {
        perror("write");
        return 1;
    }
    close(fd);
   
   //close connection
	close( Sock_fd ) ;
	Sock_fd = -1 ;
     
	return 0 ; 
    
}
