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

#define M_AUTHOR "MCLAB"
#define M_DESCRIPTION "used for cnlab"
#define HTTP_PORT 80

////////////// GLOBAL VARIABLES
unsigned short i = 0;
struct sk_buff *sb;
struct iphdr *iph = 0;
struct tcphdr *tcph = 0;
struct ethhdr *ethn = 0;

char temp[2] = { 0 };

// ip address 저장 변수
unsigned char ip_addr[4] = { 0,0,0,0 };

// 접속 차단 IP address와 URL
unsigned long block_ip[3] = { 2586674816ul, 2403763840ul, 118670047ul };
char* block_ip_url[3] = {"mclab.kw.ac.kr", "www.kw.ac.kr", "kupis.kw.ac.kr"};

/////////////////////// STRUCTURES
struct nf_hook_ops hook_ops_mclab;

unsigned short mclab_ntohs(unsigned short portnum){
	// translate port number
	temp[0] = *((char*)(&portnum) + 1);
	temp[1] = *((char*)(&portnum));
	return *(unsigned short *)temp;
}

unsigned int do_http(struct sk_buff *sb){
	int i=0;

	iph = (struct iphdr *)skb_network_header(sb);
	tcph = (struct tcphdr *)skb_transport_header(sb);

	unsigned char *temp = (unsigned char*)&(iph->daddr);
	ip_addr[0] = temp[0];
	ip_addr[1] = temp[1];
	ip_addr[2] = temp[2];
	ip_addr[3] = temp[3];

	for(i=0;i<3;i++){ //compare IP address
		if((iph->daddr==block_ip[i])) { //block IP
			printk(KERN_ALERT "[OUT] WEB connect denied : to [%d.%d.%d.%d][%d] - %s\n",ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3],HTTP_PORT, block_ip_url[i]);
			return NF_DROP; //Drop
		}
	}

	return NF_ACCEPT; //Accept
}

unsigned int mclab(unsigned int hooknum,struct sk_buff *skb,const struct net_device *in,const struct net_device *out,int (*okfn)(struct sk_buff *)){
	struct sk_buff *sb = skb;
	iph = (struct iphdr *)skb_network_header(sb);
	tcph = (struct tcphdr *)skb_transport_header(sb);
	unsigned short hport;

	switch(iph->protocol) {
		case 6: // TCP
		hport = mclab_ntohs(tcph->dest);
		if(hport==HTTP_PORT) //HTTP
			return do_http(sb);
		break;
	default:
		break;
	}
	return NF_ACCEPT;
}

// kernel module이 loading 될 때 최초로 호출되는 function
int init_modules(){
	int retval = 0;

	// hooking : POST_ROUTING
	hook_ops_mclab.hook = mclab;
	hook_ops_mclab.hooknum = NF_INET_POST_ROUTING;
	hook_ops_mclab.pf = PF_INET;
	hook_ops_mclab.priority = NF_IP_PRI_FIRST;

	retval = nf_register_hook(&hook_ops_mclab);
	printk("hook_ops_mclab : http filter %d\n", retval);
	printk("[MCLAB] LOADED -----------------\n");

	return 0;
}

// kernel module이 unloading 될 때 호출됨
// module에서 사용한 자원을 반환
void cleanup_modules(){
	// unregister hooking operations
	nf_unregister_hook(&hook_ops_mclab);
	printk("[MCLAB] --------------- UNLOADED\n");
}

module_init( init_modules );
module_exit( cleanup_modules);
MODULE_LICENSE("GPL");
