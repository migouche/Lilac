; ModuleID = 'jit'
source_filename = "jit"

define fastcc i32 @sum(i32 %0, i32 %1) {
entry:
  %result = add nuw i32 %0, %1
  ret i32 %result
}

define fastcc i32 @div(i32 %0, i32 %1) {
entry:
  %result = sdiv i32 %0, %1
  ret i32 %result
}

define i32 @avg(i32 %a, i32 %b) {
entry:
  %calltmp = call i32 @sum(i32 %a, i32 %b)
  %calltmp1 = call i32 @div(i32 %calltmp, i32 2)
  ret i32 %calltmp1
}

define i32 @main() {
entry:
  %calltmp = call i32 @avg(i32 3, i32 6)
  ret i32 %calltmp
}