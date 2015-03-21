#include <stdio.h>

int temp[8][7] = {0};

int LCS(char* x,char* y, int i, int j);

int main(){
	char x[8] = {"abcbdab\0"};
	char y[7] = {"bdcaba\0"};
	int i=7,j=6;
	int lcs_length;
	lcs_length = LCS(x,y,i,j);
	printf("A: %s\nB: %s\n",x,y);
	printf("LCS Lenght is %d\n",lcs_length);
}

int LCS(char* x, char* y, int i, int j){
	int a, b;
	if(temp[i][j]==0){
		if(i==0 || j==0)
			return temp[i-1][j-1];
		if(x[i-1]==y[j-1]){
			return LCS(x,y,i-1,j-1)+1;
		}
		else{
			a=LCS(x,y,i-1,j);
			b=LCS(x,y,i,j-1);
			if(a>=b)
				return a;
			else
				return b;
		}
	}
}
