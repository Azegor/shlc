; ModuleID = 'shl_global_module'
source_filename = "shl_global_module"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"

@str_const = private unnamed_addr constant [3 x i8] c"yo\00"
@str_const.1 = private unnamed_addr constant [11 x i8] c"samestring\00"
@str_const.2 = private unnamed_addr constant [10 x i8] c"hi there\0A\00"
@str_const.3 = private unnamed_addr constant [4 x i8] c"--<\00"
@str_const.4 = private unnamed_addr constant [4 x i8] c">--\00"
@str_const.5 = private unnamed_addr constant [6 x i8] c" <-> \00"

declare void @prt_i(i64)

declare void @prt_w_i(i64, i64)

declare void @prtln_i(i64)

declare void @prt_f(double)

declare void @prt_w_f(double, double)

declare void @prtln_f(double)

declare void @prt_c(i8)

declare void @prtln_c(i8)

declare void @prt_b(i1)

declare void @prtln_b(i1)

declare void @prt_s(i8*)

declare void @prtln_s(i8*)

define void @_Z2hiv() {
entry:
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  call void @prt_c(i8 72)
  call void @prt_c(i8 105)
  call void @prt_c(i8 10)
  br label %ret
}

define void @_Z4loopv() {
entry:
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

define double @_Z12testasdfasdfi(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  store i64 767, i64* %i_alloca
  %i1 = load i64, i64* %i_alloca
  %i1_cst_flt_t = sitofp i64 %i1 to double
  store double %i1_cst_flt_t, double* %retval
  br label %ret
}

define void @_Z2yov() {
entry:
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

define void @_Z4testi(i64 %a) {
entry:
  %a_alloca = alloca i64
  store i64 %a, i64* %a_alloca
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

declare void @exit(i64)

define void @_Z3blaii(i64 %a, i64 %b) {
entry:
  %a_alloca = alloca i64
  store i64 %a, i64* %a_alloca
  %b_alloca = alloca i64
  store i64 %b, i64* %b_alloca
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  call void @exit(i64 1)
  br label %ret
}

define void @_Z4bla2iiff(i64 %a, i64 %b, double %c, double %d) {
entry:
  %a_alloca.i = alloca i64
  %a_alloca = alloca i64
  store i64 %a, i64* %a_alloca
  %b_alloca = alloca i64
  store i64 %b, i64* %b_alloca
  %c_alloca = alloca double
  store double %c, double* %c_alloca
  %d_alloca = alloca double
  store double %d, double* %d_alloca
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  %0 = bitcast i64* %a_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %0)
  store i64 2, i64* %a_alloca.i
  %1 = bitcast i64* %a_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %1)
  br label %ret

; uselistorder directives
  uselistorder i64* %a_alloca.i, { 0, 2, 1 }
}

define void @_Z16whatever_fn_nameifffcccccb(i64 %a, double %b, double %c, double %d, i8 %e, i8 %f, i8 %g, i8 %h, i8 %i, i1 %l) {
entry:
  %a_alloca = alloca i64
  store i64 %a, i64* %a_alloca
  %b_alloca = alloca double
  store double %b, double* %b_alloca
  %c_alloca = alloca double
  store double %c, double* %c_alloca
  %d_alloca = alloca double
  store double %d, double* %d_alloca
  %e_alloca = alloca i8
  store i8 %e, i8* %e_alloca
  %f_alloca = alloca i8
  store i8 %f, i8* %f_alloca
  %g_alloca = alloca i8
  store i8 %g, i8* %g_alloca
  %h_alloca = alloca i8
  store i8 %h, i8* %h_alloca
  %i_alloca = alloca i8
  store i8 %i, i8* %i_alloca
  %l_alloca = alloca i1
  store i1 %l, i1* %l_alloca
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

declare void @_Z11declarationv()

declare void @_Z5decl2iiff(i64, i64, double, double)

declare void @_Z5decl3v()

declare void @nativeFunction()

declare void @ANF(i64, i64, i64, i64, i64, i64, i1, i1, i1, i1, i1, i1)

declare void @sin(double)

declare void @cos(double)

declare void @tan(double)

define double @_Z7complexib(i64 %i, i1 %b) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca double
  %res_alloca = alloca double
  %i_alloca2 = alloca i64
  %j_alloca = alloca i64
  br label %fnbody

ret:                                              ; preds = %ifend
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  store double 0.000000e+00, double* %res_alloca
  store double 1.204000e+03, double* %res_alloca
  br label %whlhead

whlhead:                                          ; preds = %whlloop, %fnbody
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %whlloop, label %whlend

whlloop:                                          ; preds = %whlhead
  store double 4.200000e+01, double* %res_alloca
  br label %whlhead

whlend:                                           ; preds = %whlhead
  store i64 0, i64* %i_alloca2
  br label %doloop

doloop:                                           ; preds = %dohead, %whlend
  store i64 7, i64* %i_alloca2
  br label %dohead

dohead:                                           ; preds = %doloop
  br i1 false, label %doloop, label %doend

doend:                                            ; preds = %dohead
  store i64 0, i64* %j_alloca
  store i64 0, i64* %j_alloca
  br label %forhead

forhead:                                          ; preds = %forloop, %doend
  %j = load i64, i64* %j_alloca
  %cmp_lt = icmp slt i64 %j, 7
  br i1 %cmp_lt, label %forloop, label %forend

forloop:                                          ; preds = %forhead
  %i3 = load i64, i64* %i_alloca2
  %sum = add i64 %i3, 2
  store i64 %sum, i64* %i_alloca2
  %j4 = load i64, i64* %j_alloca
  %sum5 = add i64 %j4, 1
  store i64 %sum5, i64* %j_alloca
  br label %forhead

forend:                                           ; preds = %forhead
  %b6 = load i1, i1* %b_alloca
  br i1 %b6, label %then, label %ifend

then:                                             ; preds = %forend
  store double 1.200000e+02, double* %res_alloca
  br label %ifend

ifend:                                            ; preds = %then, %forend
  %res = load double, double* %res_alloca
  store double %res, double* %retval
  br label %ret
}

define double @_Z6doublev() {
entry:
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  store double 1.234500e+00, double* %retval
  br label %ret
}

define i64 @_Z7integerv() {
entry:
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  store i64 123, i64* %retval
  br label %ret
}

define i1 @_Z7booleanv() {
entry:
  %retval = alloca i1
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i1, i1* %retval
  ret i1 %0

fnbody:                                           ; preds = %entry
  store i1 true, i1* %retval
  br label %ret
}

define i8 @_Z4charv() {
entry:
  %retval = alloca i8
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i8, i8* %retval
  ret i8 %0

fnbody:                                           ; preds = %entry
  store i8 99, i8* %retval
  br label %ret
}

define void @_Z9duplicatev() {
entry:
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

define double @_Z6doublef(double %f) {
entry:
  %f_alloca = alloca double
  store double %f, double* %f_alloca
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  %f1 = load double, double* %f_alloca
  store double %f1, double* %retval
  br label %ret
}

define void @_Z10double_f_fv() {
entry:
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

define i64 @_Z11returnParami(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  store i64 %i1, i64* %retval
  br label %ret
}

define void @_Z10whlVarTestbif(i1 %b, i64 %i, double %f) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %f_alloca = alloca double
  store double %f, double* %f_alloca
  br label %fnbody

ret:                                              ; preds = %whlend8, %whlloop7, %whlloop3, %whlloop
  ret void

fnbody:                                           ; preds = %entry
  br label %whlhead

whlhead:                                          ; preds = %fnbody
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %whlloop, label %whlend

whlloop:                                          ; preds = %whlhead
  br label %ret

whlend:                                           ; preds = %whlhead
  br label %whlhead2

whlhead2:                                         ; preds = %whlend
  %i5 = load i64, i64* %i_alloca
  %i5_cst_boo_t = icmp ne i64 %i5, 0
  br i1 %i5_cst_boo_t, label %whlloop3, label %whlend4

whlloop3:                                         ; preds = %whlhead2
  br label %ret

whlend4:                                          ; preds = %whlhead2
  br label %whlhead6

whlhead6:                                         ; preds = %whlend4
  %f9 = load double, double* %f_alloca
  %f9_cst_boo_t = fcmp one double %f9, 0.000000e+00
  br i1 %f9_cst_boo_t, label %whlloop7, label %whlend8

whlloop7:                                         ; preds = %whlhead6
  br label %ret

whlend8:                                          ; preds = %whlhead6
  br label %ret
}

define double @_Z12int_flt_casti(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %i1_cst_flt_t = sitofp i64 %i1 to double
  store double %i1_cst_flt_t, double* %retval
  br label %ret
}

define i64 @_Z12flt_int_castf(double %f) {
entry:
  %f_alloca = alloca double
  store double %f, double* %f_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %f1 = load double, double* %f_alloca
  %f1_cst_int_t = fptosi double %f1 to i64
  store i64 %f1_cst_int_t, i64* %retval
  br label %ret
}

define i8 @_Z12int_chr_casti(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca i8
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i8, i8* %retval
  ret i8 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %i1_cst_chr_t = trunc i64 %i1 to i8
  store i8 %i1_cst_chr_t, i8* %retval
  br label %ret
}

define double @_Z12chr_flt_castc(i8 %c) {
entry:
  %c_alloca = alloca i8
  store i8 %c, i8* %c_alloca
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  %c1 = load i8, i8* %c_alloca
  %c1_cst_flt_t = sitofp i8 %c1 to double
  store double %c1_cst_flt_t, double* %retval
  br label %ret
}

define i64 @_Z12chr_int_castc(i8 %c) {
entry:
  %c_alloca = alloca i8
  store i8 %c, i8* %c_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %c1 = load i8, i8* %c_alloca
  %c1_cst_int_t = sext i8 %c1 to i64
  store i64 %c1_cst_int_t, i64* %retval
  br label %ret
}

define i64 @_Z12boo_int_castb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  %b1_cst_int_t = sext i1 %b1 to i64
  store i64 %b1_cst_int_t, i64* %retval
  br label %ret
}

define double @_Z12boo_flt_castb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  %b1_cst_flt_t = sitofp i1 %b1 to double
  store double %b1_cst_flt_t, double* %retval
  br label %ret
}

