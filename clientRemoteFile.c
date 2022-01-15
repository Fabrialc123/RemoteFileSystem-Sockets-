#include "fileServer.h"

int main(int argc, char *argv[]){

    int socketfd;						/** Socket descriptor */
    unsigned int port;					/** Port number */
    struct sockaddr_in server_address;	/** Server address structure */
    char* serverIP;						/** Server IP */
    
    FILE *fileDescriptor;				/** File descriptor */
    struct stat st;						/** Structure with file's info */
    char* fileName;						/** Name of the file to be transferred */
    unsigned int fileNameLength;		/** Length of the file name */   
    unsigned int fileSize;				/** Size of the file */
    
    unsigned int readBytes;				/** Number of read bytes */  
    unsigned int totalBytesRead;			/** Total number of read bytes from file */  
    unsigned int bytesSent;				/** Number of bytes sent */ 
    unsigned int bytesReceived;			/** Bytes received */   
    unsigned int ack;					/** Ack */
    
    void* message;						/** Message sent to the server side */
    						
		
		// Clear buffer
		message = (void *) malloc (MAX_MSG_LENGTH);
		memset(message, 0, MAX_MSG_LENGTH);
		

		// Check arguments!
		if (argc != 4){
			fprintf(stderr,"ERROR wrong number of arguments\n");
		   	fprintf(stderr,"Usage:\n$>%s host port fileName\n", argv[0]);
		   	exit(0);
		}

		// Get the server address
		serverIP = argv[1];
		
		// Get the port
		port = atoi(argv[2]);
		
		// Get the file name
		fileName = argv[3];
		
		
		// Create socket
		socketfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		

		// Check if the socket has been successfully created
		if (socketfd < 0)
			showError("ERROR while opening socket");			
				

		// Fill server address structure
		memset(&server_address, 0, sizeof(server_address));
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = inet_addr(serverIP);
		server_address.sin_port = htons(port);		

		// Connect with server
		if(connect(socketfd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
			showError("ERROR while stablishing connection");		

		printf ("Connection established with server!\n");

		// Open file
		if((fileDescriptor = fopen(fileName, "r")) < 0){
			showError("ERROR: Source file cannot be opened \n");
		}

		// Check!
		if (fileDescriptor == NULL){
			showError("ERROR while opening file");
		}

		printf ("File has been successfully opened!\n");		
				
		// Get file size
		fseek(fileDescriptor, 0L, SEEK_END); // seek to end of file
		fileSize = ftell(fileDescriptor); // get current file pointer
		fseek(fileDescriptor, 0L, SEEK_SET); // seek back to beginning of file	
		
					
  		printf("File size: %d bytes\n", fileSize);
	
		// Get the file name length
		fileNameLength = strlen(fileName);	
		printf("File name: %s -> %d characteres\n", fileName, fileNameLength);		
		
		
		// Send size of the file name
		if((send(socketfd,&fileNameLength,sizeof(fileNameLength),0) < 0)){
			showError("ERROR while sending file name size");
		}
		
				
		// Send file name
		if((send(socketfd,fileName,fileNameLength,0) < 0)){
			showError("ERROR while sending file name");
		}		
		
		
		// Send file size
		if((send(socketfd,&fileSize,sizeof(unsigned int),0) < 0)){
			showError("ERROR while sending file size");
		}		
			
				
		// Send file contents
		totalBytesRead = bytesReceived = bytesSent = 0;


		do {
			if (totalBytesRead + MAX_MSG_LENGTH < fileSize) readBytes = MAX_MSG_LENGTH;
			else readBytes = fileSize - totalBytesRead;

			if((fread(message,1,readBytes,fileDescriptor) < readBytes)){
				printf("ERROR in reading file: %d / %d \n", readBytes, fileSize);
				memset(message,1,readBytes);
			}

			totalBytesRead += readBytes;

			printf("(%d%)Sending %d / %d bytes \n", (int) (((float) totalBytesRead / fileSize) * 100),totalBytesRead, fileSize);

			if((send(socketfd,message,readBytes,0) < 0)){
				showError("ERROR while sending file contents");
			}			
		}	while (totalBytesRead < fileSize);		

		// File sent
		printf ("File has been sent to server side\n");		
		
		// Wait for ACK!!!
		if((recv(socketfd,&ack,sizeof(unsigned int),0)) < 0){
			showError("ERROR while receiving ACK");
		}
		
		
		printf("ACK received! Closing...\n\n");
	

		// Close file
		fclose (fileDescriptor);

		// Close socket
		close(socketfd);

	return 0;
}
