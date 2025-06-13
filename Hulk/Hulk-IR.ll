; ModuleID = 'hulk_module'
source_filename = "hulk_module"

@0 = private unnamed_addr constant [4 x i8] c"%g\0A\00", align 1

declare i32 @printf(ptr, ...)

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %0 = call i32 (ptr, ...) @printf(ptr @0, double 0x400921FB54442D18)
  ret i32 0
}