define i1 @_Z12chr_boo_castc(i8 %c) {
entry:
  %c_alloca = alloca i8
  store i8 %c, i8* %c_alloca
  %retval = alloca i1
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i1, i1* %retval
  ret i1 %0

fnbody:                                           ; preds = %entry
  %c1 = load i8, i8* %c_alloca
  %c1_cst_boo_t = icmp ne i8 %c1, 0
  store i1 %c1_cst_boo_t, i1* %retval
  br label %ret
}

define i1 @_Z12flt_boo_castf(double %f) {
entry:
  %f_alloca = alloca double
  store double %f, double* %f_alloca
  %retval = alloca i1
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i1, i1* %retval
  ret i1 %0

fnbody:                                           ; preds = %entry
  %f1 = load double, double* %f_alloca
  %f1_cst_boo_t = fcmp one double %f1, 0.000000e+00
  store i1 %f1_cst_boo_t, i1* %retval
  br label %ret
}

define i64 @_Z11assign_testiii(i64 %i, i64 %j, i64 %k) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %j_alloca = alloca i64
  store i64 %j, i64* %j_alloca
  %k_alloca = alloca i64
  store i64 %k, i64* %k_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %k1 = load i64, i64* %k_alloca
  store i64 %k1, i64* %i_alloca
  %j2 = load i64, i64* %j_alloca
  %i3 = load i64, i64* %i_alloca
  %sum = add i64 %i3, %j2
  store i64 %sum, i64* %i_alloca
  %k4 = load i64, i64* %k_alloca
  %j5 = load i64, i64* %j_alloca
  %prod = mul i64 %j5, %k4
  %i6 = load i64, i64* %i_alloca
  %sum7 = add i64 %i6, %prod
  store i64 %sum7, i64* %k_alloca
  %k8 = load i64, i64* %k_alloca
  store i64 %k8, i64* %retval
  br label %ret
}

define i64 @_Z12chain_assigniii(i64 %i, i64 %j, i64 %k) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %j_alloca = alloca i64
  store i64 %j, i64* %j_alloca
  %k_alloca = alloca i64
  store i64 %k, i64* %k_alloca
  %retval = alloca i64
  %e_alloca = alloca i64
  %a_alloca = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  store i64 0, i64* %e_alloca
  store i64 0, i64* %a_alloca
  %k1 = load i64, i64* %k_alloca
  store i64 %k1, i64* %j_alloca
  store i64 %k1, i64* %i_alloca
  store i64 %k1, i64* %e_alloca
  %k2 = load i64, i64* %k_alloca
  %j3 = load i64, i64* %j_alloca
  %sum = add i64 %j3, %k2
  store i64 %sum, i64* %j_alloca
  %i4 = load i64, i64* %i_alloca
  %sum5 = add i64 %i4, %sum
  store i64 %sum5, i64* %i_alloca
  %a = load i64, i64* %a_alloca
  %sum6 = add i64 %a, %sum5
  store i64 %sum6, i64* %a_alloca
  %a7 = load i64, i64* %a_alloca
  store i64 %a7, i64* %retval
  br label %ret
}

define double @_Z10local_varsb(i1 %bl) {
entry:
  %bl_alloca = alloca i1
  store i1 %bl, i1* %bl_alloca
  %retval = alloca double
  %a_alloca = alloca i64
  %b_alloca = alloca i8
  %c_alloca = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  store i64 42, i64* %a_alloca
  store i8 99, i8* %b_alloca
  store double 4.500000e+00, double* %c_alloca
  %c = load double, double* %c_alloca
  %bl1 = load i1, i1* %bl_alloca
  %bl1_cst_flt_t = sitofp i1 %bl1 to double
  %prod = fmul double %bl1_cst_flt_t, %c
  %b = load i8, i8* %b_alloca
  %bl2 = load i1, i1* %bl_alloca
  %bl2_cst_chr_t = sext i1 %bl2 to i8
  %prod3 = mul i8 %bl2_cst_chr_t, %b
  %prod3_cst_int_t = sext i8 %prod3 to i64
  %a = load i64, i64* %a_alloca
  %bl4 = load i1, i1* %bl_alloca
  %bl4_cst_int_t = sext i1 %bl4 to i64
  %prod5 = mul i64 %bl4_cst_int_t, %a
  %sum = add i64 %prod5, 1
  %prod6 = mul i64 %sum, %prod3_cst_int_t
  %prod6_cst_flt_t = sitofp i64 %prod6 to double
  %sum7 = fadd double %prod6_cst_flt_t, %prod
  store double %sum7, double* %retval
  br label %ret
}

