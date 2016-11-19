# distributerFileServer
Distributed File Server For Reliable File Transmission

Author : Surjith Bhagavath Singh
File details	: Read me for Distributed File Server (Programming Assignment 3 - Network Systems)

There are three main files for this programming assignment
	1.client/dfc.c
	2.client/dfc.conf
	3.fileServers/dfs.c
	4.fileServers/dfs{server num}/dfs.conf
	
dfc.c
	This is the client file which takes the username,password and server details from dfc.conf file and do three basic functionalities

	LIST
		This lists the files that are available in the server, if the servers are down it looks for the best combination of the available servers to recreate the file and lists whether it can be reconstructed or not
	PUT
		This command encrypts the file according to username and password details, splits the file into 4 parts and sends it to corresponding servers(According to md5sum modulus value). For redundancy purposes this uploads pairs of file chunks into the server.
	GET
		This function first calls list to get the details of the available server and available filechunks to get the data optimally(Traffic congestion) and if the file is reconstructable, then it gets the data from the server and merge them into a single file. After merging the file this decrypts the file according to the username and password combination

dfs.c
	This is the server code, it takes the folder info for the server and port info to create a socket. This runs in a infinite while loop, to serve the clients. This cross checks the username and password data sent by client with its dfs.conf file. This can handle multiple clients. It stores the recieved files in (.filename.format.chunk_number).It creates a directory for eac user. 

Temporary directories for processing these files are created (encrypted,downloads,temp). The files that are downloaded from the server are kept in downloads folder.
