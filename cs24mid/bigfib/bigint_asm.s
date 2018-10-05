
# this file implements the function
# int __add_bigint(uint64_t *a, uint64_t *b, uint8_t size)
# where we add two size-length array-representations of 
# 64-bit quadwords a and b, updating b. 
# we return 1 if the function completes
# without overflow, and 0 otherwise.


.globl __add_bigint
__add_bigint:
# a in rdi, b in rsi, size in rdx

# don't need to save callee save registers, since we're not
# using them

# first, check whether size = 0; if so, return 1
cmpq $0, %rdx
movq $1, %rax
je program_end

# zero the registers that we'll be using; clear and save
# carry flag, since it might have been changed by the cmp
# above, and will be changed by the cmp below
xor %r10, %r10
xor %rcx, %rcx
xor %r11, %r11
clc
pushf 

# go through each element in the size-length array
loop_beginning:
	cmp %r10, %rdx 				# check counter
	je loop_end					# if we're at the last slot,
								# exit the loop
	popf						# recall carry flag
	mov (%rdi, %r10, 8), %rcx	
	adc %rcx, (%rsi, %r10, 8)	# add corresponding slots
	mov %rcx, (%rdi, %r10, 8)
	inc %r10					# increment loop counter
	pushf						# save carry flag
	jmp loop_beginning			# repeat 

loop_end:
	popf						# recall the carry out from
								# the last addition
	jnc program_end				# check overflow
	mov $0, %rax				# if overflow, return 0
	
program_end:
	ret 						# otherwise, return 1