define void @_Z10break_testb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %whlend
  ret void

fnbody:                                           ; preds = %entry
  br label %whlhead

whlhead:                                          ; preds = %fnbody
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %whlloop, label %whlend

whlloop:                                          ; preds = %whlhead
  br label %whlend

whlend:                                           ; preds = %whlloop, %whlhead
  br label %ret
}

define void @_Z7if_testb(i1 %b) {
entry:
  %b_alloca.i = alloca i1
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %doloop10, %else3
  ret void

fnbody:                                           ; preds = %entry
  br label %doloop

doloop:                                           ; preds = %dohead, %fnbody
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %else

dohead:                                           ; preds = %then
  %b6 = load i1, i1* %b_alloca
  br i1 %b6, label %doloop, label %doend

doend:                                            ; preds = %dohead, %then2
  br label %doloop7

then:                                             ; preds = %doloop
  br label %dohead

else:                                             ; preds = %doloop
  %b5 = load i1, i1* %b_alloca
  %not = xor i1 %b5, true
  br i1 %not, label %then2, label %else3

then2:                                            ; preds = %else
  br label %doend

else3:                                            ; preds = %else
  br label %ret

doloop7:                                          ; preds = %dohead8, %doend
  %0 = bitcast i1* %b_alloca.i to i8*
  call void @llvm.lifetime.start(i64 1, i8* %0)
  store i1 true, i1* %b_alloca.i
  %b1.i = load i1, i1* %b_alloca.i
  br i1 %b1.i, label %whlloop.i, label %_Z10break_testb.exit

whlloop.i:                                        ; preds = %doloop7
  br label %_Z10break_testb.exit

_Z10break_testb.exit:                             ; preds = %doloop7, %whlloop.i
  %1 = bitcast i1* %b_alloca.i to i8*
  call void @llvm.lifetime.end(i64 1, i8* %1)
  br label %dohead8

dohead8:                                          ; preds = %_Z10break_testb.exit
  br i1 true, label %doloop7, label %doend9

doend9:                                           ; preds = %dohead8
  br label %doloop10

doloop10:                                         ; preds = %doend9
  br label %ret

; uselistorder directives
  uselistorder i1* %b_alloca, { 1, 0, 2, 3 }
  uselistorder i1* %b_alloca.i, { 0, 2, 3, 1 }
  uselistorder label %_Z10break_testb.exit, { 1, 0 }
  uselistorder label %doend, { 1, 0 }
}

define i64 @_Z12test_bitcompicb(i64 %i, i8 %c, i1 %b) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %c_alloca = alloca i8
  store i8 %c, i8* %c_alloca
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %b2 = load i1, i1* %b_alloca
  %bitcmpl = xor i1 %b2, true
  %bitcmpl_cst_int_t = sext i1 %bitcmpl to i64
  %bitor = or i64 %bitcmpl_cst_int_t, %i1
  %b3 = load i1, i1* %b_alloca
  %b3_cst_chr_t = sext i1 %b3 to i8
  %c4 = load i8, i8* %c_alloca
  %bitcmpl5 = xor i8 %c4, -1
  %bitand = and i8 %bitcmpl5, %b3_cst_chr_t
  %bitand_cst_int_t = sext i8 %bitand to i64
  %bitxor = xor i64 %bitand_cst_int_t, %bitor
  %i6 = load i64, i64* %i_alloca
  %bitcmpl7 = xor i64 %i6, -1
  %bitor8 = or i64 %bitcmpl7, %bitxor
  store i64 %bitor8, i64* %retval
  br label %ret
}

define i8* @_Z10stringtestv() {
entry:
  %retval = alloca i8*
  %s_alloca = alloca i8*
  %s3_alloca = alloca i8*
  %s2_alloca = alloca i8*
  br label %fnbody

ret:                                              ; preds = %fnbody
  %0 = load i8*, i8** %retval
  ret i8* %0

fnbody:                                           ; preds = %entry
  store i8* getelementptr inbounds ([3 x i8], [3 x i8]* @str_const, i32 0, i32 0), i8** %s_alloca
  store i8* getelementptr inbounds ([11 x i8], [11 x i8]* @str_const.1, i32 0, i32 0), i8** %s3_alloca
  store i8* getelementptr inbounds ([11 x i8], [11 x i8]* @str_const.1, i32 0, i32 0), i8** %s2_alloca
  %s = load i8*, i8** %s_alloca
  store i8* %s, i8** %retval
  br label %ret
}

define void @_Z12print_stringv() {
entry:
  %i_alloca = alloca i64
  br label %fnbody

ret:                                              ; preds = %forend
  ret void

fnbody:                                           ; preds = %entry
  call void @prt_s(i8* getelementptr inbounds ([10 x i8], [10 x i8]* @str_const.2, i32 0, i32 0))
  store i64 0, i64* %i_alloca
  store i64 0, i64* %i_alloca
  br label %forhead

forhead:                                          ; preds = %forloop, %fnbody
  %i = load i64, i64* %i_alloca
  %cmp_lt = icmp slt i64 %i, 100
  br i1 %cmp_lt, label %forloop, label %forend

forloop:                                          ; preds = %forhead
  call void @prt_s(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str_const.3, i32 0, i32 0))
  %i1 = load i64, i64* %i_alloca
  %i2 = load i64, i64* %i_alloca
  %prod = mul i64 %i2, %i1
  call void @prt_i(i64 %prod)
  call void @prt_s(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str_const.4, i32 0, i32 0))
  %i3 = load i64, i64* %i_alloca
  %sum = add i64 %i3, 1
  store i64 %sum, i64* %i_alloca
  br label %forhead

forend:                                           ; preds = %forhead
  call void @prt_c(i8 10)
  br label %ret
}

define void @_Z16print_mult_tablev() {
entry:
  %i_alloca = alloca i64
  %j_alloca = alloca i64
  br label %fnbody

ret:                                              ; preds = %forend
  ret void

fnbody:                                           ; preds = %entry
  store i64 0, i64* %i_alloca
  store i64 0, i64* %j_alloca
  store i64 1, i64* %i_alloca
  br label %forhead

forhead:                                          ; preds = %forend3, %fnbody
  %i = load i64, i64* %i_alloca
  %cmp_lte = icmp sle i64 %i, 10
  br i1 %cmp_lte, label %forloop, label %forend

forloop:                                          ; preds = %forhead
  store i64 1, i64* %j_alloca
  br label %forhead1

forend:                                           ; preds = %forhead
  br label %ret

forhead1:                                         ; preds = %forloop2, %forloop
  %j = load i64, i64* %j_alloca
  %cmp_lte4 = icmp sle i64 %j, 10
  br i1 %cmp_lte4, label %forloop2, label %forend3

forloop2:                                         ; preds = %forhead1
  %j5 = load i64, i64* %j_alloca
  %i6 = load i64, i64* %i_alloca
  %prod = mul i64 %i6, %j5
  call void @prt_w_i(i64 %prod, i64 3)
  call void @prt_c(i8 32)
  %j7 = load i64, i64* %j_alloca
  %sum = add i64 %j7, 1
  store i64 %sum, i64* %j_alloca
  br label %forhead1

forend3:                                          ; preds = %forhead1
  call void @prt_c(i8 10)
  %i8 = load i64, i64* %i_alloca
  %sum9 = add i64 %i8, 1
  store i64 %sum9, i64* %i_alloca
  br label %forhead
}

