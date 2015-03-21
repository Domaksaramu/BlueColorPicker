//////////////////////////////////////////////////////////////////
//  File Name	 	: srv.c					//
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
#include <pwd.h>


#define MAX_BUF 20
int checkWild(char* str);
int checkIP(char* cli_ip, FILE* fp);
int log_auth(int connfd);
int user_match(char *user, char *passwd);
int main(int argc, char *argv[]) {
	int listenfd, connfd, clilen;
	struct sockaddr_in servaddr, cliaddr;

	FILE *fp_checkIP;
	char* cli_ip;
	int i = 0;
	fp_checkIP = fopen("access.txt","r");	// open access.txt for checking IP


	listenfd = socket(AF_INET, SOCK_STREAM,0);//creat socket
	memset(&servaddr,0,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(atoi(argv[1]));

	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))== -1){//bind socket with IP and port number
		perror("bind() error\n");
		exit(0);
	}

	listen(listenfd, 5);	// listen
	for(;;) {
		fseek(fp_checkIP,0,SEEK_SET);
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);//accept client connection
		if(connfd==-1){
			perror("accept() error\n");
			continue;
		}
		else{
			cli_ip = inet_ntoa(cliaddr.sin_addr);
			printf("** Client is trying to connect **\n");
			printf(" - IP : %s\n",cli_ip);
			printf(" - port : %d\n",ntohs(cliaddr.sin_port));
		}
////////////////////////	check IP	////////////////////////////

		if(!checkIP(cli_ip,fp_checkIP)){
			printf("** It is NOT authenticated client **\n");
			write(connfd,"FAIL",4);
			close(connfd);
			continue;
		}
		else{
			printf("** Client is connected **\n");
			write(connfd,"OK",2);
			
		}
////////////////////////////////////////////////////////////////////////////

////////////////////////	Authentication		//////////////////// 
		if(log_auth(connfd) == 0) {
			printf("** Fail to log-in **\n");
			close(connfd);
			continue;
		}
		else{
			printf("** Success to log-in **\n");
			close(connfd);
		}
////////////////////////////////////////////////////////////////////////////
	}
}
//////////////////////////////////////////////////////////////////
// log_auth							//
//--------------------------------------------------------------//
// input : int connfd 	-> socket discriptor			//
// output : int		-> return 1 or 0			//
// purpose : this is authentication function			//
//////////////////////////////////////////////////////////////////
int log_auth(int connfd)
{
	char user[MAX_BUF], passwd[MAX_BUF], buf[MAX_BUF];
	int n, count=1;
	FILE* fd;
	while(1) {
		n = read(connfd,user,MAX_BUF);// read client ID
		buf[n] = '\0';
		write(connfd, "OK", 2);
		
		n = read(connfd,passwd,MAX_BUF);// read client password
		buf[n] = '\0';
////////////////////	check user ID and password	/////////////////////
		if((n = user_match(user, passwd)) == 1){
			write(connfd,"OK",2);
			return 1;// if ID, password are correct, return 1
		}
		else if(n == 0){
			if(count >= 3) {
				write(connfd,"DISCONNECTION",13);
				break;// if they are not correct in three times, break
			}	
			printf("** User is trying to log-in (%d/3) **\n",count);
			printf("** Log-in failed **\n");
			write(connfd, "FAIL", MAX_BUF);
			count++;
			continue;
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////
// user_match							//
//--------------------------------------------------------------//
// input : char* user, passwd	-> user ID and password		//
// output : int		-> return 1 or 0			//
// purpose : this checks IP and password 			//
//////////////////////////////////////////////////////////////////
int user_match(char *user, char *passwd)
{
	FILE *fp;
	struct passwd *pw;
	fp = fopen("passwd", "r");
	while(pw = fgetpwent(fp)){
		if((!strcmp(user,pw->pw_name))&&(!strcmp(passwd,pw->pw_passwd)))
			return 1;// if ID and password are matched, return 1
	}
	return 0;
}
//////////////////////////////////////////////////////////////////
// checkIP							//
//--------------------------------------------------------------//
// input : char* cli_ipm FILE* fp -> client IP and fill pointer	//
// output : int		-> return 1 or 0			//
// purpose : it checks IP address 				//
//////////////////////////////////////////////////////////////////
int checkIP(char* cli_ip, FILE* fp){
	char f_ip[4][4], c_ip[4][4], buf[MAX_BUF];
	char* temp;
	int i=0,j;
	int cnt = 0;
	temp = strtok(cli_ip,".");
	do{
		strcpy(c_ip[i],temp);//divide client IP into four parts
		i++;
	}while((temp = strtok(NULL,".\0"))!=NULL);
	while(fgets(buf,MAX_BUF,fp)!=NULL){// get IP from passwd file for checking
		i=0;
		
		memset(f_ip,0,sizeof(f_ip));
		temp = strtok(buf,".\n\0");
		do{
			strcpy(f_ip[i],temp);// divide checking IP into four parts
			i++;			
		}while((temp=strtok(NULL,".\n\0"))!=NULL);

		for(i=0;i<4;i++){
			if(checkWild(f_ip[i])){//if a part of IP contains *, count and continue
				cnt++;
				continue;
			}

			if(!strcmp(c_ip[i],f_ip[i])){
				cnt++;	// if a part of IP is matched, count
			}
			else{
				cnt=0;
				break;
			}
		}
	}
		if(cnt==4)
			return 1;
		else
			return 0;

	
}
//////////////////////////////////////////////////////////////////
// checkWild							//
//--------------------------------------------------------------//
// input : char* cli_ipm FILE* fp -> client IP and fill pointer	//
// output : int		-> return 1 or 0			//
// purpose : // check IP address whether there is *(wildcard)	//
// or not							//
//////////////////////////////////////////////////////////////////
int checkWild(char* str){	int j;
	for(j=0;j<strlen(str);j++){
		if(str[j]=='*'){
			return 1; // if there is *, return 1
		}
	}
	return 0;
}


