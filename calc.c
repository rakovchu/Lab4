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

static ssize_t write_a_callback(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    char read_buf[128];
    int read_buf_size = 128;

    if (count < read_buf_size)
        read_buf_size = count;
    if (copy_from_user(read_buf, buf, read_buf_size))
        return -EINVAL;

    printk(KERN_INFO "calc input: %s\n", buf);
    if (sscanf(buf, "%d", &calc_a) == 1)
        calc_read_args++;
    else
        printk(KERN_ERR "wrong format!\n");

    return read_buf_size;
}

static ssize_t write_b_callback(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    char read_buf[128];
    int read_buf_size = 128;

    if (count < read_buf_size)
        read_buf_size = count;
    if (copy_from_user(read_buf, buf, read_buf_size))
        return -EINVAL;

    printk(KERN_INFO "calc input: %s\n", buf);
    if (sscanf(buf, "%d", &calc_b) == 1)
        calc_read_args++;
    else
        printk(KERN_ERR "wrong format!\n");

    return read_buf_size;
}

static ssize_t write_op_callback(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
    char read_buf[128];
    int read_buf_size = 128;

    if (count < read_buf_size)
        read_buf_size = count;
    if (copy_from_user(read_buf, buf, read_buf_size))
        return -EINVAL;

    printk(KERN_INFO "calc input: %s\n", buf);
    if (sscanf(buf, "%c", &calc_op) == 1)
        calc_read_args++;
    else
        printk(KERN_ERR "wrong format!\n");

    return read_buf_size;
}

static ssize_t read_result_callback(struct file *file, char *buf, size_t count, loff_t *ppos)
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

static const struct file_operations calc_a_fops = {
    .owner = THIS_MODULE,
    .write  = write_a_callback,
};

static const struct file_operations calc_b_fops = {
    .owner = THIS_MODULE,
    .write  = write_b_callback,
};

static const struct file_operations calc_op_fops = {
    .owner = THIS_MODULE,
    .write  = write_op_callback,
};

static const struct file_operations calc_res_fops = {
    .owner = THIS_MODULE,
    .read  = read_result_callback,
};

static int __init calc_init(void)
{
    printk(KERN_INFO "Calc module loaded\n");

    if (!proc_create("calc_a", 0666, NULL, &calc_a_fops)) {
        printk(KERN_ERR "cannot create calc_a entry in /proc\n");
        return -ENOMEM;
    }
    if (!proc_create("calc_b", 0666, NULL, &calc_b_fops)) {
        printk(KERN_ERR "cannot create calc_b entry in /proc\n");
        return -ENOMEM;
    }
    if (!proc_create("calc_op", 0666, NULL, &calc_op_fops)) {
        printk(KERN_ERR "cannot create calc_op entry in /proc\n");
        return -ENOMEM;
    }
    if (!proc_create("calc_res", 0666, NULL, &calc_res_fops)) {
        printk(KERN_ERR "cannot create calc_res entry in /proc\n");
        return -ENOMEM;
    }

    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit calc_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
    remove_proc_entry("calc_a", NULL);
    remove_proc_entry("calc_b", NULL);
    remove_proc_entry("calc_op", NULL);
    remove_proc_entry("calc_res", NULL);
}

module_init(calc_init);
module_exit(calc_cleanup);

// http://www.linuxdevcenter.com/pub/a/linux/2007/07/05/devhelloworld-a-simple-introduction-to-device-drivers-under-linux.html?page=2
