
#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <string.h>

using namespace Sync;
bool flag=true;


// creating a thread class called client thread
//this class include all the functionality that needs to write the msg to socket and server.

class ClientThread : public Thread{
	private:
		Socket& socket; //the socket we created to connect client and server

		//the bytearray is used to communicate between server and socket
		//the string is ued to communicate between client and socket
		ByteArray msg;
		std::string msg_str;

	public:
		ClientThread(Socket& socket): socket(socket){} // using the client thread based on the socket we create to have user interface

		~ClientThread(){}

		virtual long ThreadMain()
		{
			bool trigger = false;// initially, we set the state of the server connection with the server is close 
			try{// by first trying the 
				int result=socket.Open();
			}catch(...){// once the the error is catched 
				std::cout<< "Need to run server"<<std::endl;
				trigger=true;// set the server trigger is still on 
				flag=false;// notify that the socket is running
				// notify the client that the server is still on
			}

			while(true){
				if(trigger==true){// if the trigger is true
					break; //break the loop
				}
				std::getline(std::cin,msg_str);//getting the user input and store to msg_string.
				msg=ByteArray(msg_str);// turn the string message to byteArray type,
				// so the socket can send to the server 


				socket.Write(msg);//write the msg byteArray to the socket
				if(msg_str=="done"){// if the 
					flag=false;
					break;
				}

				socket.Read(msg);//get the feedback from the server
				if(msg.ToString()!="Message received"){
					flag=false;
					break;
				}
				msg_str=msg.ToString();//convert ByteArray msg to string and print
				std::cout<<msg_str<<std::endl;
			}

			std::cout<<"closing client"<<std::endl;
			return 0;
		}
};

int main(void)
{
	// Welcome the user 
	std::cout << "SE3313 Lab 3 Client" << std::endl;

	// Create our socket

	Socket socket("127.0.0.1", 3000);
	ClientThread clientThread(socket);
	while(flag){
		sleep(1);
	}

	try{// trying to close the socket
		socket.Close();
		std::cout<<"socket closing"<<std::endl;
	}catch(...){// if fails, all socket must be closed.
		std::cout<<"ending"<<std::endl;
	}

	return 0;
}