define void @_Z8test_fibv() {
entry:
  %val_alloca.i14 = alloca i64
  %retval.i15 = alloca i64
  %fib_1_alloca.i16 = alloca i64
  %fib_2_alloca.i17 = alloca i64
  %res_alloca.i18 = alloca i64
  %i_alloca.i19 = alloca i64
  %val_alloca.i = alloca i64
  %retval.i = alloca i64
  %fib_1_alloca.i = alloca i64
  %fib_2_alloca.i = alloca i64
  %res_alloca.i = alloca i64
  %i_alloca.i = alloca i64
  %i_alloca = alloca i64
  br label %fnbody

ret:                                              ; preds = %forend7
  ret void

fnbody:                                           ; preds = %entry
  store i64 0, i64* %i_alloca
  store i64 0, i64* %i_alloca
  br label %forhead

forhead:                                          ; preds = %_Z8fib_iteri.exit, %fnbody
  %i = load i64, i64* %i_alloca
  %cmp_lt = icmp slt i64 %i, 35
  br i1 %cmp_lt, label %forloop, label %forend

forloop:                                          ; preds = %forhead
  %i1 = load i64, i64* %i_alloca
  %callres = call i64 @_Z3fibi(i64 %i1)
  call void @prt_i(i64 %callres)
  call void @prt_s(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str_const.5, i32 0, i32 0))
  %i2 = load i64, i64* %i_alloca
  %0 = bitcast i64* %val_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %0)
  %1 = bitcast i64* %retval.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %1)
  %2 = bitcast i64* %fib_1_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %2)
  %3 = bitcast i64* %fib_2_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %3)
  %4 = bitcast i64* %res_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %4)
  %5 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %5)
  store i64 %i2, i64* %val_alloca.i
  %val1.i = load i64, i64* %val_alloca.i
  %cmp_lt.i = icmp slt i64 %val1.i, 3
  br i1 %cmp_lt.i, label %then.i, label %ifend.i

then.i:                                           ; preds = %forloop
  store i64 1, i64* %retval.i
  br label %_Z8fib_iteri.exit

ifend.i:                                          ; preds = %forloop
  store i64 1, i64* %fib_1_alloca.i
  store i64 1, i64* %fib_2_alloca.i
  store i64 0, i64* %res_alloca.i
  store i64 0, i64* %i_alloca.i
  store i64 3, i64* %i_alloca.i
  br label %forhead.i

forhead.i:                                        ; preds = %forloop.i, %ifend.i
  %val2.i = load i64, i64* %val_alloca.i
  %i.i = load i64, i64* %i_alloca.i
  %cmp_lte.i = icmp sle i64 %i.i, %val2.i
  br i1 %cmp_lte.i, label %forloop.i, label %forend.i

forloop.i:                                        ; preds = %forhead.i
  %fib_2.i = load i64, i64* %fib_2_alloca.i
  %fib_1.i = load i64, i64* %fib_1_alloca.i
  %sum.i = add i64 %fib_1.i, %fib_2.i
  store i64 %sum.i, i64* %res_alloca.i
  %fib_13.i = load i64, i64* %fib_1_alloca.i
  store i64 %fib_13.i, i64* %fib_2_alloca.i
  %res.i = load i64, i64* %res_alloca.i
  store i64 %res.i, i64* %fib_1_alloca.i
  %i4.i = load i64, i64* %i_alloca.i
  %sum5.i = add i64 %i4.i, 1
  store i64 %sum5.i, i64* %i_alloca.i
  br label %forhead.i

forend.i:                                         ; preds = %forhead.i
  %res6.i = load i64, i64* %res_alloca.i
  store i64 %res6.i, i64* %retval.i
  br label %_Z8fib_iteri.exit

_Z8fib_iteri.exit:                                ; preds = %then.i, %forend.i
  %6 = load i64, i64* %retval.i
  %7 = bitcast i64* %val_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %7)
  %8 = bitcast i64* %retval.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %8)
  %9 = bitcast i64* %fib_1_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %9)
  %10 = bitcast i64* %fib_2_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %10)
  %11 = bitcast i64* %res_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %11)
  %12 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %12)
  call void @prtln_i(i64 %6)
  %i4 = load i64, i64* %i_alloca
  %sum = add i64 %i4, 1
  store i64 %sum, i64* %i_alloca
  br label %forhead

forend:                                           ; preds = %forhead
  store i64 0, i64* %i_alloca
  br label %forhead5

forhead5:                                         ; preds = %_Z8fib_iteri.exit38, %forend
  %i8 = load i64, i64* %i_alloca
  %cmp_lt9 = icmp slt i64 %i8, 93
  br i1 %cmp_lt9, label %forloop6, label %forend7

forloop6:                                         ; preds = %forhead5
  %i10 = load i64, i64* %i_alloca
  %13 = bitcast i64* %val_alloca.i14 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %13)
  %14 = bitcast i64* %retval.i15 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %14)
  %15 = bitcast i64* %fib_1_alloca.i16 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %15)
  %16 = bitcast i64* %fib_2_alloca.i17 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %16)
  %17 = bitcast i64* %res_alloca.i18 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %17)
  %18 = bitcast i64* %i_alloca.i19 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %18)
  store i64 %i10, i64* %val_alloca.i14
  %val1.i20 = load i64, i64* %val_alloca.i14
  %cmp_lt.i21 = icmp slt i64 %val1.i20, 3
  br i1 %cmp_lt.i21, label %then.i22, label %ifend.i23

then.i22:                                         ; preds = %forloop6
  store i64 1, i64* %retval.i15
  br label %_Z8fib_iteri.exit38

ifend.i23:                                        ; preds = %forloop6
  store i64 1, i64* %fib_1_alloca.i16
  store i64 1, i64* %fib_2_alloca.i17
  store i64 0, i64* %res_alloca.i18
  store i64 0, i64* %i_alloca.i19
  store i64 3, i64* %i_alloca.i19
  br label %forhead.i27

forhead.i27:                                      ; preds = %forloop.i35, %ifend.i23
  %val2.i24 = load i64, i64* %val_alloca.i14
  %i.i25 = load i64, i64* %i_alloca.i19
  %cmp_lte.i26 = icmp sle i64 %i.i25, %val2.i24
  br i1 %cmp_lte.i26, label %forloop.i35, label %forend.i37

