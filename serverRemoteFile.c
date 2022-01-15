#include "fileServer.h"

int main(int argc, char *argv[]){

     int socketfd;						/** Socket descriptor */
     int newsockfd;						/** Socket descriptor */
     unsigned int port;					/** Listening port */
     struct sockaddr_in serverAddress;	/** Server address structure */
     struct sockaddr_in clientAddress;	/** Client address structure */
     unsigned int clientLength;			/** Length of client structure */     
     
     FILE *fileDescriptor;				/** File descriptor */
     unsigned int fileSize;				/** File size */
     char fileName[MAX_NAME_SIZE];		/** Maximum length for file name */
     unsigned int fileNameLength;		/** Length of the file name */  
     
     unsigned int bytesReceived;		/** Bytes received */
     unsigned int totalBytesReceived;	/** Total bytes received */	
     unsigned int bytesSent;			/** Number of bytes sent */
     
     void * message;					/** Message */    
     char* charPtr;						/** Pointer to read the file name */
     unsigned int ack;					/** Ack */
     
     
     	// Init...
     	bytesReceived = totalBytesReceived = 0;
     
     	// Clear buffer
		message = (void *) malloc (MAX_MSG_LENGTH);
		memset (message, 0, MAX_MSG_LENGTH);
		memset (fileName, 0, MAX_NAME_SIZE);


     	 // Check arguments
		 if (argc < 2) {
			 fprintf(stderr,"ERROR wrong number of arguments\n");
			 fprintf(stderr,"Usage:\n$>%s port\n", argv[0]);
			 exit(1);
		 }

		 // Create the socket
		socketfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

		 // Check
		if (socketfd < 0)
			showError("ERROR while opening socket");		

		 // Init server structure
		 memset(&serverAddress, 0, sizeof(serverAddress));

		 // Get listening port
		 port = atoi(argv[1]);

		 // Fill server structure
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr= htonl(INADDR_ANY);
		serverAddress.sin_port = htons(port);		

		 // Bind
		if(bind(socketfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
			showError("ERROR while binding");		

		 // Listen
		listen(socketfd,10);

		 // Get length of client structure
		 clientLength = sizeof(clientAddress);

		while(1){
			// Accept!
			newsockfd = accept(socketfd, (struct sockaddr *) &clientAddress, &clientLength);

			// Check accept result
			if (newsockfd < 0)
				showError("ERROR while accepting connection");	


			// Read length of file name
			if(recv(newsockfd,&fileNameLength , sizeof(fileNameLength),0) < 0)
				showError("ERROR while receiving the file name lenght");
		
			printf ("Received length of file name: %d\n", fileNameLength);
			

			// Read the file name
			if(recv(newsockfd,&fileName , fileNameLength,0) < 0)
				showError("ERROR while receiving the file name");		
			
			
			// Add a suffix to differentiate files
			strcat (fileName, ".new");
			
			
			printf ("Received file name: %s\n", fileName);

			
			// Read file size
			if(recv(newsockfd,&fileSize , sizeof(unsigned int),0) < 0)
				showError("ERROR while receiving the file size");		
		
		
			printf ("Received file Size: %d\n", fileSize);
			
			
			// Create file in the server side
				if((fileDescriptor = fopen(fileName, "w+")) < 0){
					showError("ERROR: Target file cannot be open to append data");
				}		
			
			
			// Check!
			if (fileDescriptor == NULL){
				showError("ERROR while opening file");
			}		
			
			
			// Read data from client
			do{
				if (totalBytesReceived + MAX_MSG_LENGTH < fileSize) bytesReceived = MAX_MSG_LENGTH;
				else bytesReceived = fileSize - totalBytesReceived;

				if(recv(newsockfd,message,bytesReceived,0) < 0)
					showError("ERROR while receiving the file");

				if(fwrite (message, 1,bytesReceived,fileDescriptor) < bytesReceived){
					showError("ERROR while writing the file");
				}
				totalBytesReceived += bytesReceived;
			}while (totalBytesReceived < fileSize);		
			
					
			printf ("File has been successfully received\n");	
			
			// Send ACK
			ack = EVERYTHING_OK;
			if(send(newsockfd, &ack, sizeof(unsigned int),0) < 0){
				showError("ERROR while sending ACK");
			}
				
			printf("ACK sent! Closing...\n\n");	
			
			// Close file
			fclose (fileDescriptor);

			memset(fileName,0,MAX_NAME_SIZE);
		}	

		// Close sockets
		close(newsockfd);
		close(socketfd);

     return 0; 
}


