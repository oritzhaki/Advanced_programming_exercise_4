#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdio.h> 
#include "DefaultIO.h"

using namespace std;


class SocketIO : public DefaultIO{
private:
    int socket;
    

public:
    SocketIO(int client_sock);
    string read();
    void write(const string& data);
    void writeFile(const string& filePath, const string& contents);
    void saveData(string dataType);
    string readFile(const string& filePath);
    void close();
    const int BUFFER_SIZE = 4096;
};