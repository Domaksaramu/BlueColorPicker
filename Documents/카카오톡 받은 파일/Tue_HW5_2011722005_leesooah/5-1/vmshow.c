#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/unistd.h>
#include <linux/sched.h>

#define __NR_add 349

void **syscall_table = (void**)SYSCALL_TABLE;
asmlinkage int (*real_add)(int, int);

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

asmlinkage int get_info(int pid, struct vm_info *info)
{	
	struct task_struct *t;
	struct mm_struct *mm;
	struct vm_info tmp_info;

	t= pid_task(find_vpid(pid), PIDTYPE_PID); // get task_struct information
	mm = get_task_mm(t); // get mm_struct information

	strcpy(tmp_info.name, t->comm);
	// code
	tmp_info.startcode = mm->start_code;
	tmp_info.endcode = mm->end_code;
	// data
	tmp_info.startdata = mm->start_data;
	tmp_info.enddata = mm->end_data;
	// heap
	tmp_info.startheap = mm->start_brk;
	tmp_info.endheap = mm->brk;
	// stack
	tmp_info.startstack = mm->start_stack;

	mmput(mm);

	copy_to_user(info, &tmp_info, sizeof(struct vm_info)); // copy to user from kernel
	return 0;

}

void make_rw(void * address) // function that allows user to modify the file 
{
	unsigned int level;
	pte_t *pte = lookup_address((unsigned long)address, &level);
	if(pte->pte &~ _PAGE_RW)
	{
		pte->pte |= _PAGE_RW;
	}
}

void make_ro(void * address) // function to prevent file modification
{
	unsigned int level;
	pte_t *pte = lookup_address((unsigned long)address, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}

int module_start(void)
{
	make_rw(syscall_table);
	printk("\n\n\n\n2011722005, Lee Soo-Ah\n");
	printk("hooking : sys_open() is wrapped 0x%p\n",syscall_table);
	real_add = syscall_table[__NR_add]; // save sys_open system call to pointer
	syscall_table[__NR_add] = get_info; // connect my system call function to open() 
	return 0;
	
}

void module_end(void)
{
	syscall_table[__NR_add] = real_add; // connect original sys_open() function to open() 
	make_ro(syscall_table);
	printk("hooking : sys_open() is unwrapped\n");
}

module_init(module_start);
module_exit(module_end);
MODULE_LICENSE("GPL");
