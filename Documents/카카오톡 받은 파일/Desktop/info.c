#include <linux/module.h>
#include <linux/highmem.h>
#include <asm/unistd.h>
#include <linux/sched.h>

#define __NR_add 349
void **syscall_table = (void**)SYSCALL_TABLE;

asmlinkage int (*real_add)(int, int);

struct vm_info
{
	//char name[10];
	unsigned long startcode;
	unsigned long endcode;
	unsigned long startdata;
	unsigned long enddata;
	unsigned long startbrk;
	unsigned long brk;
	unsigned long startstack;
};

asmlinkage int get_info(int pid, struct vm_info *info)
{
	struct task_struct *t;
	struct mm_struct *mm;
	struct vm_info tmp_info;

	t = pid_task(find_vpid(pid), PIDTYPE_PID);
	mm = get_task_mm(t);

	//strcpy(tmp_info.name, t->comm);
	tmp_info.startcode = mm->start_code;
	tmp_info.endcode = mm->end_code;
	tmp_info.startdata = mm->start_data;
	tmp_info.enddata = mm->start_data;
	tmp_info.startbrk = mm->start_brk;
	tmp_info.brk = mm->brk;
	tmp_info.startstack = mm->start_stack;

	mmput(mm);

	copy_to_user(info, &tmp_info, sizeof(struct vm_info));
	return 0;
}

void make_rw(void *address)
{
	unsigned int level;
	pte_t * pte = lookup_address((unsigned long)address, &level);
	if(pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
}

void make_ro(void *address)
{
	unsigned int level;
	pte_t *pte = lookup_address((unsigned long)address, &level);
	pte->pte = pte->pte &~ _PAGE_RW;
}
 
int hooking_init(void)
{
	make_rw(syscall_table);
	real_add = syscall_table[__NR_add];
	syscall_table[__NR_add] = get_info;
	return 0;
}

void hooking_exit(void)
{

	syscall_table[__NR_add] = real_add;
	make_ro(syscall_table);
}

module_init(hooking_init);
module_exit(hooking_exit);
MODULE_LICENSE("GPL");