forloop.i35:                                      ; preds = %forhead.i27
  %fib_2.i28 = load i64, i64* %fib_2_alloca.i17
  %fib_1.i29 = load i64, i64* %fib_1_alloca.i16
  %sum.i30 = add i64 %fib_1.i29, %fib_2.i28
  store i64 %sum.i30, i64* %res_alloca.i18
  %fib_13.i31 = load i64, i64* %fib_1_alloca.i16
  store i64 %fib_13.i31, i64* %fib_2_alloca.i17
  %res.i32 = load i64, i64* %res_alloca.i18
  store i64 %res.i32, i64* %fib_1_alloca.i16
  %i4.i33 = load i64, i64* %i_alloca.i19
  %sum5.i34 = add i64 %i4.i33, 1
  store i64 %sum5.i34, i64* %i_alloca.i19
  br label %forhead.i27

forend.i37:                                       ; preds = %forhead.i27
  %res6.i36 = load i64, i64* %res_alloca.i18
  store i64 %res6.i36, i64* %retval.i15
  br label %_Z8fib_iteri.exit38

_Z8fib_iteri.exit38:                              ; preds = %then.i22, %forend.i37
  %19 = load i64, i64* %retval.i15
  %20 = bitcast i64* %val_alloca.i14 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %20)
  %21 = bitcast i64* %retval.i15 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %21)
  %22 = bitcast i64* %fib_1_alloca.i16 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %22)
  %23 = bitcast i64* %fib_2_alloca.i17 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %23)
  %24 = bitcast i64* %res_alloca.i18 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %24)
  %25 = bitcast i64* %i_alloca.i19 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %25)
  call void @prtln_i(i64 %19)
  %i12 = load i64, i64* %i_alloca
  %sum13 = add i64 %i12, 1
  store i64 %sum13, i64* %i_alloca
  br label %forhead5

forend7:                                          ; preds = %forhead5
  br label %ret

; uselistorder directives
  uselistorder i64* %i_alloca.i, { 0, 2, 3, 4, 5, 6, 1 }
  uselistorder i64* %res_alloca.i, { 0, 4, 2, 3, 5, 1 }
  uselistorder i64* %fib_2_alloca.i, { 0, 2, 3, 4, 1 }
  uselistorder i64* %fib_1_alloca.i, { 0, 2, 3, 4, 5, 1 }
  uselistorder i64* %retval.i, { 0, 3, 4, 2, 1 }
  uselistorder i64* %val_alloca.i, { 0, 2, 3, 4, 1 }
  uselistorder i64* %i_alloca.i19, { 0, 2, 3, 4, 5, 6, 1 }
  uselistorder i64* %res_alloca.i18, { 0, 4, 2, 3, 5, 1 }
  uselistorder i64* %fib_2_alloca.i17, { 0, 2, 3, 4, 1 }
  uselistorder i64* %fib_1_alloca.i16, { 0, 2, 3, 4, 5, 1 }
  uselistorder i64* %retval.i15, { 0, 3, 4, 2, 1 }
  uselistorder i64* %val_alloca.i14, { 0, 2, 3, 4, 1 }
  uselistorder label %_Z8fib_iteri.exit38, { 1, 0 }
  uselistorder label %_Z8fib_iteri.exit, { 1, 0 }
}

define void @_Z9testStuffv() {
entry:
  %b_alloca.i = alloca i1
  %retval.i = alloca double
  br label %fnbody

ret:                                              ; preds = %_Z5errorb.exit
  ret void

fnbody:                                           ; preds = %entry
  call void @_Z10forvartestv()
  %0 = bitcast i1* %b_alloca.i to i8*
  call void @llvm.lifetime.start(i64 1, i8* %0)
  %1 = bitcast double* %retval.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %1)
  store i1 false, i1* %b_alloca.i
  %b1.i = load i1, i1* %b_alloca.i
  br i1 %b1.i, label %whlloop.i, label %whlend.i

whlloop.i:                                        ; preds = %fnbody
  %b2.i = load i1, i1* %b_alloca.i
  br i1 %b2.i, label %then.i, label %else.i

whlend.i:                                         ; preds = %else.i, %fnbody
  store double 3.000000e+00, double* %retval.i
  br label %_Z5errorb.exit

then.i:                                           ; preds = %whlloop.i
  store double 3.140000e+00, double* %retval.i
  br label %_Z5errorb.exit

else.i:                                           ; preds = %whlloop.i
  br label %whlend.i

_Z5errorb.exit:                                   ; preds = %whlend.i, %then.i
  %2 = load double, double* %retval.i
  %3 = bitcast i1* %b_alloca.i to i8*
  call void @llvm.lifetime.end(i64 1, i8* %3)
  %4 = bitcast double* %retval.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %4)
  call void @prtln_f(double 1.200000e+00)
  call void @prtln_f(double 0.000000e+00)
  call void @prtln_f(double 3.140000e+00)
  call void @prtln_f(double 4.224000e+01)
  call void @prtln_f(double 1.000000e+308)
  br label %ret

; uselistorder directives
  uselistorder double* %retval.i, { 0, 3, 2, 4, 1 }
  uselistorder i1* %b_alloca.i, { 0, 2, 3, 4, 1 }
  uselistorder label %_Z5errorb.exit, { 1, 0 }
}

define void @main() {
entry:
  %i_alloca.i = alloca i64
  %j_alloca.i = alloca i64
  br label %fnbody

ret:                                              ; preds = %_Z16print_mult_tablev.exit
  ret void

fnbody:                                           ; preds = %entry
  call void @prt_c(i8 72)
  call void @prt_c(i8 105)
  call void @prt_c(i8 10)
  call void @_Z12print_stringv()
  %0 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %0)
  %1 = bitcast i64* %j_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %1)
  store i64 0, i64* %i_alloca.i
  store i64 0, i64* %j_alloca.i
  store i64 1, i64* %i_alloca.i
  br label %forhead.i

forhead.i:                                        ; preds = %forend3.i, %fnbody
  %i.i = load i64, i64* %i_alloca.i
  %cmp_lte.i = icmp sle i64 %i.i, 10
  br i1 %cmp_lte.i, label %forloop.i, label %_Z16print_mult_tablev.exit

forloop.i:                                        ; preds = %forhead.i
  store i64 1, i64* %j_alloca.i
  br label %forhead1.i

forhead1.i:                                       ; preds = %forloop2.i, %forloop.i
  %j.i = load i64, i64* %j_alloca.i
  %cmp_lte4.i = icmp sle i64 %j.i, 10
  br i1 %cmp_lte4.i, label %forloop2.i, label %forend3.i

forloop2.i:                                       ; preds = %forhead1.i
  %j5.i = load i64, i64* %j_alloca.i
  %i6.i = load i64, i64* %i_alloca.i
  %prod.i = mul i64 %i6.i, %j5.i
  call void @prt_w_i(i64 %prod.i, i64 3)
  call void @prt_c(i8 32)
  %j7.i = load i64, i64* %j_alloca.i
  %sum.i = add i64 %j7.i, 1
  store i64 %sum.i, i64* %j_alloca.i
  br label %forhead1.i

forend3.i:                                        ; preds = %forhead1.i
  call void @prt_c(i8 10)
  %i8.i = load i64, i64* %i_alloca.i
  %sum9.i = add i64 %i8.i, 1
  store i64 %sum9.i, i64* %i_alloca.i
  br label %forhead.i

