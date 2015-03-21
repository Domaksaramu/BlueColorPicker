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
	{ 0x6b281618, "remove_proc_entry" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4f391624, "create_proc_entry" },
	{ 0xacf7d2ed, "proc_mkdir" },
	{ 0x4daf3a08, "init_task" },
	{ 0xbb163497, "__task_pid_nr_ns" },
	{ 0x33584ef0, "task_tgid_nr_ns" },
	{ 0x91715312, "sprintf" },
	{ 0xd9fb7bd8, "task_active_pid_ns" },
	{ 0x42bc3c5d, "get_task_mm" },
	{ 0x2f377ae9, "pid_task" },
	{ 0x11e17a6, "find_vpid" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "445FFCCFC24500917C1CAC3");
