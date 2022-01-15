#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_MSG_LENGTH 1024		// Maximum length of the message
#define MAX_NAME_SIZE 256		// Maximum length of the file name
#define EVERYTHING_OK 111		// ACK


/** Function that shows an error message */
void showError(const char *msg);