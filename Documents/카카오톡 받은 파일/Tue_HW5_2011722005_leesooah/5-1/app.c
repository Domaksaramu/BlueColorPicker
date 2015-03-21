#include <linux/unistd.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

int global; // bss
int global2 = 1; // data

struct vm_info
{
	char name[10];
	unsigned long startcode;
	unsigned long endcode;
	unsigned long startdata;
	unsigned long enddata;
	unsigned long startheap;
	unsigned long endheap;
	unsigned long startstack;
};
int main(int argc, const char *argv[])
{
	struct vm_info info;
	static int stat = 1; // data
	int a; // stack
	char *mem;
	char * text = "hello"; // text`s address = stack, address of text`s string = code

	mem = malloc(5); // heap
	syscall(__NR_add, getpid(), &info);

	printf("name : %s\n", info.name);
	printf("Code : %p (%lx~%lx)\n", text, info.startcode, info.endcode);
	printf("Data : %p, %p (%lx~%lx)\n", &global2, &stat, info.startdata, info.enddata);
	printf("BSS : %p (%lx ~ %lx)\n", &global, info.enddata, info.startheap);
	printf("Heap : %p (%lx~%lx)\n", mem, info.startheap, info.endheap);
	printf("Stack : %p, %p (%lx~)\n", &text, &a, info.startstack);

	free(mem);
	return 0;
}
