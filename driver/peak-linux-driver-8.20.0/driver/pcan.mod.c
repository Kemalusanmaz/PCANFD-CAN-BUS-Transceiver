#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xc1514a3b, "free_irq" },
	{ 0x3c913dfb, "usb_alloc_urb" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xceadc1c3, "usb_free_urb" },
	{ 0xd43859f2, "param_ops_uint" },
	{ 0x64de735d, "param_ops_ulong" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xd494bc92, "pci_enable_device" },
	{ 0x7f02188f, "__msecs_to_jiffies" },
	{ 0x5e515be6, "ktime_get_ts64" },
	{ 0xd6a0cccd, "usb_get_current_frame_number" },
	{ 0x3f1584d8, "proc_create" },
	{ 0xc0f4ce8f, "param_ops_ushort" },
	{ 0x55127462, "pci_iomap" },
	{ 0xb0e602eb, "memmove" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xf5ce77e6, "i2c_bit_add_bus" },
	{ 0x4788f068, "sysfs_add_file_to_group" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xaca7784b, "usb_register_driver" },
	{ 0x7e2d339e, "__pci_register_driver" },
	{ 0xedc03953, "iounmap" },
	{ 0xc942eefb, "param_array_ops" },
	{ 0x308dbbb7, "pci_request_regions" },
	{ 0x69acdf38, "memcpy" },
	{ 0x37a0cba, "kfree" },
	{ 0x2531a760, "seq_lseek" },
	{ 0xc3055d20, "usleep_range_state" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0xf6ebc03b, "net_ratelimit" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0x55ec7f62, "pci_irq_vector" },
	{ 0x14894049, "param_ops_byte" },
	{ 0x34db050b, "_raw_spin_lock_irqsave" },
	{ 0x35789eee, "kmem_cache_alloc_trace" },
	{ 0xcc5005fe, "msleep_interruptible" },
	{ 0x86bb6b5a, "pci_unregister_driver" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x1035c7c2, "__release_region" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x92997ed8, "_printk" },
	{ 0x4cd65682, "usb_clear_halt" },
	{ 0x8ddd8aad, "schedule_timeout" },
	{ 0x1000e51, "schedule" },
	{ 0x92271252, "usb_bulk_msg" },
	{ 0x4725f3fa, "usb_reset_device" },
	{ 0xd0da656b, "__stack_chk_fail" },
	{ 0xb2fcb56d, "queue_delayed_work_on" },
	{ 0x6383b27c, "__x86_indirect_thunk_rdx" },
	{ 0x2de8eb5f, "usb_submit_urb" },
	{ 0xd6263894, "_dev_info" },
	{ 0x5437d888, "i2c_del_adapter" },
	{ 0x167c5967, "print_hex_dump" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0xe4e71ba2, "_dev_err" },
	{ 0x92212d22, "__class_register" },
	{ 0x92d5838e, "request_threaded_irq" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0xdc6a670b, "dma_alloc_attrs" },
	{ 0x670ecece, "__x86_indirect_thunk_rbx" },
	{ 0xa01f2664, "pci_read_config_word" },
	{ 0x8b340b22, "usb_control_msg" },
	{ 0x9166fada, "strncpy" },
	{ 0x9046bd32, "usb_set_interface" },
	{ 0x1edb69d6, "ktime_get_raw_ts64" },
	{ 0xde80cd09, "ioremap" },
	{ 0x7377b2e, "class_unregister" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0x449ad0a7, "memcmp" },
	{ 0x33e5be20, "sysfs_remove_file_from_group" },
	{ 0xfef216eb, "_raw_spin_trylock" },
	{ 0xcefb0c9f, "__mutex_init" },
	{ 0x6d7d5d36, "usb_deregister" },
	{ 0x37befc70, "jiffies_to_msecs" },
	{ 0x9a994cf7, "current_task" },
	{ 0xd35cce70, "_raw_spin_unlock_irqrestore" },
	{ 0x7a08e219, "pci_iounmap" },
	{ 0x1e1e140e, "ns_to_timespec64" },
	{ 0xdbdf6c92, "ioport_resource" },
	{ 0xfb578fc5, "memset" },
	{ 0x7c5e2542, "_dev_warn" },
	{ 0x33ad43af, "pci_alloc_irq_vectors_affinity" },
	{ 0x97934ecf, "del_timer_sync" },
	{ 0x7674696d, "pci_set_master" },
	{ 0xb4b19daa, "param_ops_charp" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xb7704b1c, "dma_get_required_mask" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0xe2d5255a, "strcmp" },
	{ 0x15ba50a6, "jiffies" },
	{ 0xb093ae0b, "dma_set_coherent_mask" },
	{ 0x8798f688, "pv_ops" },
	{ 0xd51950b1, "seq_read" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x10515209, "device_create_with_groups" },
	{ 0xa648e561, "__ubsan_handle_shift_out_of_bounds" },
	{ 0x3ab8a07e, "dma_free_attrs" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0x9fa7184a, "cancel_delayed_work_sync" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x36ca193e, "pci_release_regions" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xbba4fe2a, "driver_for_each_device" },
	{ 0x66cca4f9, "__x86_indirect_thunk_rcx" },
	{ 0xc1352057, "__register_chrdev" },
	{ 0xeabef977, "device_destroy" },
	{ 0x5d1d6f6f, "remove_proc_entry" },
	{ 0x813c6f7b, "usb_kill_urb" },
	{ 0xb43f9365, "ktime_get" },
	{ 0xe6c49e03, "seq_printf" },
	{ 0xffeedf6a, "delayed_work_timer_fn" },
	{ 0x20000329, "simple_strtoul" },
	{ 0x405c9ca9, "i2c_transfer" },
	{ 0x350790, "pci_disable_device" },
	{ 0xa4a2b286, "usb_reset_endpoint" },
	{ 0x1d509809, "single_release" },
	{ 0x9f14b916, "dma_set_mask" },
	{ 0x77358855, "iomem_resource" },
	{ 0xa6893d39, "single_open" },
	{ 0x1793ad5b, "pci_write_config_word" },
	{ 0x48302eaa, "pci_free_irq_vectors" },
	{ 0xc4f0da12, "ktime_get_with_offset" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0xf301d0c, "kmalloc_caches" },
	{ 0x85bd1608, "__request_region" },
	{ 0x2d3385d3, "system_wq" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "i2c-algo-bit");

MODULE_ALIAS("pci:v0000001Cd00000001sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000003sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000004sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000005sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000006sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000007sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000008sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000009sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000002sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd0000000Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000010sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000013sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000016sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000017sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000018sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd00000019sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000001Cd0000001Asv*sd*bc*sc*i*");
MODULE_ALIAS("usb:v0C72p000Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p000Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0012d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0011d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0013d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0C72p0014d*dc*dsc*dp*ic*isc*ip*in*");

MODULE_INFO(srcversion, "4D8A17286A9F60256682269");
