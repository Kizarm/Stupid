; ModuleID = 't.c'
target datalayout = "e-m:e-p:32:32-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "thumbv7em--"

@pole = common global [16 x i32] zeroinitializer, align 4
@sp = internal global i32 1, align 4

; Function Attrs: norecurse nounwind
define arm_aapcscc void @test() #0 {
  %f = alloca float, align 4
  %1 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @pole, i32 0, i32 1), align 4, !tbaa !3
  store volatile i32 %1, i32* @sp, align 4, !tbaa !3
  %2 = bitcast float* %f to i8*
  call void @llvm.lifetime.start(i64 4, i8* %2)
  %3 = load volatile i32, i32* @sp, align 4, !tbaa !3
  %4 = sitofp i32 %3 to float
  store volatile float %4, float* %f, align 4
  %5 = load volatile float, float* %f, align 4
  %6 = fptosi float %5 to i32
  store volatile i32 %6, i32* @sp, align 4, !tbaa !3
  call void @llvm.lifetime.end(i64 4, i8* %2)
  ret void
}

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start(i64, i8* nocapture) #1

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end(i64, i8* nocapture) #1

; Function Attrs: norecurse nounwind
define arm_aapcscc i32 @main() #0 {
  %f.i = alloca float, align 4
  %1 = load i32, i32* getelementptr inbounds ([16 x i32], [16 x i32]* @pole, i32 0, i32 1), align 4, !tbaa !3
  store volatile i32 %1, i32* @sp, align 4, !tbaa !3
  %2 = bitcast float* %f.i to i8*
  call void @llvm.lifetime.start(i64 4, i8* %2)
  %3 = load volatile i32, i32* @sp, align 4, !tbaa !3
  %4 = sitofp i32 %3 to float
  store volatile float %4, float* %f.i, align 4
  %5 = load volatile float, float* %f.i, align 4
  %6 = fptosi float %5 to i32
  store volatile i32 %6, i32* @sp, align 4, !tbaa !3
  call void @llvm.lifetime.end(i64 4, i8* %2)
  ret i32 0
}

attributes #0 = { norecurse nounwind "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="cortex-m4" "target-features"="+d16,+dsp,+fp-only-sp,+hwdiv,+strict-align,+vfp4,-crypto,-fp-armv8,-neon" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"min_enum_size", i32 4}
!2 = !{!"clang version 3.8.0-2ubuntu4 (tags/RELEASE_380/final)"}
!3 = !{!4, !4, i64 0}
!4 = !{!"int", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
