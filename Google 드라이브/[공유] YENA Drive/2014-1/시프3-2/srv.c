///////////////////////////////////////////////////////////////////////////////
// File Name	: srv.c
// Date			: 2014/06/02
// Number		: Practice #3-2
// Class		: Tuesday 1,2
// Os			: Ubuntu 12.04 64bits
// Author		: Lee Hyun Jae
// Student ID	: 2010720092
// --------------------------------------------------------------------------
// Title		: System Programing Practice #3-2
// Description	: Practice server with data connection
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>

#define BUF_SIZE 256
#define MAX_BUF 8912

char* convert_str_to_addr(char *str, unsigned int *port);
int client_info(struct sockaddr_in *addr);
int cmd_process(char* argv1, char* argv2);

///////////////////////////////////////////////////////////////////////////////
// main
// ==========================================================================
// Input :	int - number of arguments
//			char - Port number
// output : int - 1 fail
//				  0 success
// Purpose : Communicate with client
///////////////////////////////////////////////////////////////////////////////

void main(int argc, char **argv){
	char *cmd;
	char buff[MAX_BUF]={0,};
	char result_buff[MAX_BUF]={0,};
	int n, listenfd, connfd;
	struct sockaddr_in cliaddr, servaddr;
	socklen_t clilen;

///////////////////////////// Socket //////////////////////////////////////////
	listenfd=socket(AF_INET,SOCK_STREAM,0);
///////////////////////////// End of socket ///////////////////////////////////

///////////////////////////// Bind ////////////////////////////////////////////
	memset(&servaddr,0,sizeof(servaddr));
	memset(&buff,0,sizeof(buff));

	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(atoi(argv[1]));

	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
///////////////////////////// End of bind /////////////////////////////////////

///////////////////////////// Listen //////////////////////////////////////////
	listen(listenfd,5);
///////////////////////////// End of listen ///////////////////////////////////

///////////////////////////// Communicate /////////////////////////////////////
	while(1){
		pid_t pid;
		clilen=sizeof(cliaddr);
		connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);

///////////////////////////// Child process ///////////////////////////////////
		if(!(pid=fork())){
			while(1){
				read(connfd,buff,MAX_BUF-1);
				cmd=strtok(buff," \n");

///////////////////////////// Data connection /////////////////////////////////
				if(!strcmp(cmd,"PORT")){
					char *host_ip;
					char *temp;
					int sockfd;
					unsigned int port_num;
					struct sockaddr_in cliaddr;

					temp=strtok(NULL," \n");
					host_ip = convert_str_to_addr(temp,(unsigned int *) &port_num);

///////////////////////////// Socket //////////////////////////////////////////
					sockfd=socket(AF_INET,SOCK_STREAM,0);
///////////////////////////// End of socket ///////////////////////////////////

///////////////////////////// Connect /////////////////////////////////////////
					memset(&cliaddr,0,sizeof(cliaddr));
					cliaddr.sin_family=AF_INET;
					cliaddr.sin_addr.s_addr=inet_addr(host_ip);
					cliaddr.sin_port=htons(port_num);

					connect(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr));
					write(sockfd,"PORT command successful\n",25); //ACK
///////////////////////////// End of connect //////////////////////////////////
					read(sockfd,buff,MAX_BUF-1);
					cmd_process(buff,result_buff); //NLST command
					write(sockfd,result_buff,strlen(result_buff));

					close(sockfd);
				}
///////////////////////////// End of Data connection //////////////////////////
				else if(!strcmp(cmd,"QUIT")){
					write(connfd,"QUIT",4);
					exit(0);
				}
			}
		}
///////////////////////////// End of Child process ////////////////////////////
		else{
			client_info(&cliaddr);
		}
		close(connfd);
///////////////////////////// End of Communicate //////////////////////////////
	}
	close(listenfd);
}

///////////////////////////////////////////////////////////////////////////////
// convert_str_to_addr
// ==========================================================================
// Input :	char - read string from client
//			unsigned int - port numver
// output : char - IP address
// Purpose : Read command and doing it
///////////////////////////////////////////////////////////////////////////////

char* convert_str_to_addr(char *str, unsigned int *port){
	int i;
	char *addr;
	char *temp[6];

	temp[0]=strtok(str,",");
	for(i=1;i<6;i++){ //IP address
		temp[i]=strtok(NULL,",");
		*(temp[i]-1)='.';
	} //end of for
	*(temp[4]-1)='\0';
	addr=str;

	*port=atoi(temp[4]); //port number
	*port=(*port)*256+atoi(temp[5]);

	return addr;
}

///////////////////////////////////////////////////////////////////////////////
// client_info
// ==========================================================================
// Input :	struct sockaddr_in - client address
// output : int - 1 fail
//				  0 success
// Purpose : Read command and doing it
///////////////////////////////////////////////////////////////////////////////

int client_info(struct sockaddr_in *addr){
	char *c_addr=inet_ntoa(addr->sin_addr); //client address
	char port[BUF_SIZE]={0,}; //client port
	int n=ntohs(addr->sin_port); //port number
	int i=n, l=0; //i: for length. l: length of port number

	while(i>0){ //length of port number
		i/=10;
		l++;
	} //end of while

	for(i=0;i<l;i++){ //int to array
		port[l-1-i]=n%10+48;
		n/=10;
	} //end of for

	write(STDOUT_FILENO,"==========Client info==========\n",32); //print out port number
	write(STDOUT_FILENO,"client IP: ",11);
	write(STDOUT_FILENO,c_addr,strlen(c_addr));
	write(STDOUT_FILENO,"\nclient port: ",14);
	write(STDOUT_FILENO,port,strlen(port));
	write(STDOUT_FILENO,"\n===============================\n",33);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// cmd_process
// ==========================================================================
// Input :	int - socket descripter
//			char - receive command
//			char - result
// output : int - 1 fail
//				  0 success
// Purpose : Read command and doing it
///////////////////////////////////////////////////////////////////////////////

int cmd_process(char* argv1, char* argv2){
	memset(argv2,0,sizeof(argv2));

	if(!strcmp(argv1,"NLST")){ //NLST command
		DIR *dp; //directory
		struct dirent *dirp; //directory entry

		if((dp=opendir("."))==NULL){ //open directory
			strcpy(argv2,"No such file or directory\n");
		}

		while((dirp=readdir(dp))!=NULL){
			if(dirp->d_name[0]!='.'){
				strcat(argv2,dirp->d_name);
				strcat(argv2," ");
			} //end of if
		} //end of while
		strcat(argv2,"\n");
		closedir(dp); //close directory
	}
	else{
		return -1;
	} //end of if

	return 0;
}
