	.text
	.file	"test.language.ll"
	.globl	_Z2hiv
	.p2align	4, 0x90
	.type	_Z2hiv,@function
_Z2hiv:                                 # @_Z2hiv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	movl	$72, %edi
	callq	prt_c
	movl	$105, %edi
	callq	prt_c
	movl	$10, %edi
	callq	prt_c
	popq	%rax
	retq
.Lfunc_end0:
	.size	_Z2hiv, .Lfunc_end0-_Z2hiv
	.cfi_endproc

	.globl	_Z4loopv
	.p2align	4, 0x90
	.type	_Z4loopv,@function
_Z4loopv:                               # @_Z4loopv
	.cfi_startproc
# BB#0:                                 # %ret
	retq
.Lfunc_end1:
	.size	_Z4loopv, .Lfunc_end1-_Z4loopv
	.cfi_endproc

	.globl	_Z12testasdfasdfi
	.p2align	4, 0x90
	.type	_Z12testasdfasdfi,@function
_Z12testasdfasdfi:                      # @_Z12testasdfasdfi
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	movq	$767, -8(%rsp)          # imm = 0x2FF
	movabsq	$4649957819167014912, %rax # imm = 0x4087F80000000000
	movq	%rax, -16(%rsp)
	movsd	-16(%rsp), %xmm0        # xmm0 = mem[0],zero
	retq
.Lfunc_end2:
	.size	_Z12testasdfasdfi, .Lfunc_end2-_Z12testasdfasdfi
	.cfi_endproc

	.globl	_Z2yov
	.p2align	4, 0x90
	.type	_Z2yov,@function
_Z2yov:                                 # @_Z2yov
	.cfi_startproc
# BB#0:                                 # %ret
	retq
.Lfunc_end3:
	.size	_Z2yov, .Lfunc_end3-_Z2yov
	.cfi_endproc

	.globl	_Z4testi
	.p2align	4, 0x90
	.type	_Z4testi,@function
_Z4testi:                               # @_Z4testi
	.cfi_startproc
# BB#0:                                 # %ret
	movq	%rdi, -8(%rsp)
	retq
.Lfunc_end4:
	.size	_Z4testi, .Lfunc_end4-_Z4testi
	.cfi_endproc

	.globl	_Z3blaii
	.p2align	4, 0x90
	.type	_Z3blaii,@function
_Z3blaii:                               # @_Z3blaii
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp1:
	.cfi_def_cfa_offset 32
	movq	%rdi, 16(%rsp)
	movq	%rsi, 8(%rsp)
	movl	$1, %edi
	callq	exit
	addq	$24, %rsp
	retq
.Lfunc_end5:
	.size	_Z3blaii, .Lfunc_end5-_Z3blaii
	.cfi_endproc

	.globl	_Z4bla2iiff
	.p2align	4, 0x90
	.type	_Z4bla2iiff,@function
_Z4bla2iiff:                            # @_Z4bla2iiff
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -16(%rsp)
	movq	%rsi, -24(%rsp)
	movsd	%xmm0, -32(%rsp)
	movsd	%xmm1, -40(%rsp)
	movq	$2, -8(%rsp)
	retq
.Lfunc_end6:
	.size	_Z4bla2iiff, .Lfunc_end6-_Z4bla2iiff
	.cfi_endproc

	.globl	_Z16whatever_fn_nameifffcccccb
	.p2align	4, 0x90
	.type	_Z16whatever_fn_nameifffcccccb,@function
_Z16whatever_fn_nameifffcccccb:         # @_Z16whatever_fn_nameifffcccccb
	.cfi_startproc
# BB#0:                                 # %ret
	movb	8(%rsp), %al
	movq	%rdi, -8(%rsp)
	movsd	%xmm0, -16(%rsp)
	movsd	%xmm1, -24(%rsp)
	movsd	%xmm2, -32(%rsp)
	movb	%sil, -33(%rsp)
	movb	%dl, -34(%rsp)
	movb	%cl, -35(%rsp)
	movb	%r8b, -36(%rsp)
	movb	%r9b, -37(%rsp)
	andb	$1, %al
	movb	%al, -38(%rsp)
	retq
.Lfunc_end7:
	.size	_Z16whatever_fn_nameifffcccccb, .Lfunc_end7-_Z16whatever_fn_nameifffcccccb
	.cfi_endproc

	.globl	_Z7complexib
	.p2align	4, 0x90
	.type	_Z7complexib,@function
_Z7complexib:                           # @_Z7complexib
	.cfi_startproc
# BB#0:                                 # %fnbody
	movq	%rdi, -8(%rsp)
	andl	$1, %esi
	movb	%sil, -41(%rsp)
	movq	$0, -40(%rsp)
	movabsq	$4653010063445721088, %rax # imm = 0x4092D00000000000
	movq	%rax, -40(%rsp)
	movabsq	$4631107791820423168, %rax # imm = 0x4045000000000000
	jmp	.LBB8_1
	.p2align	4, 0x90
.LBB8_2:                                # %whlloop
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rax, -40(%rsp)
.LBB8_1:                                # %whlhead
                                        # =>This Inner Loop Header: Depth=1
	movzbl	-41(%rsp), %ecx
	cmpl	$1, %ecx
	je	.LBB8_2
# BB#3:                                 # %doend
	movq	$0, -24(%rsp)
	movq	$7, -24(%rsp)
	movq	$0, -32(%rsp)
	jmp	.LBB8_4
	.p2align	4, 0x90
.LBB8_5:                                # %forloop
                                        #   in Loop: Header=BB8_4 Depth=1
	addq	$2, -24(%rsp)
	incq	-32(%rsp)
.LBB8_4:                                # %forhead
                                        # =>This Inner Loop Header: Depth=1
	cmpq	$6, -32(%rsp)
	jle	.LBB8_5
# BB#6:                                 # %forend
	movzbl	-41(%rsp), %eax
	cmpl	$1, %eax
	jne	.LBB8_8
# BB#7:                                 # %then
	movabsq	$4638144666238189568, %rax # imm = 0x405E000000000000
	movq	%rax, -40(%rsp)
.LBB8_8:                                # %ret
	movsd	-40(%rsp), %xmm0        # xmm0 = mem[0],zero
	movsd	%xmm0, -16(%rsp)
	retq
.Lfunc_end8:
	.size	_Z7complexib, .Lfunc_end8-_Z7complexib
	.cfi_endproc

	.globl	_Z6doublev
	.p2align	4, 0x90
	.type	_Z6doublev,@function
_Z6doublev:                             # @_Z6doublev
	.cfi_startproc
# BB#0:                                 # %entry
	movabsq	$4608238512912635789, %rax # imm = 0x3FF3C083126E978D
	movq	%rax, -8(%rsp)
	movsd	-8(%rsp), %xmm0         # xmm0 = mem[0],zero
	retq
.Lfunc_end9:
	.size	_Z6doublev, .Lfunc_end9-_Z6doublev
	.cfi_endproc

	.globl	_Z7integerv
	.p2align	4, 0x90
	.type	_Z7integerv,@function
