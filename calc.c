#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("/proc calculator module");

static int calc_a = 0, calc_b = 0;
static char calc_op = '+';
static int calc_read_args = 0;

int calculate(int a, int b, char op)
{
    calc_read_args = 0;     // reset args counter
    if (op == '+')
        return a + b;
    if (op == '-')
        return a - b;
    if (op == '*')
        return a * b;
    if (op == '/')
        return a / b;
    if (op == '%')
        return a % b;
    return 0;
}

void save_input(const char *buf)
{
    if (calc_read_args == 0)
        calc_read_args += sscanf(buf, "%d %c %d", &calc_a, &calc_op, &calc_b);
    else if (calc_read_args == 1)
        calc_read_args += sscanf(buf, "%c %d", &calc_op, &calc_b);
    else if (calc_read_args == 2)
        calc_read_args += sscanf(buf, "%d", &calc_b);
}

static ssize_t read_callback(struct file *file, char *buf, size_t count, loff_t *ppos)
{
    char result_str[16];
    int len;

    if (calc_read_args == 3)
        sprintf(result_str, "%d\n", calculate(calc_a, calc_b, calc_op));
    else
        sprintf(result_str, "---\n");
    len = strlen(result_str);

    // We only support reading the whole string at once.
    if (count < len)
            return -EINVAL;
    // If file position is non-zero, then assume the string has
    // been read and indicate there is no more data to be read.
    if (*ppos != 0)
            return 0;

    if (copy_to_user(buf, result_str, len))
        return -EINVAL;
    // Tell the user how much data we wrote.
    *ppos = len;
    return len;
}

static ssize_t write_callback(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    char read_buf[128];
    int read_buf_size = 128;

    if (count < read_buf_size)
        read_buf_size = count;
    if (copy_from_user(read_buf, buf, read_buf_size))
        return -EINVAL;

    printk(KERN_INFO "calc input: %s\n", buf);
    save_input(read_buf);

    return read_buf_size;
}

static const struct file_operations calc_fops = {
    .owner = THIS_MODULE,
    .read  = read_callback,
    .write = write_callback,
};

static int __init calc_init(void)
{
    printk(KERN_INFO "Calc module loaded\n");

    if (!proc_create("calc", 0666, NULL, &calc_fops)) {
        printk(KERN_ERR "cannot create calc entry in /proc\n");
        return -ENOMEM;
    }

    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit calc_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
    remove_proc_entry("calc", NULL);
}

module_init(calc_init);
module_exit(calc_cleanup);

// http://www.linuxdevcenter.com/pub/a/linux/2007/07/05/devhelloworld-a-simple-introduction-to-device-drivers-under-linux.html?page=2
