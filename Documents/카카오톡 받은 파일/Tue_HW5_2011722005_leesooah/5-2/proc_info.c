#include <linux/module.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/kernel.h>
#include <linux/kernel_stat.h>
#include <linux/tty.h>
#include <linux/string.h>
#include <linux/mman.h>
#include <linux/proc_fs.h>
#include <linux/ioport.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/pagemap.h>
#include <linux/swap.h>
#include <linux/smp.h>
#include <linux/signal.h>
#include <linux/highmem.h>
#include <linux/file.h>
#include <linux/fdtable.h>
#include <linux/times.h>
#include <linux/cpuset.h>
#include <linux/rcupdate.h>
#include <linux/delayacct.h>
#include <linux/seq_file.h>
#include <linux/pid_namespace.h>
#include <linux/ptrace.h>
#include <linux/tracehook.h>
#include <linux/hugetlb.h>
#include <linux/huge_mm.h>
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/highmem.h>
#include <linux/ptrace.h>
#include <linux/slab.h>
#include <linux/pagemap.h>
#include <linux/mempolicy.h>
#include <linux/rmap.h>
#include <linux/swap.h>
#include <linux/swapops.h>
#include <linux/mman.h>
 
#include <asm/pgtable.h>
#include <asm/processor.h>
#include <asm/elf.h>
#include <asm/uaccess.h>
#include <asm/tlbflush.h>

struct proc_dir_entry *dir_fp;
struct proc_dir_entry *pid_fp;
struct proc_dir_entry *fd;

int *data_pid;

/////// task_state_array //////////////////
static const char * const task_state_array[] = {
         "R (running)",          /*   0 */
         "S (sleeping)",         /*   1 */
         "D (disk sleep)",       /*   2 */
        "T (stopped)",          /*   4 */
         "t (tracing stop)",     /*   8 */
         "Z (zombie)",           /*  16 */
         "X (dead)",             /*  32 */
         "x (dead)",             /*  64 */
         "K (wakekill)",         /* 128 */
        "W (waking)",           /* 256 */
};

//////////// get_task_ state function ////////////////
static inline const char *get_task_state(struct task_struct *tsk)
{
        unsigned int state = (tsk->state & TASK_REPORT) | tsk->exit_state;
        const char * const *p = &task_state_array[0];
        BUILD_BUG_ON(1 + ilog2(TASK_STATE_MAX) != ARRAY_SIZE(task_state_array));

       while (state) {
                 p++;
                state >>= 1;
      }
         return *p;
}

///////////// get_task_cred function ////////////////////
const struct cred *get_task_cred(struct task_struct *task)
{
        const struct cred *cred;

         rcu_read_lock();

       do {
                cred = __task_cred((task));
                BUG_ON(!cred);
        } while (!atomic_inc_not_zero(&((struct cred *)cred)->usage));

        rcu_read_unlock();
         return cred;
}

