///////////////////////////////////////////////////////////////////////////////
// File Name	: cli.c
// Date			: 2014/06/02
// Number		: Practice #3-2
// Class		: Tuesday 1,2
// Os			: Ubuntu 12.04 64bits
// Author		: Lee Hyun Jae
// Student ID	: 2010720092
// --------------------------------------------------------------------------
// Title		: System Programing Practice #3-2
// Description	: Practice client with data connection
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 256
#define MAX_BUF 8912

char* convert_addr_to_str(unsigned long ip_addr, unsigned int port);
char* itoa(int num); //integer to ascii

///////////////////////////////////////////////////////////////////////////////
// main
// ==========================================================================
// Input :	int - number of arguments
//			char - IP, Port number
// output : int - 1 fail
//				  0 success
// Purpose : Read command and doing it
///////////////////////////////////////////////////////////////////////////////

void main(int argc, char **argv){
	int sockfd, n;
	char* cmd;
	char buf[MAX_BUF]={0,};
	char cbuf[MAX_BUF]={0,};
	char rbuf[MAX_BUF]={0,};
	struct sockaddr_in servaddr;
	
	srand(time(NULL)); //for random
///////////////////////////// Control connection //////////////////////////////

///////////////////////////// Socket //////////////////////////////////////////
	sockfd=socket(AF_INET,SOCK_STREAM,0);
///////////////////////////// End of socket ///////////////////////////////////

///////////////////////////// Connect /////////////////////////////////////////
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	servaddr.sin_port=htons(atoi(argv[2]));

	connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
///////////////////////////// End of connect //////////////////////////////////

///////////////////////////// Communicate /////////////////////////////////////
	while(1){
		memset(buf,0,sizeof(buf));
		memset(cbuf,0,sizeof(cbuf));

		write(STDOUT_FILENO,"> ",2);
		read(STDIN_FILENO,buf,sizeof(buf));

		cmd=strtok(buf, " \n");

		if(!strcmp(cmd,"quit")){
			write(sockfd,"QUIT",5);
			n=read(sockfd,rbuf,MAX_BUF-1);
			rbuf[n]='\0';
			if(!strcmp(rbuf,"QUIT")){
				write(STDOUT_FILENO,"Program quit\n",13);
				break;
			}
			write(STDOUT_FILENO,rbuf,n);
		}
///////////////////////////// Data connection /////////////////////////////////
		else if(!strcmp(cmd,"ls")){
			int port_num, listenfd, connfd;
			char *hostport;
			struct sockaddr_in serv,temp;
			socklen_t clilen;

			port_num=(rand()%20000)+10001; //random port number

			listenfd=socket(AF_INET,SOCK_STREAM,0);

			memset(&temp, 0, sizeof(temp));
			temp.sin_family=AF_INET;
			temp.sin_addr.s_addr=inet_addr("127.0.0.1");
			temp.sin_port=htons(port_num);

			bind(listenfd,(struct sockaddr*)&temp,sizeof(temp));
			listen(listenfd,5);

			hostport = convert_addr_to_str(temp.sin_addr.s_addr, temp.sin_port);
			write(STDOUT_FILENO,"converting to ",14);
			write(STDOUT_FILENO,hostport,strlen(hostport));
			write(STDOUT_FILENO,"\n",1);

			write(sockfd,hostport,strlen(hostport));

			clilen=sizeof(serv);
			connfd=accept(listenfd,(struct sockaddr*)&serv,&clilen);

			n=read(connfd,rbuf,MAX_BUF-1); //ack
			rbuf[n]='\0';
			write(STDOUT_FILENO,rbuf,n);

			write(connfd,"NLST",5); //command
			n=read(connfd,rbuf,MAX_BUF-1); //result
			rbuf[n]='\0';
			write(STDOUT_FILENO,rbuf,n);
			close(connfd);
			close(listenfd);
		}
///////////////////////////// End of Control connection ///////////////////////
		else{
			write(STDERR_FILENO,"Command error\n",14);
		}
///////////////////////////// End of communicate //////////////////////////////
	}
	close(sockfd);
}

///////////////////////////////////////////////////////////////////////////////
// convert_addr_to_str
// ==========================================================================
// Input :	unsigned long - IP address
//			unsigned int - Port number
// output : char - translated address
// Purpose : Translate IP address for server
///////////////////////////////////////////////////////////////////////////////

char* convert_addr_to_str(unsigned long ip_addr, unsigned int port){
	char *addr;
	int i;
	addr=(char*)malloc(sizeof(char)*BUF_SIZE); //memory allocate
	memset(addr,0,sizeof(addr));

	strcpy(addr,"PORT "); //PORT command

	unsigned char* temp=(unsigned char*)&ip_addr;
	for(i=0;i<8;i++){ //IP address
		if(i%2==0) strcat(addr,itoa(temp[i/2]));
		else strcat(addr,",");
	} //end of for

	temp=(unsigned char*)&port; //divide port number
	strcat(addr,itoa(temp[0]));
	strcat(addr,",");
	strcat(addr,itoa(temp[1]));

	return addr;
}

///////////////////////////////////////////////////////////////////////////////
// itoa
// ==========================================================================
// Input :	int - number for translate
// output : char - translated array
// Purpose : Integer to array
///////////////////////////////////////////////////////////////////////////////

char* itoa(int num){
	char *buf; //return buffer
	buf=(char *)malloc(sizeof(char));
	memset(buf,0,sizeof(char));

	int tmp=num; //variable for calculate positional number
	int l=0; //positional number of num

	while(tmp>=10){ //calculate positional number
		tmp/=10;
		l++;
	}
	for(l;l>=0;l--){ //change type and copy fo buf
		buf[l]=(num%10)+48;
		num/=10;
	} //end of for

	return buf; //return translated string
}