_Z7integerv:                            # @_Z7integerv
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$123, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end10:
	.size	_Z7integerv, .Lfunc_end10-_Z7integerv
	.cfi_endproc

	.globl	_Z7booleanv
	.p2align	4, 0x90
	.type	_Z7booleanv,@function
_Z7booleanv:                            # @_Z7booleanv
	.cfi_startproc
# BB#0:                                 # %entry
	movb	$1, -1(%rsp)
	movb	-1(%rsp), %al
	retq
.Lfunc_end11:
	.size	_Z7booleanv, .Lfunc_end11-_Z7booleanv
	.cfi_endproc

	.globl	_Z4charv
	.p2align	4, 0x90
	.type	_Z4charv,@function
_Z4charv:                               # @_Z4charv
	.cfi_startproc
# BB#0:                                 # %entry
	movb	$99, -1(%rsp)
	movb	-1(%rsp), %al
	retq
.Lfunc_end12:
	.size	_Z4charv, .Lfunc_end12-_Z4charv
	.cfi_endproc

	.globl	_Z9duplicatev
	.p2align	4, 0x90
	.type	_Z9duplicatev,@function
_Z9duplicatev:                          # @_Z9duplicatev
	.cfi_startproc
# BB#0:                                 # %ret
	retq
.Lfunc_end13:
	.size	_Z9duplicatev, .Lfunc_end13-_Z9duplicatev
	.cfi_endproc

	.globl	_Z6doublef
	.p2align	4, 0x90
	.type	_Z6doublef,@function
_Z6doublef:                             # @_Z6doublef
	.cfi_startproc
# BB#0:                                 # %entry
	movsd	%xmm0, -8(%rsp)
	movsd	-8(%rsp), %xmm0         # xmm0 = mem[0],zero
	movsd	%xmm0, -16(%rsp)
	movsd	-16(%rsp), %xmm0        # xmm0 = mem[0],zero
	retq
.Lfunc_end14:
	.size	_Z6doublef, .Lfunc_end14-_Z6doublef
	.cfi_endproc

	.globl	_Z10double_f_fv
	.p2align	4, 0x90
	.type	_Z10double_f_fv,@function
_Z10double_f_fv:                        # @_Z10double_f_fv
	.cfi_startproc
# BB#0:                                 # %ret
	retq
.Lfunc_end15:
	.size	_Z10double_f_fv, .Lfunc_end15-_Z10double_f_fv
	.cfi_endproc

	.globl	_Z11returnParami
	.p2align	4, 0x90
	.type	_Z11returnParami,@function
_Z11returnParami:                       # @_Z11returnParami
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	movq	-8(%rsp), %rax
	movq	%rax, -16(%rsp)
	movq	-16(%rsp), %rax
	retq
.Lfunc_end16:
	.size	_Z11returnParami, .Lfunc_end16-_Z11returnParami
	.cfi_endproc

	.globl	_Z10whlVarTestbif
	.p2align	4, 0x90
	.type	_Z10whlVarTestbif,@function
_Z10whlVarTestbif:                      # @_Z10whlVarTestbif
	.cfi_startproc
# BB#0:                                 # %whlhead
	andl	$1, %edi
	movb	%dil, -17(%rsp)
	movq	%rsi, -8(%rsp)
	movsd	%xmm0, -16(%rsp)
	movzbl	-17(%rsp), %eax
	cmpl	$1, %eax
	retq
.Lfunc_end17:
	.size	_Z10whlVarTestbif, .Lfunc_end17-_Z10whlVarTestbif
	.cfi_endproc

	.globl	_Z12int_flt_casti
	.p2align	4, 0x90
	.type	_Z12int_flt_casti,@function
_Z12int_flt_casti:                      # @_Z12int_flt_casti
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	cvtsi2sdq	-8(%rsp), %xmm0
	movsd	%xmm0, -16(%rsp)
	movsd	-16(%rsp), %xmm0        # xmm0 = mem[0],zero
	retq
.Lfunc_end18:
	.size	_Z12int_flt_casti, .Lfunc_end18-_Z12int_flt_casti
	.cfi_endproc

	.globl	_Z12flt_int_castf
	.p2align	4, 0x90
	.type	_Z12flt_int_castf,@function
_Z12flt_int_castf:                      # @_Z12flt_int_castf
	.cfi_startproc
# BB#0:                                 # %entry
	movsd	%xmm0, -8(%rsp)
	cvttsd2si	-8(%rsp), %rax
	movq	%rax, -16(%rsp)
	movq	-16(%rsp), %rax
	retq
.Lfunc_end19:
	.size	_Z12flt_int_castf, .Lfunc_end19-_Z12flt_int_castf
	.cfi_endproc

	.globl	_Z12int_chr_casti
	.p2align	4, 0x90
	.type	_Z12int_chr_casti,@function
_Z12int_chr_casti:                      # @_Z12int_chr_casti
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	movb	-8(%rsp), %al
	movb	%al, -9(%rsp)
	movb	-9(%rsp), %al
	retq
.Lfunc_end20:
	.size	_Z12int_chr_casti, .Lfunc_end20-_Z12int_chr_casti
	.cfi_endproc

	.globl	_Z12chr_flt_castc
	.p2align	4, 0x90
	.type	_Z12chr_flt_castc,@function
_Z12chr_flt_castc:                      # @_Z12chr_flt_castc
	.cfi_startproc
# BB#0:                                 # %entry
	movb	%dil, -9(%rsp)
	movsbl	-9(%rsp), %eax
	cvtsi2sdl	%eax, %xmm0
	movsd	%xmm0, -8(%rsp)
	movsd	-8(%rsp), %xmm0         # xmm0 = mem[0],zero
	retq
.Lfunc_end21:
	.size	_Z12chr_flt_castc, .Lfunc_end21-_Z12chr_flt_castc
	.cfi_endproc

	.globl	_Z12chr_int_castc
	.p2align	4, 0x90
	.type	_Z12chr_int_castc,@function
_Z12chr_int_castc:                      # @_Z12chr_int_castc
	.cfi_startproc
# BB#0:                                 # %entry
	movb	%dil, -9(%rsp)
	movsbq	-9(%rsp), %rax
	movq	%rax, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end22:
	.size	_Z12chr_int_castc, .Lfunc_end22-_Z12chr_int_castc
	.cfi_endproc

	.globl	_Z12boo_int_castb
	.p2align	4, 0x90
	.type	_Z12boo_int_castb,@function
_Z12boo_int_castb:                      # @_Z12boo_int_castb
	.cfi_startproc
# BB#0:                                 # %entry
	andl	$1, %edi
	movb	%dil, -9(%rsp)
	movzbl	-9(%rsp), %eax
	shlq	$63, %rax
	sarq	$63, %rax
	movq	%rax, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end23:
	.size	_Z12boo_int_castb, .Lfunc_end23-_Z12boo_int_castb
	.cfi_endproc

	.globl	_Z12boo_flt_castb
	.p2align	4, 0x90
	.type	_Z12boo_flt_castb,@function