//////////// get_mm_counter function //////////////////
unsigned long get_mm_counter(struct mm_struct *mm, int member)
{
         long val = 0;
 
         /*
          * Don't use task->mm here...for avoiding to use task_get_mm()..
          * The caller must guarantee task->mm is not invalid.
          */
         val = atomic_long_read(&mm->rss_stat.count[member]);
         /*
         * counter is updated in asynchronous manner and may go to minus.
          * But it's never be expected number for users.
          */
         if (val < 0)
                 return 0;
        return (unsigned long)val;
}

 
int read_infor(char * page, char ** start, off_t off, int count, int *eof, void * data)
{
	int len;
	unsigned long infor, text, lib, swap;
	unsigned long hiwater_vm, total_vm, hiwater_rss, total_rss;
	struct task_struct *p;
	struct mm_struct *mm;
        const struct cred *cred;
        pid_t ppid, tpid;
	struct pid_namespace *ns;
        int i;

	p = pid_task(find_vpid((int)data),PIDTYPE_PID); // get task_struct information
	mm = get_task_mm(p); // get mm_struct information
 	ns = task_active_pid_ns(p); // get pid_namespace information

	len = sprintf(page, "Name: %s\n", p->comm);
       rcu_read_lock();
       ppid = pid_alive(p)?task_tgid_nr_ns(rcu_dereference(p->real_parent), ns):0;
         tpid = 0;
         if (pid_alive(p)) {
                 struct task_struct *tracer = ptrace_parent(p);
                 if (tracer)
                         tpid = task_pid_nr_ns(tracer, ns);
         }
         cred = get_task_cred(p);
         len += sprintf(page+len, "State:\t%s\nTgid:\t%d\nPid:\t%d\nPPid:\t%d\nTracerPid:\t%d\nUid:\t%d\t%d\t%d\t%d\nGid:\t%d\t%d\t%d\t%d\n", get_task_state(p), task_tgid_nr_ns(p, ns), (int)data, ppid, tpid, cred->uid, cred->euid, cred->suid, cred->fsuid, cred->gid, cred->egid, cred->sgid, cred->fsgid);

         rcu_read_unlock();
 
	if(mm)
	{
       	 	hiwater_vm = total_vm = mm->total_vm;
        	if (hiwater_vm < mm->hiwater_vm)
                 	hiwater_vm = mm->hiwater_vm;
        	hiwater_rss = total_rss = get_mm_rss(mm);
        	if (hiwater_rss < mm->hiwater_rss)
                 	hiwater_rss = mm->hiwater_rss;
 
         	infor = mm->total_vm - mm->shared_vm - mm->stack_vm;
         	text = (PAGE_ALIGN(mm->end_code) - (mm->start_code & PAGE_MASK)) >> 10;
         	lib = (mm->exec_vm << (PAGE_SHIFT-10)) - text;
         	swap = get_mm_counter(mm, MM_SWAPENTS);
         	len += sprintf(page+len, "VmPeak:\t%8lu kB\nVmSize:\t%8lu kB\nVmLck:\t%8lu kB\nVmPin:\t%8lu kB\nVmHWM:\t%8lu kB\nVmRSS:\t%8lu kB\nVmData:\t%8lu kB\nVmStk:\t%8lu kB\nVmExe:\t%8lu kB\nVmLib:\t%8lu kB\nVmPTE:\t%8lu kB\nVmSwap:\t%8lu kB\n", 
		hiwater_vm << (PAGE_SHIFT-10), 
		(total_vm - mm->reserved_vm) << (PAGE_SHIFT-10), 
		mm->locked_vm << (PAGE_SHIFT-10), 
		mm->pinned_vm << (PAGE_SHIFT-10), 
		hiwater_rss << (PAGE_SHIFT-10), 
		total_rss << (PAGE_SHIFT-10), 
		infor << (PAGE_SHIFT-10), 
		mm->stack_vm << (PAGE_SHIFT-10), text, 
		lib,(PTRS_PER_PTE*sizeof(pte_t)*mm->nr_ptes) >> 10, swap << (PAGE_SHIFT-10));

	}

	return len;
}

int write_infor(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char *kdata;
	kdata = (char *)data;
	copy_from_user(kdata, buffer, count); // copy to kernel from user
	kdata[count] = '\n';
	return count;
}

int init_proc(void)
{
	char buf[20] = {0,};
	struct task_struct * findtask = &init_task; // init task

	dir_fp = proc_mkdir("m2011722005",NULL);
	do
	{
		sprintf(buf,"m%d",findtask->pid);
		pid_fp = proc_mkdir(buf,dir_fp); // make directory
		fd = create_proc_entry("INFO",0666,pid_fp); // make file
		data_pid = findtask->pid; 
		if(fd)
		{
			fd->data = data_pid;
			fd->read_proc = read_infor;
			fd->write_proc = write_infor;
		}
		findtask = next_task(findtask); // go to next task
	}while((findtask->pid != init_task.pid));

	return 0;
		
}

void exit_proc(void)
{
	remove_proc_entry("m2011722005",NULL);
}

module_init(init_proc);
module_exit(exit_proc);
MODULE_LICENSE("GPL");
