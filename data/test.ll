@pi = global i32 3

define i32 @sum(i32 %0, i32 %1) {
entry:
  %result = add nuw nsw i32 %0, %1
  ret i32 %result
}

define i32 @sub(i32 %0, i32 %1) {
entry:
  %result = sub nuw nsw i32 %0, %1
  ret i32 %result
}

define i32 @div(i32 %0, i32 %1) {
entry:
  %result = sdiv i32 %0, %1
  ret i32 %result
}

define i32 @mul(i32 %0, i32 %1) {
entry:
  %result = mul nsw i32 %0, %1
  ret i32 %result
}



define i32 @circle_area(i32 %a) {
entry:
  br label %case0

case0:                                            ; preds = %entry
  br i1 true, label %body0, label %no_match

no_match:                                         ; preds = %case0
  unreachable

body0:                                            ; preds = %case0
  %loadtmp = load i32, ptr @pi
  %calltmp = call i32 @mul(i32 %a, i32 %a)
  %calltmp1 = call i32 @mul(i32 %loadtmp, i32 %calltmp)
  ret i32 %calltmp1
}

define i32 @main() {
entry:
  br label %case0

case0:                                            ; preds = %entry
  br i1 true, label %body0, label %no_match

no_match:                                         ; preds = %case0
  unreachable

body0:                                            ; preds = %case0
  %calltmp = call i32 @circle_area(i32 5)
  ret i32 %calltmp
}
; a good "getting started" exercise with alive2
; would be to change the udiv (unsigned divide)
; instruction in @src above into an sdiv (signed
; divide) and then fix @tgt so that the optimization
; is again correct

; alive-tv is a translation validation tool based
; on Alive2:
;   https://github.com/AliveToolkit/alive2
;
; you can use it in two ways:
; 1. as illustrated by the default text above, if
;    you provide an LLVM IR function called src
;    and one called tgt, alive-tv will try to prove
;    that tgt refines src
; 2. otherwise, alive-tv will run an optimization
;    pipeline similar to -O2 on your code, and then
;    try to show that the optimized function(s) 
;    refine the original one(s)