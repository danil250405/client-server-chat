/*
	Various settings for client and server for minimizing misconfiguration errors.
*/
#ifndef _SETTINGS
#define _SETTINGS 1

//some common settings for client and server programs, add yours ....

#define MAX_MSG_SIZE 255                  // guess what ?
#define SERVER_ADDRESS "server_socket"    // filename for local sockets 

//use the folowing macro for error checking 

static char _errBuf[ 1024 ] ;

#define _runSafe( code ) if((code) == -1){\
	sprintf( _errBuf, "Error in: %s on line: %d:", __FILE__, __LINE__ ) ;\
	perror( _errBuf ) ;\
	exit( 1 ) ;\
}

#endif
