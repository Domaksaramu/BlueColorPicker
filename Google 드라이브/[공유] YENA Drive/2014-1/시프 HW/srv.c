#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>

#define BUF_SIZE 128
#define MAX_BUF 8192

char* itoa(int num);

int main(int argc, char **argv){
	char buf[MAX_BUF]={0,}; //receive buf
	char rbuf[MAX_BUF]={0,}; //response buf
	char gbuf[BUF_SIZE]={0,}; //get string from file
	char tbuf_n[BUF_SIZE]={0,}; //current time buf
	char tbuf_f[BUF_SIZE]={0,}; //last modified time buf
	char *cmd, *get, *tar;
	int n, listenfd, connfd;
	time_t now;
	struct sockaddr_in cliaddr, servaddr;
	struct tm *tmp_n, *tmp_f;
	struct stat sp;
	socklen_t clilen;
	FILE *html;
	
	if(argc<2){
		write(STDERR_FILENO,"input port number\n",18);
		return 0;
	}

	listenfd=socket(AF_INET,SOCK_STREAM,0);

	memset(&servaddr,0,sizeof(servaddr));

	servaddr.sin_family=AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(atoi(argv[1]));

	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1){
		write(STDERR_FILENO,"bind() error\n",13);
		return 0;
	}
	if(listen(listenfd,5)==-1){
		write(STDERR_FILENO,"listen() error\n",15);
		return 0;
	}
	clilen=sizeof(cliaddr);

	while(1){
		connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
		memset(&buf,0,sizeof(buf));
		memset(&rbuf,0,sizeof(rbuf));
		memset(&tbuf_n,0,sizeof(tbuf_n));
		memset(&tbuf_f,0,sizeof(tbuf_f));

		n=read(connfd,buf,MAX_BUF-1);
		buf[n]='\0';
		write(STDOUT_FILENO,buf,n);

		cmd=strtok(buf,"\n"); //Command line
		get=strtok(cmd," "); //GET
		tar=strtok(NULL,"/ "); //target

		if(lstat(tar,&sp)==0){
			html=fopen(tar, "r");

			time(&now);
			tmp_n=gmtime(&now); //current time
			tmp_f=gmtime(&(sp.st_mtime)); //last modified time
			strftime(tbuf_n,sizeof(tbuf_n),"%a, %d-%b-%y %X %Z\n",tmp_n);
			strftime(tbuf_f,sizeof(tbuf_f),"%a, %d-%b-%y %X %Z\n",tmp_f);

			strcpy(rbuf,"HTTP/1.1 200 OK\nDate: ");
			strcat(rbuf,tbuf_n);
			strcat(rbuf,"Server: Apache/1.3.19 (Unix) PHP/4.0.6\nMIME-version: 1.0\nContent-type: text/html\nLast-modified: ");
			strcat(rbuf,tbuf_f);
			strcat(rbuf,"Content-length: ");
			strcat(rbuf,itoa((int)sp.st_size));
			strcat(rbuf,"\n\n");

			write(connfd,rbuf,strlen(rbuf));
			memset(&rbuf,0,sizeof(rbuf));

			while(fgets(gbuf,BUF_SIZE,html)!=NULL){
				strcat(rbuf,gbuf);
			}
			write(connfd,rbuf,strlen(rbuf));
		}
		else{
			strcpy(rbuf,"HTTP/1.1 404 Not Found\n\n");
			write(connfd,rbuf,strlen(rbuf));
		}
		close(connfd);
	}
}

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
