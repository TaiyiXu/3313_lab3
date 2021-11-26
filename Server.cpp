#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>
#include <string.h>

using namespace Sync;

// This thread handles the server operations

class SocketThread: public Thread// create an another instance taht represent the socket connect to the server to handle the byteArray from client
{
    private:
        Socket& socket;//the socket connecting to the server.
        ByteArray msg;//the client message

    public:
        SocketThread(Socket& socket): socket(socket){}

        ~SocketThread(){}

        Socket& GetSocket()
        {
            return socket;
        }

        virtual long ThreadMain()
        {
            while(true)
            {
                try{
                    socket.Read(msg);// reading the data from socket.
                    std::cout<<"Reading message"<<std::endl;

                    std::string str=msg.ToString();//covert byteArray to string.
                    if(str=="done")//so the application can check if the msg is termination cmd
                    {
                        std::cout<<"Socket terminated"<<std::endl<<"Press enter to terminate the sever..."<< std::endl;
                        break;
                    }

                    msg=ByteArray(str);//once we done checking with the cmd, dispaly the message on server
                    std::cout<<"Received message from client: "<<str<<std::endl;

                    //send the receive msg to the client so they know the msg was sent to the server, successfully
                    msg=ByteArray("Message received");
                    socket.Write(msg);
                    std::cout<<"message sent"<< std::endl;

                }catch(...){

                }
            }
            return 0;
        }

};

class ServerThread : public Thread
{
private:
    SocketServer& server;
    std::vector<SocketThread*> socketThreadvector;//create a verctor list of socketThread to keep track how many socket are connected to the server
    bool terminate = false;

public:
    ServerThread(SocketServer& server)
    : server(server)
    {}

    ~ServerThread()
    {
        // Cleanup
        for(int i =0; i<socketThreadvector.size();i++){
            Socket& socket=socketThreadvector[i]->GetSocket();
            socket.Close();
        }

    }

    virtual long ThreadMain()
    {
        while (true)
        {
        // Wait for a client socket connection
        Socket* newConnection = new Socket(server.Accept());

        // A reference to this pointer 
        Socket& socketReference = *newConnection;

        // whenever a new connection is made, push it to the socket thread vector list
        socketThreadvector.push_back(new SocketThread(socketReference));
        }
    }
};


int main(void)
{
    std::cout << "I am a server." << std::endl;
    std::cout << "Enter to terminate the server if you opened it by mistake" << std::endl;
    std::cout.flush();

    // Create our server
    SocketServer server(3000);    

    // Need a thread to perform server operations
    ServerThread serverThread(server);
	
    // This will wait for input to shutdown the server
    FlexWait cinWaiter(1, stdin);
    cinWaiter.Wait();// wait for client to enter new message 
    std::cin.get();

    //by entering enter, the server will read as pass this lane and then will execute next command line,
    //which is shutdone();
    // Shut down and clean up the server
    server.Shutdown();

}