_Z16print_mult_tablev.exit:                       ; preds = %forhead.i
  %2 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %2)
  %3 = bitcast i64* %j_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %3)
  call void @_Z8test_fibv()
  call void @_Z9testStuffv()
  call void @exit(i64 0)
  br label %ret

; uselistorder directives
  uselistorder i64* %j_alloca.i, { 0, 2, 3, 4, 5, 6, 7, 1 }
  uselistorder i64* %i_alloca.i, { 0, 3, 4, 2, 5, 6, 7, 1 }
  uselistorder void (i8)* @prt_c, { 1, 0, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
}

define void @_Z8overloadi(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

define void @_Z8overloadf(double %f) {
entry:
  %f_alloca = alloca double
  store double %f, double* %f_alloca
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

define void @_Z13call_overloadv() {
entry:
  %f_alloca.i = alloca double
  %i_alloca.i = alloca i64
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  %0 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %0)
  store i64 1, i64* %i_alloca.i
  %1 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %1)
  %2 = bitcast double* %f_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %2)
  store double 0x400921FB54442D18, double* %f_alloca.i
  %3 = bitcast double* %f_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %3)
  br label %ret

; uselistorder directives
  uselistorder i64* %i_alloca.i, { 0, 2, 1 }
  uselistorder double* %f_alloca.i, { 0, 2, 1 }
}

define void @_Z8flt_callf(double %f) {
entry:
  %f_alloca = alloca double
  store double %f, double* %f_alloca
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  br label %ret
}

define void @_Z13call_imp_castv() {
entry:
  %f_alloca.i = alloca double
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  %0 = bitcast double* %f_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %0)
  store double 0.000000e+00, double* %f_alloca.i
  %1 = bitcast double* %f_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %1)
  br label %ret

; uselistorder directives
  uselistorder double* %f_alloca.i, { 0, 2, 1 }
}

define i64 @_Z3fibi(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %ifend, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %cmp_lte = icmp sle i64 %i1, 2
  br i1 %cmp_lte, label %then, label %ifend

then:                                             ; preds = %fnbody
  store i64 1, i64* %retval
  br label %ret

ifend:                                            ; preds = %fnbody
  %i2 = load i64, i64* %i_alloca
  %diff = sub i64 %i2, 2
  %callres = call i64 @_Z3fibi(i64 %diff)
  %i3 = load i64, i64* %i_alloca
  %diff4 = sub i64 %i3, 1
  %callres5 = call i64 @_Z3fibi(i64 %diff4)
  %sum = add i64 %callres5, %callres
  store i64 %sum, i64* %retval
  br label %ret

; uselistorder directives
  uselistorder i64 (i64)* @_Z3fibi, { 1, 2, 0 }
}

define i64 @_Z8fib_iteri(i64 %val) {
entry:
  %val_alloca = alloca i64
  store i64 %val, i64* %val_alloca
  %retval = alloca i64
  %fib_1_alloca = alloca i64
  %fib_2_alloca = alloca i64
  %res_alloca = alloca i64
  %i_alloca = alloca i64
  br label %fnbody

ret:                                              ; preds = %forend, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %val1 = load i64, i64* %val_alloca
  %cmp_lt = icmp slt i64 %val1, 3
  br i1 %cmp_lt, label %then, label %ifend

then:                                             ; preds = %fnbody
  store i64 1, i64* %retval
  br label %ret

ifend:                                            ; preds = %fnbody
  store i64 1, i64* %fib_1_alloca
  store i64 1, i64* %fib_2_alloca
  store i64 0, i64* %res_alloca
  store i64 0, i64* %i_alloca
  store i64 3, i64* %i_alloca
  br label %forhead

forhead:                                          ; preds = %forloop, %ifend
  %val2 = load i64, i64* %val_alloca
  %i = load i64, i64* %i_alloca
  %cmp_lte = icmp sle i64 %i, %val2
  br i1 %cmp_lte, label %forloop, label %forend

forloop:                                          ; preds = %forhead
  %fib_2 = load i64, i64* %fib_2_alloca
  %fib_1 = load i64, i64* %fib_1_alloca
  %sum = add i64 %fib_1, %fib_2
  store i64 %sum, i64* %res_alloca
  %fib_13 = load i64, i64* %fib_1_alloca
  store i64 %fib_13, i64* %fib_2_alloca
  %res = load i64, i64* %res_alloca
  store i64 %res, i64* %fib_1_alloca
  %i4 = load i64, i64* %i_alloca
  %sum5 = add i64 %i4, 1
  store i64 %sum5, i64* %i_alloca
  br label %forhead

forend:                                           ; preds = %forhead
  %res6 = load i64, i64* %res_alloca
  store i64 %res6, i64* %retval
  br label %ret

; uselistorder directives
  uselistorder i64 3, { 5, 6, 0, 1, 2, 3, 4, 7 }
}

define void @_Z12cfr_test_if1b(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %ifend, %then
  ret void

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %ifend

then:                                             ; preds = %fnbody
  br label %ret

ifend:                                            ; preds = %fnbody
  call void @exit(i64 1)
  br label %ret
}

define void @_Z12cfr_test_if2b(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %ifend, %then
  ret void

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %else

then:                                             ; preds = %fnbody
  br label %ret

else:                                             ; preds = %fnbody
  call void @exit(i64 0)
  br label %ifend

ifend:                                            ; preds = %else
  call void @exit(i64 1)
  br label %ret
}

define void @_Z12cfr_test_if3b(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %ifend, %else
  ret void

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %else

then:                                             ; preds = %fnbody
  call void @exit(i64 0)
  br label %ifend

else:                                             ; preds = %fnbody
  br label %ret

ifend:                                            ; preds = %then
  call void @exit(i64 1)
  br label %ret
}

define void @_Z12cfr_test_if4b(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %else, %then
  ret void

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %else

then:                                             ; preds = %fnbody
  br label %ret

else:                                             ; preds = %fnbody
  br label %ret
}

define void @_Z12cfr_test_whlb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %whlend, %whlloop
  ret void

fnbody:                                           ; preds = %entry
  br label %whlhead

whlhead:                                          ; preds = %fnbody
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %whlloop, label %whlend

whlloop:                                          ; preds = %whlhead
  br label %ret

whlend:                                           ; preds = %whlhead
  call void @exit(i64 1)
  br label %ret
}

define void @_Z15cfr_test_do_whlb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %doloop
  ret void

fnbody:                                           ; preds = %entry
  br label %doloop

doloop:                                           ; preds = %fnbody
  br label %ret
}

define void @_Z12cfr_test_forb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %forend, %forloop
  ret void

fnbody:                                           ; preds = %entry
  br label %forhead

forhead:                                          ; preds = %fnbody
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %forloop, label %forend

forloop:                                          ; preds = %forhead
  br label %ret

forend:                                           ; preds = %forhead
  call void @exit(i64 1)
  br label %ret
}

define i64 @_Z8wrong_cfb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %else, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %else

then:                                             ; preds = %fnbody
  store i64 42, i64* %retval
  br label %ret

else:                                             ; preds = %fnbody
  store i64 2, i64* %retval
  br label %ret
}

