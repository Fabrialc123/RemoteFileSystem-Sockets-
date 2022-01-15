# RemoteFileSystem-Sockets-
A Remote File System developed in "Distributed Systems" using sockets during 2021/2022.

For usage: open the server side with 


    ./serverRemoteFile PORT
    
Where PORT is the port to be used (has to be >1024).

For client side:

    ./clientRemoteFile   SERVERIP SERVERPORT FILEPATH

Where SERVERIP and SERVERPORT refers to the machine used as server (IP and port, respectively), and FILEPATH is the file to be uploaded in the server.

The uploaded files in the server side are extended with ".new".
