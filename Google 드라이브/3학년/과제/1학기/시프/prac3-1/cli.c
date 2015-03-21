//////////////////////////////////////////////////////////////////
//  File Name	 	: cli.c					//
//  Date		: 2014/05/25				//
//  OS			: Ubuntu 12.04 LTS 64bits		//
//  Author		: Lim Jae Won				//
//  Student ID		: 2010720116				//
//--------------------------------------------------------------//
//  Title : System Programming Practice #3-1			//
//  Description : User Authentication, Access Control program	//
//////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUF_SIZE 256
#define MAX_BUF 20
#define CONT_PORT 20001

void log_in(int sockfd);
int main(int argc, char *argv[])
{
	int sockfd, n, p_pid;
	char buf[MAX_BUF];
	struct sockaddr_in servaddr;
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n",argv[0]);
		exit(1);
	}
//socket create
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd ==-1){
		perror("socket() error");
		exit(1);
	}
//socketaddr_in structure
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	servaddr.sin_port=htons(atoi(argv[2]));
//connect
	if(connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1){
		perror("connect() error");
		exit(1);
	}


	n = read(sockfd,buf,MAX_BUF); // read meassage from server 
	buf[n] = '\0';
	//whether client IP is possible for connection or not 
	if(!strcmp(buf,"OK")){//if it is possible
		printf("** Client is connected **\n");
		log_in(sockfd);//call log_in fucntion
	}
	else if(!strcmp(buf,"FAIL")){// if it is not possible
		printf("** Connection refused **\n");
		close(sockfd);
		exit(0);// close socket and exit program
	}

	close(sockfd);
	return 0;
}
		
//////////////////////////////////////////////////////////////////
// log_in							//
//--------------------------------------------------------------//
// input : int cockfd 	-> socket discriptor			//
// output : int		-> return 1 or 0			//
// purpose : this is authentication function			//
//////////////////////////////////////////////////////////////////
void log_in(int sockfd){
	int n;	
	char *user, *passwd, buf[MAX_BUF];
	for(;;) {
		memset(user,0,MAX_BUF);
		memset(buf,0,MAX_BUF);
				
		user = getpass("Input ID : ");// get user ID
		write(sockfd,user,MAX_BUF);//write user ID to server

		n = read(sockfd, buf, MAX_BUF);//read a message 
		buf[n] = '\0';
		if(!strcmp(buf, "OK")){
			passwd = getpass("Input Password : ");// get password
			write(sockfd,passwd,MAX_BUF);//write password to server
		
			n = read(sockfd, buf, MAX_BUF);//read a message wheater it is correct or not
			buf[n] = '\0';

			if(!strcmp(buf, "OK")) {//if it is correct
				printf("** Success to log-in **\n");
				close(sockfd);
				exit(0);
			}
			else if(!strcmp(buf, "FAIL")){//if it is not correct
				printf("** Log-in failed **\n");
			}
			else{ //if it fail three times				
				close(sockfd);
				printf("** Connection closed **\n");				
				exit(0);
				
			}
		}
	}
}