_Z12boo_flt_castb:                      # @_Z12boo_flt_castb
	.cfi_startproc
# BB#0:                                 # %entry
	andl	$1, %edi
	movb	%dil, -9(%rsp)
	movb	-9(%rsp), %al
	shlb	$7, %al
	sarb	$7, %al
	movsbl	%al, %eax
	cvtsi2sdl	%eax, %xmm0
	movsd	%xmm0, -8(%rsp)
	movsd	-8(%rsp), %xmm0         # xmm0 = mem[0],zero
	retq
.Lfunc_end24:
	.size	_Z12boo_flt_castb, .Lfunc_end24-_Z12boo_flt_castb
	.cfi_endproc

	.globl	_Z12chr_boo_castc
	.p2align	4, 0x90
	.type	_Z12chr_boo_castc,@function
_Z12chr_boo_castc:                      # @_Z12chr_boo_castc
	.cfi_startproc
# BB#0:                                 # %entry
	movb	%dil, -1(%rsp)
	cmpb	$0, -1(%rsp)
	setne	-2(%rsp)
	movb	-2(%rsp), %al
	retq
.Lfunc_end25:
	.size	_Z12chr_boo_castc, .Lfunc_end25-_Z12chr_boo_castc
	.cfi_endproc

	.globl	_Z12flt_boo_castf
	.p2align	4, 0x90
	.type	_Z12flt_boo_castf,@function
_Z12flt_boo_castf:                      # @_Z12flt_boo_castf
	.cfi_startproc
# BB#0:                                 # %entry
	movsd	%xmm0, -8(%rsp)
	xorpd	%xmm0, %xmm0
	ucomisd	-8(%rsp), %xmm0
	setne	-9(%rsp)
	movb	-9(%rsp), %al
	retq
.Lfunc_end26:
	.size	_Z12flt_boo_castf, .Lfunc_end26-_Z12flt_boo_castf
	.cfi_endproc

	.globl	_Z11assign_testiii
	.p2align	4, 0x90
	.type	_Z11assign_testiii,@function
_Z11assign_testiii:                     # @_Z11assign_testiii
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -16(%rsp)
	movq	%rsi, -24(%rsp)
	movq	%rdx, -32(%rsp)
	movq	-32(%rsp), %rax
	movq	%rax, -16(%rsp)
	addq	-24(%rsp), %rax
	movq	%rax, -16(%rsp)
	movq	-24(%rsp), %rcx
	imulq	-32(%rsp), %rcx
	addq	%rax, %rcx
	movq	%rcx, -32(%rsp)
	movq	%rcx, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end27:
	.size	_Z11assign_testiii, .Lfunc_end27-_Z11assign_testiii
	.cfi_endproc

	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3
.LCPI28_0:
	.quad	4616752568008179712     # double 4.5
	.text
	.globl	_Z10local_varsb
	.p2align	4, 0x90
	.type	_Z10local_varsb,@function
_Z10local_varsb:                        # @_Z10local_varsb
	.cfi_startproc
# BB#0:                                 # %entry
	andl	$1, %edi
	movb	%dil, -25(%rsp)
	movq	$42, -24(%rsp)
	movb	$99, -26(%rsp)
	movabsq	$4616752568008179712, %rax # imm = 0x4012000000000000
	movq	%rax, -8(%rsp)
	movzbl	-25(%rsp), %ecx
	shlq	$63, %rcx
	sarq	$63, %rcx
	movb	%cl, %al
	shlb	$7, %al
	sarb	$7, %al
	movsbl	%al, %eax
	cvtsi2sdl	%eax, %xmm0
	mulsd	.LCPI28_0(%rip), %xmm0
	movb	%cl, %al
	mulb	-26(%rsp)
	movsbq	%al, %rax
	imulq	-24(%rsp), %rcx
	incq	%rcx
	imulq	%rax, %rcx
	cvtsi2sdq	%rcx, %xmm1
	addsd	%xmm0, %xmm1
	movsd	%xmm1, -16(%rsp)
	movsd	-16(%rsp), %xmm0        # xmm0 = mem[0],zero
	retq
.Lfunc_end28:
	.size	_Z10local_varsb, .Lfunc_end28-_Z10local_varsb
	.cfi_endproc

	.globl	_Z10break_testb
	.p2align	4, 0x90
	.type	_Z10break_testb,@function
_Z10break_testb:                        # @_Z10break_testb
	.cfi_startproc
# BB#0:                                 # %ret
	andl	$1, %edi
	movb	%dil, -1(%rsp)
	retq
.Lfunc_end29:
	.size	_Z10break_testb, .Lfunc_end29-_Z10break_testb
	.cfi_endproc

	.globl	_Z7if_testb
	.p2align	4, 0x90
	.type	_Z7if_testb,@function
_Z7if_testb:                            # @_Z7if_testb
	.cfi_startproc
# BB#0:                                 # %entry
	andl	$1, %edi
	movb	%dil, -2(%rsp)
	.p2align	4, 0x90
.LBB30_1:                               # %doloop
                                        # =>This Inner Loop Header: Depth=1
	movzbl	-2(%rsp), %eax
	cmpl	$1, %eax
	jne	.LBB30_3
# BB#2:                                 # %dohead
                                        #   in Loop: Header=BB30_1 Depth=1
	movb	-2(%rsp), %al
	testb	%al, %al
	jne	.LBB30_1
	jmp	.LBB30_4
.LBB30_3:                               # %else
	movzbl	-2(%rsp), %eax
	cmpl	$1, %eax
	je	.LBB30_5
	.p2align	4, 0x90
.LBB30_4:                               # %dohead8
                                        # =>This Inner Loop Header: Depth=1
	movb	$1, -1(%rsp)
	jmp	.LBB30_4
.LBB30_5:                               # %ret
	retq
.Lfunc_end30:
	.size	_Z7if_testb, .Lfunc_end30-_Z7if_testb
	.cfi_endproc

	.globl	_Z12test_bitcompicb
	.p2align	4, 0x90
	.type	_Z12test_bitcompicb,@function
_Z12test_bitcompicb:                    # @_Z12test_bitcompicb
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	movb	%sil, -17(%rsp)
	andl	$1, %edx
	movb	%dl, -18(%rsp)
	movq	-8(%rsp), %rax
	movb	-18(%rsp), %cl
	shlb	$7, %cl
	sarb	$7, %cl
	movb	-17(%rsp), %dl
	notb	%dl
	andb	%cl, %dl
	notb	%cl
	movzbl	%cl, %ecx
	shlq	$63, %rcx
	sarq	$63, %rcx
	orq	%rax, %rcx
	movsbq	%dl, %rdx
	xorq	%rcx, %rdx
	notq	%rax
	orq	%rdx, %rax
	movq	%rax, -16(%rsp)
	movq	-16(%rsp), %rax
	retq
.Lfunc_end31:
	.size	_Z12test_bitcompicb, .Lfunc_end31-_Z12test_bitcompicb
	.cfi_endproc

	.globl	_Z10stringtestv
	.p2align	4, 0x90
	.type	_Z10stringtestv,@function
