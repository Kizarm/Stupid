
%struct.PesApi_s = type { void (%union.RamDef_u*)*, void ()*, %struct._StationInfo*, void (i16*, i16)*, void (i8*)* }
%union.RamDef_u = type { [1024 x i16] }
%struct._StationInfo = type { i32, i16, i16 }

@MainApi = internal global %struct.PesApi_s { void (%union.RamDef_u*)* @SetRamBasePtr, void ()* @Simple, %struct._StationInfo* @GStation, void (i16*, i16)* null, void (i8*)* null }, align 4
@RamBasePtr = common global %union.RamDef_u* null, align 4

define nonnull %struct.PesApi_s* @GetPesApiPtr() #0 {
  store void (%union.RamDef_u*)* @SetRamBasePtr, void (%union.RamDef_u*)** getelementptr inbounds (%struct.PesApi_s, %struct.PesApi_s* @MainApi, i32 0, i32 0), align 4
  store void ()* @Simple, void ()** getelementptr inbounds (%struct.PesApi_s, %struct.PesApi_s* @MainApi, i32 0, i32 1), align 4
  store %struct._StationInfo* @GStation, %struct._StationInfo** getelementptr inbounds (%struct.PesApi_s, %struct.PesApi_s* @MainApi, i32 0, i32 2), align 4
  store void (i16*, i16)* null, void (i16*, i16)** getelementptr inbounds (%struct.PesApi_s, %struct.PesApi_s* @MainApi, i32 0, i32 3), align 4
  store void (i8*)* null, void (i8*)** getelementptr inbounds (%struct.PesApi_s, %struct.PesApi_s* @MainApi, i32 0, i32 4), align 4
  ret %struct.PesApi_s* @MainApi
}

define void @SetRamBasePtr(%union.RamDef_u* %ptr) #0 {
  store %union.RamDef_u* %ptr, %union.RamDef_u** @RamBasePtr, align 4
  ret void
}


define zeroext i16 @WgetBit(i32 %bitadr) #0 {
  %1 = lshr i32 %bitadr, 4
  %2 = load [1024 x i16]*, [1024 x i16]** bitcast (%union.RamDef_u** @RamBasePtr to [1024 x i16]**), align 4
  %3 = getelementptr inbounds [1024 x i16], [1024 x i16]* %2, i32 0, i32 %1
  %4 = load i16, i16* %3, align 2
  %5 = and i32 %bitadr, 15
  %6 = shl i32 1, %5
  %7 = zext i16 %4 to i32
  %8 = and i32 %7, %6
  %not. = icmp ne i32 %8, 0
  %. = zext i1 %not. to i16
  ret i16 %.
}

define void @WsetBit(i32 %bitadr, i16 zeroext %bitvalue) #0 {
  %1 = and i32 %bitadr, 15
  %2 = shl i32 1, %1
  %3 = icmp eq i16 %bitvalue, 0
  br i1 %3, label %12, label %4

  %5 = lshr i32 %bitadr, 4
  %6 = load [1024 x i16]*, [1024 x i16]** bitcast (%union.RamDef_u** @RamBasePtr to [1024 x i16]**), align 4
  %7 = getelementptr inbounds [1024 x i16], [1024 x i16]* %6, i32 0, i32 %5
  %8 = load i16, i16* %7, align 2
  %9 = zext i16 %8 to i32
  %10 = or i32 %9, %2
  %11 = trunc i32 %10 to i16
  store i16 %11, i16* %7, align 2
  br label %21

  %13 = xor i32 %2, 65535
  %14 = lshr i32 %bitadr, 4
  %15 = load [1024 x i16]*, [1024 x i16]** bitcast (%union.RamDef_u** @RamBasePtr to [1024 x i16]**), align 4
  %16 = getelementptr inbounds [1024 x i16], [1024 x i16]* %15, i32 0, i32 %14
  %17 = load i16, i16* %16, align 2
  %18 = zext i16 %17 to i32
  %19 = and i32 %18, %13
  %20 = trunc i32 %19 to i16
  store i16 %20, i16* %16, align 2
  br label %21

  ret void
}

define void @WcplBit(i32 %bitadr) #0 {
  %1 = and i32 %bitadr, 15
  %2 = shl i32 1, %1
  %3 = lshr i32 %bitadr, 4
  %4 = load [1024 x i16]*, [1024 x i16]** bitcast (%union.RamDef_u** @RamBasePtr to [1024 x i16]**), align 4
  %5 = getelementptr inbounds [1024 x i16], [1024 x i16]* %4, i32 0, i32 %3
  %6 = load i16, i16* %5, align 2
  %7 = zext i16 %6 to i32
  %8 = xor i32 %7, %2
  %9 = trunc i32 %8 to i16
  store i16 %9, i16* %5, align 2
  ret void
}

define zeroext i16 @TestPole(i16 zeroext %a, i16 zeroext %b) #0 {
  %1 = icmp ugt i16 %a, %b
  %b.a = select i1 %1, i16 %b, i16 %a
  ret i16 %b.a
}

define signext i16 @absi(i16 signext %n) #0 {
  %1 = icmp slt i16 %n, 0
  %2 = sub i16 0, %n
  %.0 = select i1 %1, i16 %2, i16 %n
  ret i16 %.0
}

define float @truncf(float %x) #0 {
  %1 = bitcast float %x to i32
  %2 = and i32 %1, -2147483648
  %3 = lshr i32 %1, 23
  %4 = and i32 %3, 255
  %5 = add nsw i32 %4, -127
  %6 = icmp slt i32 %5, 23
  br i1 %6, label %7, label %14

  %8 = icmp ult i32 %4, 127
  br i1 %8, label %18, label %9

  %10 = lshr i32 8388607, %5
  %11 = xor i32 %10, -1
  %12 = and i32 %1, %11
  %13 = or i32 %12, %2
  br label %18

  %15 = icmp eq i32 %5, 255
  br i1 %15, label %16, label %18

  %17 = fadd float %x, %x
  br label %20

  %u.sroa.0.0 = phi i32 [ %13, %9 ], [ %1, %14 ], [ %2, %7 ]
  %19 = bitcast i32 %u.sroa.0.0 to float
  br label %20

  %.0 = phi float [ %19, %18 ], [ %17, %16 ]
  ret float %.0
}

define float @fabsf(float %x) #0 {
  %1 = fcmp olt float %x, 0.000000e+00
  %2 = fsub float -0.000000e+00, %x
  %.0 = select i1 %1, float %2, float %x
  ret float %.0
}

define void @WordChangeWrap(i16* %ptr, i16 zeroext %newdata) #0 {
  %1 = load void (i16*, i16)*, void (i16*, i16)** getelementptr inbounds (%struct.PesApi_s, %struct.PesApi_s* @MainApi, i32 0, i32 3), align 4
  %2 = icmp eq void (i16*, i16)* %1, null
  br i1 %2, label %4, label %3

  tail call void %1(i16* %ptr, i16 zeroext %newdata) #0
  br label %4

  ret void
}

define void @Disps(i8* %str) #0 {
  %1 = load void (i8*)*, void (i8*)** getelementptr inbounds (%struct.PesApi_s, %struct.PesApi_s* @MainApi, i32 0, i32 4), align 4
  %2 = icmp eq void (i8*)* %1, null
  br i1 %2, label %4, label %3

  tail call void %1(i8* %str) #0
  br label %4

  ret void
}



