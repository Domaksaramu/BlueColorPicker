#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x669216a, "module_layout" },
	{ 0x50eedeb8, "printk" },
	{ 0x15568631, "lookup_address" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x2f287f0d, "copy_to_user" },
	{ 0x377f93f4, "mmput" },
	{ 0xe914e41e, "strcpy" },
	{ 0x42bc3c5d, "get_task_mm" },
	{ 0x2f377ae9, "pid_task" },
	{ 0x11e17a6, "find_vpid" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "C0716AFB9FF4A7606B3AB0E");