_Z10stringtestv:                        # @_Z10stringtestv
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$.Lstr_const, -32(%rsp)
	movq	$.Lstr_const.1, -8(%rsp)
	movq	$.Lstr_const.1, -16(%rsp)
	movq	-32(%rsp), %rax
	movq	%rax, -24(%rsp)
	movq	-24(%rsp), %rax
	retq
.Lfunc_end32:
	.size	_Z10stringtestv, .Lfunc_end32-_Z10stringtestv
	.cfi_endproc

	.globl	_Z12print_stringv
	.p2align	4, 0x90
	.type	_Z12print_stringv,@function
_Z12print_stringv:                      # @_Z12print_stringv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp2:
	.cfi_def_cfa_offset 16
	movl	$.Lstr_const.2, %edi
	callq	prt_s
	movq	$0, (%rsp)
	jmp	.LBB33_1
	.p2align	4, 0x90
.LBB33_2:                               # %forloop
                                        #   in Loop: Header=BB33_1 Depth=1
	movl	$.Lstr_const.3, %edi
	callq	prt_s
	movq	(%rsp), %rdi
	imulq	%rdi, %rdi
	callq	prt_i
	movl	$.Lstr_const.4, %edi
	callq	prt_s
	incq	(%rsp)
.LBB33_1:                               # %forhead
                                        # =>This Inner Loop Header: Depth=1
	cmpq	$99, (%rsp)
	jle	.LBB33_2
# BB#3:                                 # %forend
	movl	$10, %edi
	callq	prt_c
	popq	%rax
	retq
.Lfunc_end33:
	.size	_Z12print_stringv, .Lfunc_end33-_Z12print_stringv
	.cfi_endproc

	.globl	_Z16print_mult_tablev
	.p2align	4, 0x90
	.type	_Z16print_mult_tablev,@function
_Z16print_mult_tablev:                  # @_Z16print_mult_tablev
	.cfi_startproc
# BB#0:                                 # %fnbody
	subq	$24, %rsp
.Ltmp3:
	.cfi_def_cfa_offset 32
	movq	$0, 16(%rsp)
	movq	$0, 8(%rsp)
	movq	$1, 16(%rsp)
	jmp	.LBB34_1
	.p2align	4, 0x90
.LBB34_5:                               # %forend3
                                        #   in Loop: Header=BB34_1 Depth=1
	movl	$10, %edi
	callq	prt_c
	incq	16(%rsp)
.LBB34_1:                               # %forhead
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB34_3 Depth 2
	cmpq	$10, 16(%rsp)
	jg	.LBB34_6
# BB#2:                                 # %forloop
                                        #   in Loop: Header=BB34_1 Depth=1
	movq	$1, 8(%rsp)
	jmp	.LBB34_3
	.p2align	4, 0x90
.LBB34_4:                               # %forloop2
                                        #   in Loop: Header=BB34_3 Depth=2
	movq	16(%rsp), %rdi
	imulq	8(%rsp), %rdi
	movl	$3, %esi
	callq	prt_w_i
	movl	$32, %edi
	callq	prt_c
	incq	8(%rsp)
.LBB34_3:                               # %forhead1
                                        #   Parent Loop BB34_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	cmpq	$10, 8(%rsp)
	jle	.LBB34_4
	jmp	.LBB34_5
.LBB34_6:                               # %ret
	addq	$24, %rsp
	retq
.Lfunc_end34:
	.size	_Z16print_mult_tablev, .Lfunc_end34-_Z16print_mult_tablev
	.cfi_endproc

	.globl	_Z8test_fibv
	.p2align	4, 0x90
	.type	_Z8test_fibv,@function
_Z8test_fibv:                           # @_Z8test_fibv
	.cfi_startproc
# BB#0:                                 # %fnbody
	subq	$56, %rsp
.Ltmp4:
	.cfi_def_cfa_offset 64
	movq	$0, (%rsp)
	jmp	.LBB35_1
	.p2align	4, 0x90
.LBB35_8:                               # %_Z8fib_iteri.exit
                                        #   in Loop: Header=BB35_1 Depth=1
	movq	40(%rsp), %rdi
	callq	prtln_i
	incq	(%rsp)
.LBB35_1:                               # %forhead
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB35_5 Depth 2
	cmpq	$34, (%rsp)
	jg	.LBB35_9
# BB#2:                                 # %forloop
                                        #   in Loop: Header=BB35_1 Depth=1
	movq	(%rsp), %rdi
	callq	_Z3fibi
	movq	%rax, %rdi
	callq	prt_i
	movl	$.Lstr_const.5, %edi
	callq	prt_s
	movq	(%rsp), %rax
	movq	%rax, 48(%rsp)
	cmpq	$2, %rax
	jg	.LBB35_4
# BB#3:                                 # %then.i
                                        #   in Loop: Header=BB35_1 Depth=1
	movq	$1, 40(%rsp)
	jmp	.LBB35_8
	.p2align	4, 0x90
.LBB35_4:                               # %ifend.i
                                        #   in Loop: Header=BB35_1 Depth=1
	movq	$1, 24(%rsp)
	movq	$1, 32(%rsp)
	movq	$0, 16(%rsp)
	movq	$0, 8(%rsp)
	movq	$3, 8(%rsp)
	jmp	.LBB35_5
	.p2align	4, 0x90
.LBB35_6:                               # %forloop.i
                                        #   in Loop: Header=BB35_5 Depth=2
	movq	24(%rsp), %rax
	addq	32(%rsp), %rax
	movq	%rax, 16(%rsp)
	movq	24(%rsp), %rax
	movq	%rax, 32(%rsp)
	movq	16(%rsp), %rax
	movq	%rax, 24(%rsp)
	incq	8(%rsp)
.LBB35_5:                               # %forhead.i
                                        #   Parent Loop BB35_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	8(%rsp), %rax
	cmpq	48(%rsp), %rax
	jle	.LBB35_6
# BB#7:                                 # %forend.i
                                        #   in Loop: Header=BB35_1 Depth=1
	movq	16(%rsp), %rax
	movq	%rax, 40(%rsp)
	jmp	.LBB35_8
.LBB35_9:                               # %forend
	movq	$0, (%rsp)
	jmp	.LBB35_10
	.p2align	4, 0x90
.LBB35_17:                              # %_Z8fib_iteri.exit38
                                        #   in Loop: Header=BB35_10 Depth=1
	movq	40(%rsp), %rdi
	callq	prtln_i
	incq	(%rsp)
.LBB35_10:                              # %forhead5
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB35_14 Depth 2
	cmpq	$92, (%rsp)
	jg	.LBB35_18
# BB#11:                                # %forloop6
                                        #   in Loop: Header=BB35_10 Depth=1
	movq	(%rsp), %rax
	movq	%rax, 48(%rsp)
	cmpq	$2, %rax
	jg	.LBB35_13
# BB#12:                                # %then.i22
                                        #   in Loop: Header=BB35_10 Depth=1
	movq	$1, 40(%rsp)
	jmp	.LBB35_17
	.p2align	4, 0x90
