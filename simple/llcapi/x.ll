; ModuleID = 't.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@pole = common global [16 x i32] zeroinitializer, align 16
@sp = internal global i32 1, align 4

; Function Attrs: nounwind uwtable
define void @test() #0 {
  %f = alloca float, align 4
  %1 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @pole, i64 0, i64 1), align 4
  store volatile i32 %1, i32* @sp, align 4
  %2 = load volatile i32, i32* @sp, align 4
  %3 = sitofp i32 %2 to float
  store volatile float %3, float* %f, align 4
  %4 = load volatile float, float* %f, align 4
  %5 = fptosi float %4 to i32
  store volatile i32 %5, i32* @sp, align 4
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @test()
  ret i32 0
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
