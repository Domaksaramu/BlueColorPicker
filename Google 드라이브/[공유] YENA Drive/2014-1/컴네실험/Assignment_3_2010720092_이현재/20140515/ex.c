#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/module.h>

#include<linux/if_ether.h>
#include<linux/udp.h>
#include<linux/netfilter_ipv4.h>
#include<asm/byteorder.h>
#include<linux/skbuff.h>
#include<net/tcp.h>
#include<linux/time.h>
#include<linux/ctype.h>
#include<linux/string.h>

#define FTP_PORT 21

/////////////////GLOBAL variables
unsigned short i = 0;
struct sk_buff *sb;
struct iphdr *iph = 0;
struct tcphdr *tcph = 0;
struct ethhdr *ethn = 0;

char temp[2] = {0};
//IP address
unsigned char ip_addr[4] = { 0,0,0,0 };
//accpet IP address
unsigned int accept_ip[3][4] = {127,0,0,1,128,134,54,256,128,134,54,242};

/////////////////STRUCTURES
struct nf_hook_ops hook_ops_mclab;

unsigned short mclab_ntohs(unsigned short portnum){
	// translate port number
	temp[0] = *((char*)(&portnum)+1);
	temp[1] = *((char*)(&portnum));
	return *(unsigned short *)temp;
}

int do_ftp(struct sk_buff *sb){
	int i;
	int j;
	int match;

	iph = (struct iphdr *)skb_network_header(sb);
	tcph = (struct tcphdr *)skb_transport_header(sb);

	//IP address
	unsigned char *temp = (unsigned char*)&(iph->saddr);
	ip_addr[0] = temp[0];
	ip_addr[1] = temp[1];
	ip_addr[2] = temp[2];
	ip_addr[3] = temp[3];

	for(i=0;i<3;i++){ //compare IP address
		match=0;
		for(j=0;j<4;j++){
			if(accept_ip[i][j]!=ip_addr[j]) break;
			match++;
		}
		if(match==4){ //accept IP
			printk(KERN_ALERT "[IN] FTP access acceptd : from %d.%d.%d.%d\n",temp[0],temp[1],temp[2],temp[3]);
			return NF_ACCEPT; //Accept
		}
	}
	
	printk(KERN_ALERT "[IN] FTP access denied : from %d.%d.%d.%d\n",ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);
	return NF_DROP; //Drop
}

int do_icmp(struct sk_buff *sb){
	iph = (struct iphdr *)skb_network_header(sb);
	tcph = (struct tcphdr *)skb_transport_header(sb);

	unsigned char *temp = (unsigned char*)&(iph->saddr);
	ip_addr[0] = temp[0];
	ip_addr[1] = temp[1];
	ip_addr[2] = temp[2];
	ip_addr[3] = temp[3];

	printk(KERN_ALERT "[IN] ICMP message denied : from %d.%d.%d.%d\n",ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3]);

	return NF_DROP;
}

unsigned int mclab(unsigned int hooknum,
		struct sk_buff *skb,
		const struct net_device *in,
		const struct net_device *out,
		int (*okfn)(struct sk_buff *))
{
	struct sk_buff *sb = skb;
	iph = (struct iphdr*)skb_network_header(skb);
	tcph = (struct tcphr*)skb_transport_header(skb);

	unsigned short hport;

	switch(iph->protocol) {
		case 1: //ICMP
			return do_icmp(sb);
			break;
		case 6: //Print address when TCP packet
			hport = mclab_ntohs(tcph->source);
			if(hport==FTP_PORT){
				return do_ftp(sb);
			}
			break;
		default:
			break;
	}

	return NF_ACCEPT;
}

//First calling function when kernel module loaded
int init_modules(void){
	int retval = 0;
	//hooking : PRE_ROUTING
	hook_ops_mclab.hook = mclab;
	hook_ops_mclab.hooknum = NF_INET_PRE_ROUTING;
	hook_ops_mclab.pf = PF_INET;
	hook_ops_mclab.priority = NF_IP_PRI_FIRST;

	retval = nf_register_hook(&hook_ops_mclab);
	printk(KERN_ALERT "hook_ops_mclab %d\n", retval);

	printk(KERN_ALERT "[MCLAB] LOADED -----------------\n");

	return 0;
}

//Calling when kernel module unloading
//Return source
void cleanup_modules(void){
	//unregieter hooking operations
	nf_unregister_hook(&hook_ops_mclab);
	printk(KERN_ALERT "[MCLAB] --------------- UNLOADED\n");
}

module_init(init_modules);
module_exit(cleanup_modules);

MODULE_LICENSE("GPL");