.LBB35_13:                              # %ifend.i23
                                        #   in Loop: Header=BB35_10 Depth=1
	movq	$1, 24(%rsp)
	movq	$1, 32(%rsp)
	movq	$0, 16(%rsp)
	movq	$0, 8(%rsp)
	movq	$3, 8(%rsp)
	jmp	.LBB35_14
	.p2align	4, 0x90
.LBB35_15:                              # %forloop.i35
                                        #   in Loop: Header=BB35_14 Depth=2
	movq	24(%rsp), %rax
	addq	32(%rsp), %rax
	movq	%rax, 16(%rsp)
	movq	24(%rsp), %rax
	movq	%rax, 32(%rsp)
	movq	16(%rsp), %rax
	movq	%rax, 24(%rsp)
	incq	8(%rsp)
.LBB35_14:                              # %forhead.i27
                                        #   Parent Loop BB35_10 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	8(%rsp), %rax
	cmpq	48(%rsp), %rax
	jle	.LBB35_15
# BB#16:                                # %forend.i37
                                        #   in Loop: Header=BB35_10 Depth=1
	movq	16(%rsp), %rax
	movq	%rax, 40(%rsp)
	jmp	.LBB35_17
.LBB35_18:                              # %ret
	addq	$56, %rsp
	retq
.Lfunc_end35:
	.size	_Z8test_fibv, .Lfunc_end35-_Z8test_fibv
	.cfi_endproc

	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3
.LCPI36_0:
	.quad	4608083138725491507     # double 1.2
.LCPI36_1:
	.quad	4614253070214989087     # double 3.1400000000000001
.LCPI36_2:
	.quad	4631141568817628447     # double 42.240000000000002
.LCPI36_3:
	.quad	9214871658872686752     # double 1.0E+308
	.text
	.globl	_Z9testStuffv
	.p2align	4, 0x90
	.type	_Z9testStuffv,@function
_Z9testStuffv:                          # @_Z9testStuffv
	.cfi_startproc
# BB#0:                                 # %fnbody
	subq	$24, %rsp
.Ltmp5:
	.cfi_def_cfa_offset 32
	callq	_Z10forvartestv
	movb	$0, 15(%rsp)
	xorl	%eax, %eax
	testb	%al, %al
	je	.LBB36_2
# BB#1:                                 # %whlloop.i
	movb	15(%rsp), %al
	testb	%al, %al
	je	.LBB36_2
# BB#4:                                 # %then.i
	movabsq	$4614253070214989087, %rax # imm = 0x40091EB851EB851F
	jmp	.LBB36_3
.LBB36_2:                               # %whlend.i
	movabsq	$4613937818241073152, %rax # imm = 0x4008000000000000
