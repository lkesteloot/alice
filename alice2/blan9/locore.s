.text
	.align 16
.globl LOCORE_task_switch
	.type	 LOCORE_task_switch,@function
LOCORE_task_switch:
	pushl %ebp
	movl %esp,%ebp

	movl 8(%ebp),%eax   # old_sp
	cmpl $0,%eax
	je no_save          # XXX when will this happen?!

	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	movl %esp,(%eax)

no_save:

	movl 12(%ebp),%eax  # new_sp
	movl (%eax),%esp

	# sync this with kern.c:KERN_exec()

	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
	popl %ebp
	ret
