#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int **temp;

int LCS(const char* x,const char* y, int i, int j);

int main(int argc, const char *argv[]){
	int i=strlen(argv[1]),j=strlen(argv[2]);
	int lcs_length, k, l;

	temp = (int**)malloc(sizeof(int*)*(i+1));
	for(k=0;k<=i;k++){
		*(temp+k) = (int*)malloc(sizeof(int)*(j+1));
		for(l=0;l<=k;l++){
			temp[k][l] = 0xffffffff;
		}
	}


	lcs_length = LCS(argv[1],argv[2],i,j);
	printf("A: %s\nB: %s\n\n",argv[1],argv[2]);
	printf("LCS Lenght is %d\n\n",lcs_length);

	for(l=0;l<=j;l++){
		for(k=0;k<=i;k++){
			printf("%d	",temp[k][l]);
		}
		printf("\n");
	}

	for(k=0;k<i;k++){
		free(*(temp+k));
	}
	free(temp);
}

int LCS(const char* x, const char* y, int i, int j){
	int a, b;
	if(temp[i][j]<0){
		if(i==0 || j==0){
			temp[i][j] = 0;
		}
		else if(x[i-1]==y[j-1]){
			temp[i][j] = LCS(x,y,i-1,j-1)+1;
		}
		else{
			a=LCS(x,y,i-1,j);
			b=LCS(x,y,i,j-1);
			if(a>=b){
				temp[i][j] = a;
			}
			else{
				temp[i][j] = b;
			}
		}
	}

	return temp[i][j];
}
