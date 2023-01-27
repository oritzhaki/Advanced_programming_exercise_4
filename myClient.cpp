#include "myClient.h"
#include "clientUtilityFunctions.h"

int socketCreation(int server_port, char *ip_address) {
    // Create a socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        cerr << "Error creating socket" << endl;
        return -1;
    }

    // connect to the server
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(ip_address);
    serverAddr.sin_port = htons(server_port);
    if (inet_pton(AF_INET, ip_address, &serverAddr.sin_addr) <= 0) {
        cerr << "Error parsing IP address" << endl;
        return -1;
    }
   
    // Connect to the server
    if (connect(client_socket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error connecting to server" << endl;
        return -1;
    }
    return client_socket;

}

// Method to send local data from client to server
void MyClient::sendData(string message, DefaultIO* dio) {
    DefaultIO *io = dio;
    
    cout << message << endl;
    string path;
    getline(cin, path);
    string fileContent;
    try {
        fileContent = io->readFile(path);
    } catch(...) {
        throw false;
    }

    io->write(fileContent);

}

void MyClient::run(int argc, char** argv) {
    // make sure there are 3 arguments to activate client
    if (argc < 3) {
        cerr << "Usage: client.out IP_ADDRESS PORT" << endl;
        return;
    }

    // Parse the IP address and port number
    char* ip_address = argv[1];
    const int port = stoi(argv[2]);

    int client_socket = socketCreation(port, ip_address);

    DefaultIO *io = new SocketIO(client_socket);
    while(true){
        cout << "";
        string input;
        cout << "";
        string message = io->read();

        cout << "";
        if(message == "Please upload your local train CSV file.") {
            try {
                this_thread::sleep_for(std::chrono::milliseconds(100));
                sendData(message, io); // upload train data
                // read another message from server
                string newMessage = io->read();   // iris_classified.csv
                sendData(newMessage, io); // upload test data

            } catch(...) {
                 // path doesn't exist
                 io->write("");
            }        
            continue;    
        } 
        else if(message == "upload results:"){
            // receive the random port generated by server
            string newPort = io->read();
            int port = stoi(newPort);
            int newSock = socketCreation(port, ip_address);
            string path;
            getline(cin, path); // get path from user to save results
             // download file in a separate thread to allow user to continue sending requests
            thread client_thread([newSock, path]() {
                SocketIO* sio = new SocketIO(newSock);
                this_thread::sleep_for(std::chrono::milliseconds(100));
                sio->saveData(path);
                delete sio;
                close(newSock);
            });
            // Detach the thread so that it can run independently
            client_thread.detach();
            continue;
        }
        else if(message == "EXIT"){ // user pressed 8 so exit
            break;
        
        } else { 
            cout << message << endl;
            getline(cin, input);
            io->write(input);
        }
    }

    delete io;

    // close the socket
    close(client_socket);

    // Close client
    return;
}

 

