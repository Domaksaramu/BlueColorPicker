#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, const char *argv[]){
	int input, output, pagesize, readsize;
	size_t filesize;
	char* buf;
	struct timespec begin, end;
	clock_gettime(0, &begin);

	if(argc != 3){
	 	printf("Arguments are too little or too much\n");
		exit(1);
	}

	if((input = open(argv[1], O_RDONLY,0666)) == -1){
		printf("%s open error\n",argv[1]);
		exit(1);
	}

	if((output = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, 0666)) == -1){
		printf("%s open error\n",argv[1]);
		exit(1);
	}


	filesize = lseek(input, 0, SEEK_END);
	if(filesize<=0){
		printf("%s hasn't any contents\n",argv[1]);
		exit(1);
	}
	pagesize = getpagesize();
	lseek(input, 0, SEEK_SET);
	buf = (char*)malloc(sizeof(char)*pagesize);
	
	while(readsize=read(input,buf,pagesize)){
		write(output,buf,readsize);
	}

	close(input);
	close(output);

	free(buf);
	
	clock_gettime(0, &end);
	printf("total time : %.8f\n",(float)(end.tv_sec - begin.tv_sec) + ((float)(end.tv_nsec - begin.tv_nsec)/1000000000));
	return 0;
}

