	.file	"c7.c"
	.text
	.globl	whiletrue
	.data
	.align 4
	.type	whiletrue, @object
	.size	whiletrue, 4
whiletrue:
	.long	1
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movb	$1, -13(%rbp)
	cmpb	$0, -13(%rbp)
	jne	.L4
	movl	$2, -12(%rbp)
	movl	$3, -8(%rbp)
	movl	-12(%rbp), %edx
	movl	-8(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -4(%rbp)
	jmp	.L1
.L4:
	nop
.L1:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.globl	main2
	.type	main2, @function
main2:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movb	$1, -1(%rbp)
	movl	$0, %eax
	call	main
	movl	$2, whiletrue(%rip)
	jmp	.L6
.L8:
	cmpb	$0, -1(%rbp)
	je	.L7
	movl	whiletrue(%rip), %eax
	addl	$1, %eax
	movl	%eax, whiletrue(%rip)
	movzbl	-1(%rbp), %eax
	testl	%eax, %eax
	setne	%al
	xorl	$1, %eax
	movzbl	%al, %eax
	movb	%al, -1(%rbp)
	andb	$1, -1(%rbp)
	jmp	.L6
.L7:
	movl	whiletrue(%rip), %eax
	addl	$2, %eax
	movl	%eax, whiletrue(%rip)
.L6:
	movzbl	-1(%rbp), %eax
	xorl	$1, %eax
	testb	%al, %al
	jne	.L8
	movl	whiletrue(%rip), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main2, .-main2
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04.2) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
