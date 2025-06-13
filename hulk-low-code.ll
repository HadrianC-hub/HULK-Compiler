; ModuleID = 'hulk_module'
source_filename = "hulk_module"

@0 = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %0 = call double @llvm.sin.f64(double 0x400921FB54442D18)
  %1 = call double @llvm.cos.f64(double 0x400921FB54442D18)
  %divtmp = fdiv double %0, %1
  %2 = call double @llvm.sin.f64(double 0x400921FB54442D18)
  %3 = call double @llvm.cos.f64(double 0x400921FB54442D18)
  %divtmp1 = fdiv double %2, %3
  %divtmp2 = fdiv double 1.000000e+00, %divtmp1
  %addtmp = fadd double 2.000000e+00, %divtmp2
  %powtmp = call double @llvm.pow.f64(double %addtmp, double 2.000000e+00)
  %powtmp3 = call double @llvm.pow.f64(double %divtmp, double %powtmp)
  %4 = call i32 (ptr, ...) @printf(ptr @0, double %powtmp3)
  ret i32 0
}

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.sin.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.cos.f64(double) #0

; Function Attrs: nocallback nofree nosync nounwind speculatable willreturn memory(none)
declare double @llvm.pow.f64(double, double) #0

attributes #0 = { nocallback nofree nosync nounwind speculatable willreturn memory(none) }