.LBB36_3:                               # %ret
	movq	%rax, 16(%rsp)
	movsd	.LCPI36_0(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	xorpd	%xmm0, %xmm0
	callq	prtln_f
	movsd	.LCPI36_1(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	movsd	.LCPI36_2(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	movsd	.LCPI36_3(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	addq	$24, %rsp
	retq
.Lfunc_end36:
	.size	_Z9testStuffv, .Lfunc_end36-_Z9testStuffv
	.cfi_endproc

	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp6:
	.cfi_def_cfa_offset 32
	movl	$72, %edi
	callq	prt_c
	movl	$105, %edi
	callq	prt_c
	movl	$10, %edi
	callq	prt_c
	callq	_Z12print_stringv
	movq	$0, 16(%rsp)
	movq	$0, 8(%rsp)
	movq	$1, 16(%rsp)
	jmp	.LBB37_1
	.p2align	4, 0x90
.LBB37_5:                               # %forend3.i
                                        #   in Loop: Header=BB37_1 Depth=1
	movl	$10, %edi
	callq	prt_c
	incq	16(%rsp)
.LBB37_1:                               # %forhead.i
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB37_3 Depth 2
	cmpq	$10, 16(%rsp)
	jg	.LBB37_6
# BB#2:                                 # %forloop.i
                                        #   in Loop: Header=BB37_1 Depth=1
	movq	$1, 8(%rsp)
	jmp	.LBB37_3
	.p2align	4, 0x90
.LBB37_4:                               # %forloop2.i
                                        #   in Loop: Header=BB37_3 Depth=2
	movq	16(%rsp), %rdi
	imulq	8(%rsp), %rdi
	movl	$3, %esi
	callq	prt_w_i
	movl	$32, %edi
	callq	prt_c
	incq	8(%rsp)
.LBB37_3:                               # %forhead1.i
                                        #   Parent Loop BB37_1 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	cmpq	$10, 8(%rsp)
	jle	.LBB37_4
	jmp	.LBB37_5
.LBB37_6:                               # %_Z16print_mult_tablev.exit
	callq	_Z8test_fibv
	callq	_Z9testStuffv
	xorl	%edi, %edi
	callq	exit
	addq	$24, %rsp
	retq
.Lfunc_end37:
	.size	main, .Lfunc_end37-main
	.cfi_endproc

	.globl	_Z8overloadi
	.p2align	4, 0x90
	.type	_Z8overloadi,@function
_Z8overloadi:                           # @_Z8overloadi
	.cfi_startproc
# BB#0:                                 # %ret
	movq	%rdi, -8(%rsp)
	retq
.Lfunc_end38:
	.size	_Z8overloadi, .Lfunc_end38-_Z8overloadi
	.cfi_endproc

	.globl	_Z8overloadf
	.p2align	4, 0x90
	.type	_Z8overloadf,@function
_Z8overloadf:                           # @_Z8overloadf
	.cfi_startproc
# BB#0:                                 # %ret
	movsd	%xmm0, -8(%rsp)
	retq
.Lfunc_end39:
	.size	_Z8overloadf, .Lfunc_end39-_Z8overloadf
	.cfi_endproc

	.globl	_Z13call_overloadv
	.p2align	4, 0x90
	.type	_Z13call_overloadv,@function
_Z13call_overloadv:                     # @_Z13call_overloadv
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$1, -8(%rsp)
	movabsq	$4614256656552045848, %rax # imm = 0x400921FB54442D18
	movq	%rax, -8(%rsp)
	retq
.Lfunc_end40:
	.size	_Z13call_overloadv, .Lfunc_end40-_Z13call_overloadv
	.cfi_endproc

	.globl	_Z8flt_callf
	.p2align	4, 0x90
	.type	_Z8flt_callf,@function
_Z8flt_callf:                           # @_Z8flt_callf
	.cfi_startproc
# BB#0:                                 # %ret
	movsd	%xmm0, -8(%rsp)
	retq
.Lfunc_end41:
	.size	_Z8flt_callf, .Lfunc_end41-_Z8flt_callf
	.cfi_endproc

	.globl	_Z13call_imp_castv
	.p2align	4, 0x90
	.type	_Z13call_imp_castv,@function
_Z13call_imp_castv:                     # @_Z13call_imp_castv
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$0, -8(%rsp)
	retq
.Lfunc_end42:
	.size	_Z13call_imp_castv, .Lfunc_end42-_Z13call_imp_castv
	.cfi_endproc

	.globl	_Z3fibi
	.p2align	4, 0x90
	.type	_Z3fibi,@function
_Z3fibi:                                # @_Z3fibi
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp7:
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
.Ltmp8:
	.cfi_def_cfa_offset 32
.Ltmp9:
	.cfi_offset %rbx, -16
	movq	%rdi, (%rsp)
	cmpq	$2, (%rsp)
	jg	.LBB43_2
# BB#1:                                 # %then
	movq	$1, 8(%rsp)
	jmp	.LBB43_3
.LBB43_2:                               # %ifend
	movq	(%rsp), %rdi
	addq	$-2, %rdi
	callq	_Z3fibi
	movq	%rax, %rbx
	movq	(%rsp), %rdi
	decq	%rdi
	callq	_Z3fibi
	addq	%rbx, %rax
	movq	%rax, 8(%rsp)
.LBB43_3:                               # %ret
	movq	8(%rsp), %rax
	addq	$16, %rsp
	popq	%rbx
	retq
.Lfunc_end43:
	.size	_Z3fibi, .Lfunc_end43-_Z3fibi
	.cfi_endproc

	.globl	_Z8fib_iteri
	.p2align	4, 0x90
	.type	_Z8fib_iteri,@function
_Z8fib_iteri:                           # @_Z8fib_iteri
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	cmpq	$2, -8(%rsp)
	jg	.LBB44_2
# BB#1:                                 # %then
	movq	$1, -16(%rsp)
	movq	-16(%rsp), %rax
	retq
.LBB44_2:                               # %ifend
	movq	$1, -32(%rsp)
	movq	$1, -24(%rsp)
	movq	$0, -40(%rsp)
	movq	$0, -48(%rsp)
	movq	$3, -48(%rsp)
	jmp	.LBB44_3
	.p2align	4, 0x90
.LBB44_4:                               # %forloop
                                        #   in Loop: Header=BB44_3 Depth=1
	movq	-32(%rsp), %rax
	addq	-24(%rsp), %rax
	movq	%rax, -40(%rsp)
	movq	-32(%rsp), %rax
	movq	%rax, -24(%rsp)
	movq	-40(%rsp), %rax
	movq	%rax, -32(%rsp)
	incq	-48(%rsp)
.LBB44_3:                               # %forhead
                                        # =>This Inner Loop Header: Depth=1
	movq	-48(%rsp), %rax
	cmpq	-8(%rsp), %rax
	jle	.LBB44_4
# BB#5:                                 # %forend
	movq	-40(%rsp), %rax
	movq	%rax, -16(%rsp)
	movq	-16(%rsp), %rax
	retq
.Lfunc_end44:
	.size	_Z8fib_iteri, .Lfunc_end44-_Z8fib_iteri
	.cfi_endproc

	.globl	_Z12cfr_test_if1b
	.p2align	4, 0x90
	.type	_Z12cfr_test_if1b,@function
_Z12cfr_test_if1b:                      # @_Z12cfr_test_if1b
	.cfi_startproc
# BB#0:                                 # %fnbody
	pushq	%rax
.Ltmp10:
	.cfi_def_cfa_offset 16
	andb	$1, %dil
	movb	%dil, 7(%rsp)
	jne	.LBB45_2
# BB#1:                                 # %ifend
	movl	$1, %edi
	callq	exit
.LBB45_2:                               # %ret
	popq	%rax
	retq
.Lfunc_end45:
	.size	_Z12cfr_test_if1b, .Lfunc_end45-_Z12cfr_test_if1b
	.cfi_endproc

	.globl	_Z12cfr_test_if2b
	.p2align	4, 0x90
	.type	_Z12cfr_test_if2b,@function
_Z12cfr_test_if2b:                      # @_Z12cfr_test_if2b
	.cfi_startproc
# BB#0:                                 # %fnbody
	pushq	%rax
.Ltmp11:
	.cfi_def_cfa_offset 16
	andb	$1, %dil
	movb	%dil, 7(%rsp)
	jne	.LBB46_2
# BB#1:                                 # %ifend
	xorl	%edi, %edi
	callq	exit
	movl	$1, %edi
	callq	exit
.LBB46_2:                               # %ret
	popq	%rax
	retq
.Lfunc_end46:
	.size	_Z12cfr_test_if2b, .Lfunc_end46-_Z12cfr_test_if2b
	.cfi_endproc

	.globl	_Z12cfr_test_if3b
	.p2align	4, 0x90
	.type	_Z12cfr_test_if3b,@function
_Z12cfr_test_if3b:                      # @_Z12cfr_test_if3b
	.cfi_startproc
# BB#0:                                 # %fnbody
	pushq	%rax
.Ltmp12:
	.cfi_def_cfa_offset 16
	andb	$1, %dil
	movb	%dil, 7(%rsp)
	je	.LBB47_2
# BB#1:                                 # %ifend
	xorl	%edi, %edi
	callq	exit
	movl	$1, %edi
	callq	exit
.LBB47_2:                               # %ret
	popq	%rax
	retq
.Lfunc_end47:
	.size	_Z12cfr_test_if3b, .Lfunc_end47-_Z12cfr_test_if3b
	.cfi_endproc

	.globl	_Z12cfr_test_if4b
	.p2align	4, 0x90
	.type	_Z12cfr_test_if4b,@function
_Z12cfr_test_if4b:                      # @_Z12cfr_test_if4b
	.cfi_startproc
# BB#0:                                 # %ret
	andl	$1, %edi
	movb	%dil, -1(%rsp)
	retq
.Lfunc_end48:
	.size	_Z12cfr_test_if4b, .Lfunc_end48-_Z12cfr_test_if4b
	.cfi_endproc

	.globl	_Z12cfr_test_whlb
	.p2align	4, 0x90
	.type	_Z12cfr_test_whlb,@function
_Z12cfr_test_whlb:                      # @_Z12cfr_test_whlb
	.cfi_startproc
# BB#0:                                 # %whlhead
	pushq	%rax
.Ltmp13:
	.cfi_def_cfa_offset 16
	andb	$1, %dil
	movb	%dil, 7(%rsp)
	jne	.LBB49_2
# BB#1:                                 # %whlend
	movl	$1, %edi
	callq	exit
.LBB49_2:                               # %ret
	popq	%rax
	retq
.Lfunc_end49:
	.size	_Z12cfr_test_whlb, .Lfunc_end49-_Z12cfr_test_whlb
	.cfi_endproc

	.globl	_Z15cfr_test_do_whlb
	.p2align	4, 0x90
	.type	_Z15cfr_test_do_whlb,@function
_Z15cfr_test_do_whlb:                   # @_Z15cfr_test_do_whlb
	.cfi_startproc
# BB#0:                                 # %ret
	andl	$1, %edi
	movb	%dil, -1(%rsp)
	retq
.Lfunc_end50:
	.size	_Z15cfr_test_do_whlb, .Lfunc_end50-_Z15cfr_test_do_whlb
	.cfi_endproc

	.globl	_Z12cfr_test_forb
	.p2align	4, 0x90
	.type	_Z12cfr_test_forb,@function
_Z12cfr_test_forb:                      # @_Z12cfr_test_forb
	.cfi_startproc
# BB#0:                                 # %forhead
	pushq	%rax
.Ltmp14:
	.cfi_def_cfa_offset 16
	andb	$1, %dil
	movb	%dil, 7(%rsp)
	jne	.LBB51_2
# BB#1:                                 # %forend
	movl	$1, %edi
	callq	exit
.LBB51_2:                               # %ret
	popq	%rax
	retq
.Lfunc_end51:
	.size	_Z12cfr_test_forb, .Lfunc_end51-_Z12cfr_test_forb
	.cfi_endproc

	.globl	_Z8wrong_cfb
	.p2align	4, 0x90
	.type	_Z8wrong_cfb,@function
_Z8wrong_cfb:                           # @_Z8wrong_cfb
	.cfi_startproc
# BB#0:                                 # %entry
	andl	$1, %edi
	movb	%dil, -9(%rsp)
	movzbl	-9(%rsp), %eax
	cmpl	$1, %eax
	jne	.LBB52_2
# BB#1:                                 # %then
	movq	$42, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.LBB52_2:                               # %else
	movq	$2, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end52:
	.size	_Z8wrong_cfb, .Lfunc_end52-_Z8wrong_cfb
	.cfi_endproc

	.globl	_Z10asldfjasdfv
	.p2align	4, 0x90
	.type	_Z10asldfjasdfv,@function
_Z10asldfjasdfv:                        # @_Z10asldfjasdfv
	.cfi_startproc
# BB#0:                                 # %ret
	retq
.Lfunc_end53:
	.size	_Z10asldfjasdfv, .Lfunc_end53-_Z10asldfjasdfv
	.cfi_endproc

	.globl	_Z9wrong_cf2b
	.p2align	4, 0x90
	.type	_Z9wrong_cf2b,@function
_Z9wrong_cf2b:                          # @_Z9wrong_cf2b
	.cfi_startproc
# BB#0:                                 # %entry
	subq	$24, %rsp
.Ltmp15:
	.cfi_def_cfa_offset 32
	andl	$1, %edi
	movb	%dil, 15(%rsp)
	movzbl	15(%rsp), %eax
	cmpl	$1, %eax
	jne	.LBB54_3
# BB#1:                                 # %then
	movq	$42, 16(%rsp)
	jmp	.LBB54_2
.LBB54_3:                               # %else
	movl	$1, %edi
	callq	exit
	movq	$2222, 16(%rsp)         # imm = 0x8AE
.LBB54_2:                               # %ret
	movq	16(%rsp), %rax
	addq	$24, %rsp
	retq
.Lfunc_end54:
	.size	_Z9wrong_cf2b, .Lfunc_end54-_Z9wrong_cf2b
	.cfi_endproc

	.globl	_Z13cond_mv_test1i
	.p2align	4, 0x90
	.type	_Z13cond_mv_test1i,@function
_Z13cond_mv_test1i:                     # @_Z13cond_mv_test1i
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -16(%rsp)
	cmpq	$0, -16(%rsp)
	js	.LBB55_1
# BB#2:                                 # %else
	movq	-16(%rsp), %rax
	decq	%rax
	jmp	.LBB55_3
.LBB55_1:                               # %then
	movq	-16(%rsp), %rax
.LBB55_3:                               # %ret
	movq	%rax, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end55:
	.size	_Z13cond_mv_test1i, .Lfunc_end55-_Z13cond_mv_test1i
	.cfi_endproc

	.globl	_Z13cond_mv_test2i
	.p2align	4, 0x90
	.type	_Z13cond_mv_test2i,@function
_Z13cond_mv_test2i:                     # @_Z13cond_mv_test2i
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -16(%rsp)
	cmpq	$0, -16(%rsp)
	jle	.LBB56_2
# BB#1:                                 # %then
	decq	-16(%rsp)
.LBB56_2:                               # %ifend
	movq	-16(%rsp), %rax
	movq	%rax, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end56:
	.size	_Z13cond_mv_test2i, .Lfunc_end56-_Z13cond_mv_test2i
	.cfi_endproc

	.globl	_Z4opt1i
	.p2align	4, 0x90
	.type	_Z4opt1i,@function
_Z4opt1i:                               # @_Z4opt1i
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -16(%rsp)
	cmpq	$0, -16(%rsp)
	js	.LBB57_1
# BB#2:                                 # %else
	movq	-16(%rsp), %rax
	decq	%rax
	movq	%rax, -16(%rsp)
	jmp	.LBB57_3
.LBB57_1:                               # %then
	movq	-16(%rsp), %rax
.LBB57_3:                               # %ret
	movq	%rax, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end57:
	.size	_Z4opt1i, .Lfunc_end57-_Z4opt1i
	.cfi_endproc

	.globl	_Z4opt2i
	.p2align	4, 0x90
	.type	_Z4opt2i,@function
_Z4opt2i:                               # @_Z4opt2i
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -16(%rsp)
	cmpq	$0, -16(%rsp)
	jle	.LBB58_2
# BB#1:                                 # %then
	movq	-16(%rsp), %rax
	decq	%rax
	movq	%rax, -16(%rsp)
	jmp	.LBB58_3
.LBB58_2:                               # %else
	movq	-16(%rsp), %rax
.LBB58_3:                               # %ret
	movq	%rax, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end58:
	.size	_Z4opt2i, .Lfunc_end58-_Z4opt2i
	.cfi_endproc

	.globl	_Z8deadCodev
	.p2align	4, 0x90
	.type	_Z8deadCodev,@function
_Z8deadCodev:                           # @_Z8deadCodev
	.cfi_startproc
# BB#0:                                 # %entry
	movq	$0, -32(%rsp)
	movq	$0, -8(%rsp)
	movq	$0, -16(%rsp)
	movq	-32(%rsp), %rax
	movq	%rax, -40(%rsp)
	testq	%rax, %rax
	js	.LBB59_1
# BB#2:                                 # %else.i
	movq	-40(%rsp), %rax
	decq	%rax
	movq	%rax, -40(%rsp)
	jmp	.LBB59_3
.LBB59_1:                               # %then.i
	movq	-40(%rsp), %rax
.LBB59_3:                               # %_Z4opt1i.exit
	movq	%rax, -24(%rsp)
	movq	-32(%rsp), %rax
	movq	%rax, -40(%rsp)
	testq	%rax, %rax
	jle	.LBB59_5
# BB#4:                                 # %then.i8
	movq	-40(%rsp), %rax
	decq	%rax
	movq	%rax, -40(%rsp)
	jmp	.LBB59_6
.LBB59_5:                               # %else.i10
	movq	-40(%rsp), %rax
.LBB59_6:                               # %_Z4opt2i.exit
	movq	%rax, -24(%rsp)
	movq	$2, -32(%rsp)
	retq
.Lfunc_end59:
	.size	_Z8deadCodev, .Lfunc_end59-_Z8deadCodev
	.cfi_endproc

	.globl	_Z7retTestb
	.p2align	4, 0x90
	.type	_Z7retTestb,@function
_Z7retTestb:                            # @_Z7retTestb
	.cfi_startproc
# BB#0:                                 # %entry
	andl	$1, %edi
	movb	%dil, -9(%rsp)
	movzbl	-9(%rsp), %eax
	cmpl	$1, %eax
	jne	.LBB60_2
# BB#1:                                 # %then
	movq	$42, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.LBB60_2:                               # %else
	movq	$0, -8(%rsp)
	movq	-8(%rsp), %rax
	retq
.Lfunc_end60:
	.size	_Z7retTestb, .Lfunc_end60-_Z7retTestb
	.cfi_endproc

	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3
.LCPI61_0:
	.quad	4653144467747100426     # double 1234.5599999999999
	.text
	.globl	_Z10forvartestv
	.p2align	4, 0x90
	.type	_Z10forvartestv,@function
_Z10forvartestv:                        # @_Z10forvartestv
	.cfi_startproc
# BB#0:                                 # %fnbody
	subq	$24, %rsp
.Ltmp16:
	.cfi_def_cfa_offset 32
	movabsq	$4653144467747100426, %rax # imm = 0x40934A3D70A3D70A
	movq	%rax, 16(%rsp)
	movsd	.LCPI61_0(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	movsd	16(%rsp), %xmm0         # xmm0 = mem[0],zero
	callq	prtln_f
	movq	$0, 8(%rsp)
	jmp	.LBB61_1
	.p2align	4, 0x90
.LBB61_2:                               # %forloop
                                        #   in Loop: Header=BB61_1 Depth=1
	movq	8(%rsp), %rdi
	callq	prtln_i
	incq	8(%rsp)
.LBB61_1:                               # %forhead
                                        # =>This Inner Loop Header: Depth=1
	cmpq	$9, 8(%rsp)
	jle	.LBB61_2
# BB#3:                                 # %ret
	movsd	16(%rsp), %xmm0         # xmm0 = mem[0],zero
	callq	prtln_f
	movsd	16(%rsp), %xmm0         # xmm0 = mem[0],zero
	callq	prtln_f
	addq	$24, %rsp
	retq
.Lfunc_end61:
	.size	_Z10forvartestv, .Lfunc_end61-_Z10forvartestv
	.cfi_endproc

	.globl	_Z5errorb
	.p2align	4, 0x90
	.type	_Z5errorb,@function
_Z5errorb:                              # @_Z5errorb
	.cfi_startproc
# BB#0:                                 # %whlhead
	andb	$1, %dil
	movb	%dil, -9(%rsp)
	je	.LBB62_2
# BB#1:                                 # %whlloop
	movb	-9(%rsp), %al
	testb	%al, %al
	je	.LBB62_2
# BB#4:                                 # %then
	movabsq	$4614253070214989087, %rax # imm = 0x40091EB851EB851F
	jmp	.LBB62_3
.LBB62_2:                               # %whlend
	movabsq	$4613937818241073152, %rax # imm = 0x4008000000000000
.LBB62_3:                               # %ret
	movq	%rax, -8(%rsp)
	movsd	-8(%rsp), %xmm0         # xmm0 = mem[0],zero
	retq
.Lfunc_end62:
	.size	_Z5errorb, .Lfunc_end62-_Z5errorb
	.cfi_endproc

	.globl	_Z10noJmpErrorb
	.p2align	4, 0x90
	.type	_Z10noJmpErrorb,@function
_Z10noJmpErrorb:                        # @_Z10noJmpErrorb
	.cfi_startproc
# BB#0:                                 # %entry
	andl	$1, %edi
	movb	%dil, -1(%rsp)
	.p2align	4, 0x90
.LBB63_1:                               # %whlhead
                                        # =>This Inner Loop Header: Depth=1
	movb	-1(%rsp), %al
	testb	%al, %al
	jne	.LBB63_1
# BB#2:                                 # %ret
	retq
.Lfunc_end63:
	.size	_Z10noJmpErrorb, .Lfunc_end63-_Z10noJmpErrorb
	.cfi_endproc

	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3
.LCPI64_0:
	.quad	4631107791820423168     # double 42
	.text
	.globl	_Z2__v
	.p2align	4, 0x90
	.type	_Z2__v,@function
_Z2__v:                                 # @_Z2__v
	.cfi_startproc
# BB#0:                                 # %ret
	movabsq	$4631107791820423168, %rax # imm = 0x4045000000000000
	movq	%rax, -8(%rsp)
	movsd	.LCPI64_0(%rip), %xmm0  # xmm0 = mem[0],zero
	retq
.Lfunc_end64:
	.size	_Z2__v, .Lfunc_end64-_Z2__v
	.cfi_endproc

	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3
.LCPI65_0:
	.quad	4608083138725491507     # double 1.2
.LCPI65_1:
	.quad	4614253070214989087     # double 3.1400000000000001
.LCPI65_2:
	.quad	4631141568817628447     # double 42.240000000000002
.LCPI65_3:
	.quad	9214871658872686752     # double 1.0E+308
	.text
	.globl	_Z13print_doublesv
	.p2align	4, 0x90
	.type	_Z13print_doublesv,@function
_Z13print_doublesv:                     # @_Z13print_doublesv
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp17:
	.cfi_def_cfa_offset 16
	movsd	.LCPI65_0(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	xorpd	%xmm0, %xmm0
	callq	prtln_f
	movsd	.LCPI65_1(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	movsd	.LCPI65_2(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	movsd	.LCPI65_3(%rip), %xmm0  # xmm0 = mem[0],zero
	callq	prtln_f
	popq	%rax
	retq
.Lfunc_end65:
	.size	_Z13print_doublesv, .Lfunc_end65-_Z13print_doublesv
	.cfi_endproc

	.type	.Lstr_const,@object     # @str_const
	.section	.rodata.str1.1,"aMS",@progbits,1
.Lstr_const:
	.asciz	"yo"
	.size	.Lstr_const, 3

	.type	.Lstr_const.1,@object   # @str_const.1
.Lstr_const.1:
	.asciz	"samestring"
	.size	.Lstr_const.1, 11

	.type	.Lstr_const.2,@object   # @str_const.2
.Lstr_const.2:
	.asciz	"hi there\n"
	.size	.Lstr_const.2, 10

	.type	.Lstr_const.3,@object   # @str_const.3
.Lstr_const.3:
	.asciz	"--<"
	.size	.Lstr_const.3, 4

	.type	.Lstr_const.4,@object   # @str_const.4
.Lstr_const.4:
	.asciz	">--"
	.size	.Lstr_const.4, 4

	.type	.Lstr_const.5,@object   # @str_const.5
.Lstr_const.5:
	.asciz	" <-> "
	.size	.Lstr_const.5, 6


	.section	".note.GNU-stack","",@progbits
