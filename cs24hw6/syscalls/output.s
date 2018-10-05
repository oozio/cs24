#ssize_t write(int fd, const void *buf, size_t count)

#int64_t output(void *data, uint64_t size);
# data in %rdi, size in %rsi
# system call number in %rax

#define __NR_write 1
.globl output

output:
# we want the size to be in rdx and the data to be in rsi, so 
# we shuffle around these two values to the correct registers
mov %rsi, %rdx
mov %rdi, %rsi

# the number for write is 1, so set rax 
# to write to stdout, set rdi to 1
mov $1, %rax
mov $1, %rdi
#rsi and rdx already ok

# make call
syscall
