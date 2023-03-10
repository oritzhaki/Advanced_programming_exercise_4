#include "myServer.h"

MyServer::MyServer(){
    this->threadStatus.resize(this->THREAD_POOL_SIZE, true);
}

int sockAccept(int sock) {
    struct sockaddr_in client_sin; /* address struct for the sender info */
    unsigned int addr_len = sizeof (client_sin);
    /* accept creates a new client socket for the connecting client */
    int client_sock = accept(sock, (struct sockaddr *) &client_sin, &addr_len);
    if (client_sock < 0) {
        perror("Error accepting client");
        return -1;
    }
    return client_sock;
}


int creatServerSock(int port) {
    const int server_port = port;
    // socket creation, sock_stream is a const for TCP
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error creating socket");
        exit(-1);
       
    }

    /* creating the struct for the address */
    struct sockaddr_in server_sin;     /* struct for the address */
    memset(&server_sin, 0, sizeof (server_sin));  /* It copies a single character for a specified number
    * of times to an object (sin) */
    server_sin.sin_family = AF_INET;   /* address protocol type */
    server_sin.sin_addr.s_addr = INADDR_ANY; /* const for any address */
    server_sin.sin_port = htons(server_port); /* defines the port */
    /* binding the socket to the port and ip_address, while checking it can be done */
    if (bind(sock, (struct sockaddr *) &server_sin, sizeof (server_sin)) < 0) {
        perror("Error binding socket");
        exit(-1);
    }
    /* listens up to 5 clients at a time */
    if (listen(sock, 5) < 0) {
        perror("Error listening to a socket");
        exit(-1);

    }
    return sock;
}

void MyServer::handleClient(int sock) { // for individual client
    DefaultIO *io = new SocketIO(sock);
    CLI cli(io);
    cli.start();
    delete io; // if we got here user doesn't want to play anymore
    close(sock);
}

// the server is activated by: server.out port
void MyServer::runServer(int argc, char** argv) {
    // make sure there are 2 arguments to activate server
    if (argc != 2) {
        cerr << "Usage: server.out PORT" << endl;
        return;
    }
    int server_port;
    try{ // checking port validation
        server_port = stoi(argv[1]);
        if((server_port < 1024) || (server_port > 65535)){
            throw false;
        }
    } catch (...){
        cout << "Server: Invalid port, goodbye!" << endl;
        return;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0); // sock is the socket descriptor (socket ID)
    // SOCKSTREAM is communication type TPC. AF_NET is the communication domain for IPV4 (IP of different hosts)
    if(sock < 0){ // creation fails
        perror("Server: error creating socket, goodbye!");
        return;
    }

    struct sockaddr_in sin; // the socket address (IP+port)
    memset(&sin, 0, sizeof(sin)); // sets sin to zeros
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port); // converts port to bytes
    if(::bind(sock, (struct sockaddr *) &sin, sizeof(sin)) < 0){  // binding socket to port
        perror("Server: error binding socket, goodbye!");
        return;
    }

    if(listen(sock, 100) < 0){ // puts the server socket in a passive mode and waits for the client to make a connection.
        perror("Server: error listening to socket, goodbye!");
        return;
    }

    vector<thread> active_threads; // the thread pool

    while(true) { // new client - new thread

        struct sockaddr_in client_sin; // the socket address (IP+port)
        unsigned int addr_len = sizeof(client_sin);
        int client_sock = accept(sock, (struct sockaddr *) &client_sin, &addr_len); // create connection socket, ready to transfer data
        if(client_sock < 0){  // creation/connection fails
            perror("Server: error accepting client, goodbye!");
            continue; // get new client
        }
        active_threads.emplace_back(&MyServer::handleClient, this, client_sock);
        active_threads.back().detach();
    }
    
    for (auto& client_thread : active_threads) {
        client_thread.join();
    }

    close(sock); 
    return;
}