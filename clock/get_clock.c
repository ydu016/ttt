#include <linux/module.h>
#include <linux/cpufreq.h>
#include <linux/slab.h>
#include "get_clock.h"

MODULE_LICENSE("Dual BSD/GPL");

struct cpuinfo_x86 *c;

#if !defined CONFIG_S390 || !defined CONFIG_ARM64
static unsigned int kernel_get_cpu_khz(void)
{
        unsigned int khz = 0;
        unsigned int k, delta;
        int print_flag = 0;

        while(1) {
            k = cpufreq_quick_get(c->cpu_index);

            if(khz == 0) {
                khz = k;
                continue;
            }

            delta = khz > k ? khz - k : k - khz;
            if((delta / khz > 20) && (print_flag == 0)) {
                print_flag = 1;
                continue;
            }

            break;
        }

        return khz;
}
#endif

static unsigned int get_cpu_khz(void)
{
	#if defined CONFIG_S390
		printk(KERN_WARNING "Unsupported architecture: S390\n");
	#elif defined CONFIG_ARM64
		printk(KERN_WARNING "Unsupported architercture: ARM64"); 
	#endif	
	return kernel_get_cpu_khz();
}
EXPORT_SYMBOL(get_cpu_khz);

static void test(void)
{
	cycles_t cyc0, cyc1;
	unsigned int khz = get_cpu_khz();
	cyc0 = kernel_get_cycles();
	cyc1 = kernel_get_cycles();
	printk("cycle diff %llu\n", cyc1 - cyc0);
	printk("cycle lat %llu\n", ((cyc1 - cyc0) *1000)/(khz/1000));
}

static void test_kmalloc(int i)
{
        cycles_t cyc0, cyc1, diff = 0;
        unsigned int khz;
        void *ptr;
	int j;
	
	for(j = 0; j < i; j++)
	{
		cyc0 = kernel_get_cycles();
		ptr = kmalloc(1500, GFP_KERNEL);

		if(!ptr)
			printk("err\n");

		cyc1 = kernel_get_cycles();
		diff += cyc1 - cyc0;
		kfree(ptr);
	}

        khz = get_cpu_khz();
        printk("cpu khz %u\n", khz);
        printk("cycle diff %llu\n", diff);
        printk("cycle lat %llu\n", (diff *1000)/(khz/1000)/i);
}

static int __init my_init(void)
{
        int pos = 0;
        c = kmalloc(sizeof (*c), GFP_KERNEL);
        pos = cpumask_next(pos - 1, cpu_online_mask);
        if (pos < nr_cpu_ids)
            *c = cpu_data(pos);

	test_kmalloc(100);
        return 0;
}

static void __exit my_exit(void)
{
        kfree(c);
}

module_init(my_init);
module_exit(my_exit);
