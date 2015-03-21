#define __KERNEL__ 
#define MODULE 
#define _LINUX 

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/module.h> 
#include <linux/if_ether.h> 
#include <linux/udp.h> 
#include <linux/netfilter_ipv4.h> 
#include <asm/byteorder.h> 
#include <linux/skbuff.h> 
#include <net/tcp.h> 
#include <linux/time.h> 
#include <linux/ctype.h> 
#include <linux/string.h> 

#define M_AUTHOR "MCLAB" 
#define M_DESCRIPTION "used for cnlab"

////////////// GLOBAL VARIABLES 
unsigned short i = 0; 
struct sk_buff 
*sb; struct iphdr 
*iph = 0; 
struct tcphdr *tcph = 0; 
struct ethhdr *ethn = 0; 

/////////////////////// STRUCTURES 

struct nf_hook_ops hook_ops_mclab;

void mclab_prtaddr(struct sk_buff *sb);

unsigned int mclab(unsigned int hooknum, 
		  struct sk_buff *skb,
		  const struct net_device *in,
		  const struct net_device *out,
		  int (*okfn)(struct sk_buff*))
{
	struct sk_buff *sb = skb;
	iph = (struct iphdr*)skb_network_header(sb);
	
	switch(iph->protocol){
		case 6://TCP packet address
		{	printk(KERN_ALERT "[pre routing]");
			mclab_prtaddr(sb);
			break;}
		default:
			break;
	}
	
	return NF_ACCEPT; //Accept
}
void mclab_prtaddr(struct sk_buff *sb)
{
	ethn = (struct ethhdr*)skb_mac_header(sb);
	
	//SOURCE ,DESTINATION MAC address print
	printk("[FORM}");	
	printk("[%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:]",
		ethn->h_source[0],ethn->h_source[1],ethn->h_source[2],
		ethn->h_source[3],ethn->h_source[4],ethn->h_source[5]);


	printk("[TO}");	
	printk("[%.2x:%.2x:%.2x:%.2x:%.2x:%.2x:]",
		ethn->h_dest[0],ethn->h_dest[1],ethn->h_dest[2],
		ethn->h_dest[3],ethn->h_dest[4],ethn->h_dest[5]);
}

//kernel module loading function
int init_modules(void)
{
	int retval = 0;
	//hooking : PRE_ROUTING
	hook_ops_mclab.hook	= mclab;
	hook_ops_mclab.hooknum	= NF_INET_PRE_ROUTING;
	hook_ops_mclab.pf	= PF_INET;
	hook_ops_mclab.priority	= NF_IP_PRI_FIRST;

	retval = nf_register_hook(&hook_ops_mclab);
	printk(KERN_ALERT "hook_ops_mclab %d\n",retval);

	printk(KERN_ALERT "[MCLAB] LOADED -------------------\n");

	return 0;
}
//kernel module unloading
//return resources were used module
void cleanup_modules(void)
{
	//unregister hooking operations
	nf_unregister_hook(&hook_ops_mclab);
	printk(KERN_ALERT "[MCLAB] -------------------UNLOADED\n");
}

module_init(init_modules);
module_exit(cleanup_modules);

MODULE_LICENSE("GPL");