define void @_Z10asldfjasdfv() {
entry:
  br label %fnbody

ret:                                              ; preds = %ifend, %then
  ret void

fnbody:                                           ; preds = %entry
  br i1 true, label %then, label %ifend

then:                                             ; preds = %fnbody
  br label %ret

ifend:                                            ; preds = %fnbody
  br label %ret
}

define i64 @_Z9wrong_cf2b(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %ifend, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %else

then:                                             ; preds = %fnbody
  store i64 42, i64* %retval
  br label %ret

else:                                             ; preds = %fnbody
  call void @exit(i64 1)
  br label %ifend

ifend:                                            ; preds = %else
  store i64 2222, i64* %retval
  br label %ret
}

define i64 @_Z13cond_mv_test1i(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %else, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %cmp_lt = icmp slt i64 %i1, 0
  br i1 %cmp_lt, label %then, label %else

then:                                             ; preds = %fnbody
  %i2 = load i64, i64* %i_alloca
  store i64 %i2, i64* %retval
  br label %ret

else:                                             ; preds = %fnbody
  %i3 = load i64, i64* %i_alloca
  %diff = sub i64 %i3, 1
  store i64 %diff, i64* %retval
  br label %ret
}

define i64 @_Z13cond_mv_test2i(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %ifend
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %cmp_gt = icmp sgt i64 %i1, 0
  br i1 %cmp_gt, label %then, label %ifend

then:                                             ; preds = %fnbody
  %i2 = load i64, i64* %i_alloca
  %diff = sub i64 %i2, 1
  store i64 %diff, i64* %i_alloca
  br label %ifend

ifend:                                            ; preds = %then, %fnbody
  %i3 = load i64, i64* %i_alloca
  store i64 %i3, i64* %retval
  br label %ret
}

define i64 @_Z4opt1i(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %else, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %cmp_lt = icmp slt i64 %i1, 0
  br i1 %cmp_lt, label %then, label %else

then:                                             ; preds = %fnbody
  %i2 = load i64, i64* %i_alloca
  store i64 %i2, i64* %retval
  br label %ret

else:                                             ; preds = %fnbody
  %i3 = load i64, i64* %i_alloca
  %diff = sub i64 %i3, 1
  store i64 %diff, i64* %i_alloca
  store i64 %diff, i64* %retval
  br label %ret
}

define i64 @_Z4opt2i(i64 %i) {
entry:
  %i_alloca = alloca i64
  store i64 %i, i64* %i_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %else, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %i1 = load i64, i64* %i_alloca
  %cmp_gt = icmp sgt i64 %i1, 0
  br i1 %cmp_gt, label %then, label %else

then:                                             ; preds = %fnbody
  %i2 = load i64, i64* %i_alloca
  %diff = sub i64 %i2, 1
  store i64 %diff, i64* %i_alloca
  store i64 %diff, i64* %retval
  br label %ret

else:                                             ; preds = %fnbody
  %i3 = load i64, i64* %i_alloca
  store i64 %i3, i64* %retval
  br label %ret
}

define void @_Z8deadCodev() {
entry:
  %i_alloca.i3 = alloca i64
  %retval.i4 = alloca i64
  %i_alloca.i = alloca i64
  %retval.i = alloca i64
  %i_alloca = alloca i64
  %j_alloca = alloca i64
  %k_alloca = alloca i64
  br label %fnbody

ret:                                              ; preds = %_Z4opt2i.exit
  ret void

fnbody:                                           ; preds = %entry
  store i64 0, i64* %i_alloca
  store i64 0, i64* %j_alloca
  store i64 0, i64* %k_alloca
  %i = load i64, i64* %i_alloca
  %0 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %0)
  %1 = bitcast i64* %retval.i to i8*
  call void @llvm.lifetime.start(i64 8, i8* %1)
  store i64 %i, i64* %i_alloca.i
  %i1.i = load i64, i64* %i_alloca.i
  %cmp_lt.i = icmp slt i64 %i1.i, 0
  br i1 %cmp_lt.i, label %then.i, label %else.i

then.i:                                           ; preds = %fnbody
  %i2.i = load i64, i64* %i_alloca.i
  store i64 %i2.i, i64* %retval.i
  br label %_Z4opt1i.exit

else.i:                                           ; preds = %fnbody
  %i3.i = load i64, i64* %i_alloca.i
  %diff.i = sub i64 %i3.i, 1
  store i64 %diff.i, i64* %i_alloca.i
  store i64 %diff.i, i64* %retval.i
  br label %_Z4opt1i.exit

_Z4opt1i.exit:                                    ; preds = %then.i, %else.i
  %2 = load i64, i64* %retval.i
  %3 = bitcast i64* %i_alloca.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %3)
  %4 = bitcast i64* %retval.i to i8*
  call void @llvm.lifetime.end(i64 8, i8* %4)
  %i1 = load i64, i64* %i_alloca
  %5 = bitcast i64* %i_alloca.i3 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %5)
  %6 = bitcast i64* %retval.i4 to i8*
  call void @llvm.lifetime.start(i64 8, i8* %6)
  store i64 %i1, i64* %i_alloca.i3
  %i1.i5 = load i64, i64* %i_alloca.i3
  %cmp_gt.i = icmp sgt i64 %i1.i5, 0
  br i1 %cmp_gt.i, label %then.i8, label %else.i10

then.i8:                                          ; preds = %_Z4opt1i.exit
  %i2.i6 = load i64, i64* %i_alloca.i3
  %diff.i7 = sub i64 %i2.i6, 1
  store i64 %diff.i7, i64* %i_alloca.i3
  store i64 %diff.i7, i64* %retval.i4
  br label %_Z4opt2i.exit

else.i10:                                         ; preds = %_Z4opt1i.exit
  %i3.i9 = load i64, i64* %i_alloca.i3
  store i64 %i3.i9, i64* %retval.i4
  br label %_Z4opt2i.exit

_Z4opt2i.exit:                                    ; preds = %then.i8, %else.i10
  %7 = load i64, i64* %retval.i4
  %8 = bitcast i64* %i_alloca.i3 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %8)
  %9 = bitcast i64* %retval.i4 to i8*
  call void @llvm.lifetime.end(i64 8, i8* %9)
  store i64 2, i64* %i_alloca
  br label %ret

; uselistorder directives
  uselistorder i64* %retval.i, { 0, 3, 4, 2, 1 }
  uselistorder i64* %i_alloca.i, { 0, 3, 4, 2, 5, 6, 1 }
  uselistorder i64* %retval.i4, { 0, 3, 4, 2, 1 }
  uselistorder i64* %i_alloca.i3, { 0, 4, 2, 3, 5, 6, 1 }
  uselistorder i64 2, { 1, 2, 3, 4, 5, 0 }
  uselistorder i64 8, { 0, 2, 1, 3, 4, 6, 5, 7, 8, 9, 10, 11, 12, 13, 14, 16, 15, 17, 18, 19, 20, 22, 24, 26, 28, 30, 21, 23, 25, 27, 29, 31, 32, 34, 36, 38, 40, 42, 33, 35, 37, 39, 41, 43, 44, 45 }
  uselistorder label %_Z4opt2i.exit, { 1, 0 }
  uselistorder label %_Z4opt1i.exit, { 1, 0 }
}

define i64 @_Z7retTestb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca i64
  br label %fnbody

