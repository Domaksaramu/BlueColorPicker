#include <linux/unistd.h>
#include <stdio.h>
#include <assert.h>

struct vm_info
{
	unsigned long startcode;
	unsigned long endcode;
	unsigned long startdata;
	unsigned long enddata;
	unsigned long startbrk;
	unsigned long brk;
	unsigned long startstack;
};

int main(int argc, const char *argv[])
{
	int pid;
	struct vm_info info;
	pid = getpid();
	syscall(__NR_add, pid, &info);
	printf("Code : %lu ~ %lu\n", info.startcode, info.endcode);
	printf("Data : %lu ~ %lu\n", info.startdata, info.enddata);
	printf("Heap : %lu ~ %lu\n", info.startbrk, info.brk);
	printf("Stack : %lu ~ \n", info.startstack);
	
	return 0;
}