ret:                                              ; preds = %else, %then
  %0 = load i64, i64* %retval
  ret i64 %0

fnbody:                                           ; preds = %entry
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %then, label %else

then:                                             ; preds = %fnbody
  store i64 42, i64* %retval
  br label %ret

else:                                             ; preds = %fnbody
  store i64 0, i64* %retval
  br label %ret
}

define void @_Z10forvartestv() {
entry:
  %j_alloca = alloca double
  %j_alloca2 = alloca i64
  br label %fnbody

ret:                                              ; preds = %doend12
  ret void

fnbody:                                           ; preds = %entry
  store double 1.234560e+03, double* %j_alloca
  %j = load double, double* %j_alloca
  call void @prtln_f(double %j)
  %j1 = load double, double* %j_alloca
  call void @prtln_f(double %j1)
  store i64 0, i64* %j_alloca2
  br label %forhead

forhead:                                          ; preds = %forloop, %fnbody
  %j3 = load i64, i64* %j_alloca2
  %cmp_lt = icmp slt i64 %j3, 10
  br i1 %cmp_lt, label %forloop, label %forend

forloop:                                          ; preds = %forhead
  %j4 = load i64, i64* %j_alloca2
  call void @prtln_i(i64 %j4)
  %j5 = load i64, i64* %j_alloca2
  %sum = add i64 %j5, 1
  store i64 %sum, i64* %j_alloca2
  br label %forhead

forend:                                           ; preds = %forhead
  %j6 = load double, double* %j_alloca
  call void @prtln_f(double %j6)
  %j7 = load double, double* %j_alloca
  call void @prtln_f(double %j7)
  br label %forhead8

forhead8:                                         ; preds = %forend
  br label %forloop9

forloop9:                                         ; preds = %forhead8
  br label %forend10

forend10:                                         ; preds = %forloop9
  br label %doloop

doloop:                                           ; preds = %forend10
  br label %doend

doend:                                            ; preds = %doloop
  br label %whlhead

whlhead:                                          ; preds = %whlloop, %doend
  br i1 false, label %whlloop, label %whlend

whlloop:                                          ; preds = %whlhead
  br label %whlhead

whlend:                                           ; preds = %whlhead
  br label %doloop11

doloop11:                                         ; preds = %dohead, %whlend
  br label %dohead

dohead:                                           ; preds = %doloop11
  br i1 false, label %doloop11, label %doend12

doend12:                                          ; preds = %dohead
  br label %ret

; uselistorder directives
  uselistorder i1 false, { 1, 2, 0, 3 }
  uselistorder i64 1, { 19, 0, 1, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 32, 33, 34, 35, 36, 37, 2, 4, 3, 5, 6, 7, 8, 30, 10, 11, 12, 9, 31, 14, 15, 16, 13, 38, 39, 40, 41, 42, 17, 18, 43, 44, 45 }
  uselistorder i64 10, { 2, 0, 1, 3, 4 }
  uselistorder i64 0, { 8, 9, 0, 1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 22, 23, 24, 2, 3, 4, 5, 19, 6, 7, 20, 21, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34 }
}

define double @_Z5errorb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %whlend, %then
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  br label %whlhead

whlhead:                                          ; preds = %fnbody
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %whlloop, label %whlend

whlloop:                                          ; preds = %whlhead
  %b2 = load i1, i1* %b_alloca
  br i1 %b2, label %then, label %else

whlend:                                           ; preds = %else, %whlhead
  store double 3.000000e+00, double* %retval
  br label %ret

then:                                             ; preds = %whlloop
  store double 3.140000e+00, double* %retval
  br label %ret

else:                                             ; preds = %whlloop
  br label %whlend

; uselistorder directives
  uselistorder double* %retval, { 1, 0, 2 }
  uselistorder double 3.000000e+00, { 1, 0 }
  uselistorder label %ret, { 1, 0 }
}

define void @_Z10noJmpErrorb(i1 %b) {
entry:
  %b_alloca = alloca i1
  store i1 %b, i1* %b_alloca
  br label %fnbody

ret:                                              ; preds = %whlend8, %whlloop7
  ret void

fnbody:                                           ; preds = %entry
  br label %doloop

doloop:                                           ; preds = %fnbody
  br label %doend

doend:                                            ; preds = %doloop
  br label %whlhead

whlhead:                                          ; preds = %whlloop, %doend
  %b1 = load i1, i1* %b_alloca
  br i1 %b1, label %whlloop, label %whlend

whlloop:                                          ; preds = %whlhead
  br label %whlhead

whlend:                                           ; preds = %whlhead
  br label %whlhead2

whlhead2:                                         ; preds = %whlend
  %b5 = load i1, i1* %b_alloca
  br i1 %b5, label %whlloop3, label %whlend4

whlloop3:                                         ; preds = %whlhead2
  br label %whlend4

whlend4:                                          ; preds = %whlloop3, %whlhead2
  br label %whlhead6

whlhead6:                                         ; preds = %whlend4
  %b9 = load i1, i1* %b_alloca
  br i1 %b9, label %whlloop7, label %whlend8

whlloop7:                                         ; preds = %whlhead6
  br label %ret

whlend8:                                          ; preds = %whlhead6
  br label %ret
}

define double @_Z2__v() {
entry:
  %retval = alloca double
  br label %fnbody

ret:                                              ; preds = %else, %then
  %0 = load double, double* %retval
  ret double %0

fnbody:                                           ; preds = %entry
  br i1 true, label %then, label %else

then:                                             ; preds = %fnbody
  store double 4.200000e+01, double* %retval
  br label %ret

else:                                             ; preds = %fnbody
  store double 2.400000e+01, double* %retval
  br label %ret

; uselistorder directives
  uselistorder i1 true, { 1, 2, 3, 4, 0, 5, 6 }
  uselistorder i32 1, { 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 2, 3, 0, 1, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 56, 57, 58, 59, 60, 61, 62, 63, 4, 64, 6, 5, 65, 66, 7, 8, 9, 10, 55, 17, 18, 19, 20, 21, 22, 11, 12, 13, 14, 15, 16, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 23, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 24, 144, 145, 146, 147, 148 }
}

define void @_Z13print_doublesv() {
entry:
  br label %fnbody

ret:                                              ; preds = %fnbody
  ret void

fnbody:                                           ; preds = %entry
  call void @prtln_f(double 1.200000e+00)
  call void @prtln_f(double 0.000000e+00)
  call void @prtln_f(double 3.140000e+00)
  call void @prtln_f(double 4.224000e+01)
  call void @prtln_f(double 1.000000e+308)
  br label %ret

; uselistorder directives
  uselistorder double 1.000000e+308, { 1, 0 }
  uselistorder double 4.224000e+01, { 1, 0 }
  uselistorder double 3.140000e+00, { 2, 3, 0, 1 }
  uselistorder double 0.000000e+00, { 2, 0, 1, 3, 4, 5 }
  uselistorder void (double)* @prtln_f, { 5, 6, 7, 8, 9, 10, 11, 12, 13, 0, 1, 2, 3, 4 }
  uselistorder double 1.200000e+00, { 1, 0 }
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #0

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #0

attributes #0 = { argmemonly nounwind }
