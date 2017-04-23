#ifndef PES_SYSTEM_H
#define PES_SYSTEM_H

typedef unsigned short WORD;

struct SystemVariables {
    WORD I0            ;     /* 0000 */
    WORD I1            ;     /* 0010 */
    WORD I2            ;     /* 0020 */
    WORD I3            ;     /* 0030 */
    WORD I4            ;     /* 0040 */
    WORD I5            ;     /* 0050 */
    WORD I6            ;     /* 0060 */
    WORD I7            ;     /* 0070 */
    WORD I8            ;     /* 0080 */
    WORD I9            ;     /* 0090 */
    WORD I10           ;     /* 00A0 */
    WORD I11           ;     /* 00B0 */
    WORD I12           ;     /* 00C0 */
    WORD I13           ;     /* 00D0 */
    WORD I14           ;     /* 00E0 */
    WORD I15           ;     /* 00F0 */
    WORD I16           ;     /* 0100 */
    WORD I17           ;     /* 0110 */
    WORD I18           ;     /* 0120 */
    WORD I19           ;     /* 0130 */
    WORD I20           ;     /* 0140 */
    WORD I21           ;     /* 0150 */
    WORD I22           ;     /* 0160 */
    WORD I23           ;     /* 0170 */
    WORD I24           ;     /* 0180 */
    WORD I25           ;     /* 0190 */
    WORD I26           ;     /* 01A0 */
    WORD I27           ;     /* 01B0 */
    WORD I28           ;     /* 01C0 */
    WORD I29           ;     /* 01D0 */
    WORD I30           ;     /* 01E0 */
    WORD I31           ;     /* 01F0 */
    WORD O0            ;     /* 0200 */
    WORD O1            ;     /* 0210 */
    WORD O2            ;     /* 0220 */
    WORD O3            ;     /* 0230 */
    WORD O4            ;     /* 0240 */
    WORD O5            ;     /* 0250 */
    WORD O6            ;     /* 0260 */
    WORD O7            ;     /* 0270 */
    WORD O8            ;     /* 0280 */
    WORD O9            ;     /* 0290 */
    WORD O10           ;     /* 02A0 */
    WORD O11           ;     /* 02B0 */
    WORD O12           ;     /* 02C0 */
    WORD O13           ;     /* 02D0 */
    WORD O14           ;     /* 02E0 */
    WORD O15           ;     /* 02F0 */
    WORD O16           ;     /* 0300 */
    WORD O17           ;     /* 0310 */
    WORD O18           ;     /* 0320 */
    WORD O19           ;     /* 0330 */
    WORD O20           ;     /* 0340 */
    WORD O21           ;     /* 0350 */
    WORD O22           ;     /* 0360 */
    WORD O23           ;     /* 0370 */
    WORD O24           ;     /* 0380 */
    WORD O25           ;     /* 0390 */
    WORD O26           ;     /* 03A0 */
    WORD O27           ;     /* 03B0 */
    WORD O28           ;     /* 03C0 */
    WORD O29           ;     /* 03D0 */
    WORD O30           ;     /* 03E0 */
    WORD O31           ;     /* 03F0 */
    WORD D0            ;     /* 0400 */
    WORD D1            ;     /* 0410 */
    WORD D2            ;     /* 0420 */
    WORD D3            ;     /* 0430 */
    WORD D4            ;     /* 0440 */
    WORD D5            ;     /* 0450 */
    WORD D6            ;     /* 0460 */
    WORD D7            ;     /* 0470 */
    WORD D8            ;     /* 0480 */
    WORD D9            ;     /* 0490 */
    WORD D10           ;     /* 04A0 */
    WORD D11           ;     /* 04B0 */
    WORD D12           ;     /* 04C0 */
    WORD D13           ;     /* 04D0 */
    WORD D14           ;     /* 04E0 */
    WORD D15           ;     /* 04F0 */
    WORD D16           ;     /* 0500 */
    WORD D17           ;     /* 0510 */
    WORD D18           ;     /* 0520 */
    WORD D19           ;     /* 0530 */
    WORD D20           ;     /* 0540 */
    WORD D21           ;     /* 0550 */
    WORD D22           ;     /* 0560 */
    WORD D23           ;     /* 0570 */
    WORD D24           ;     /* 0580 */
    WORD D25           ;     /* 0590 */
    WORD D26           ;     /* 05A0 */
    WORD D27           ;     /* 05B0 */
    WORD D28           ;     /* 05C0 */
    WORD D29           ;     /* 05D0 */
    WORD D30           ;     /* 05E0 */
    WORD D31           ;     /* 05F0 */
    WORD D32           ;     /* 0600 */
    WORD D33           ;     /* 0610 */
    WORD D34           ;     /* 0620 */
    WORD D35           ;     /* 0630 */
    WORD D36           ;     /* 0640 */
    WORD D37           ;     /* 0650 */
    WORD D38           ;     /* 0660 */
    WORD D39           ;     /* 0670 */
    WORD D40           ;     /* 0680 */
    WORD D41           ;     /* 0690 */
    WORD D42           ;     /* 06A0 */
    WORD D43           ;     /* 06B0 */
    WORD D44           ;     /* 06C0 */
    WORD D45           ;     /* 06D0 */
    WORD D46           ;     /* 06E0 */
    WORD D47           ;     /* 06F0 */
    WORD D48           ;     /* 0700 */
    WORD D49           ;     /* 0710 */
    WORD D50           ;     /* 0720 */
    WORD D51           ;     /* 0730 */
    WORD D52           ;     /* 0740 */
    WORD D53           ;     /* 0750 */
    WORD D54           ;     /* 0760 */
    WORD D55           ;     /* 0770 */
    WORD D56           ;     /* 0780 */
    WORD D57           ;     /* 0790 */
    WORD D58           ;     /* 07A0 */
    WORD D59           ;     /* 07B0 */
    WORD D60           ;     /* 07C0 */
    WORD D61           ;     /* 07D0 */
    WORD D62           ;     /* 07E0 */
    WORD D63           ;     /* 07F0 */
    union {
        WORD T0            ;     /* 0800 */
        WORD W0            ;     /* 0800 */
    };
    union {
        WORD T1            ;     /* 0810 */
        WORD W1            ;     /* 0810 */
    };
    union {
        WORD T2            ;     /* 0820 */
        WORD W2            ;     /* 0820 */
    };
    union {
        WORD T3            ;     /* 0830 */
        WORD W3            ;     /* 0830 */
    };
    union {
        WORD T4            ;     /* 0840 */
        WORD W4            ;     /* 0840 */
    };
    union {
        WORD T5            ;     /* 0850 */
        WORD W5            ;     /* 0850 */
    };
    union {
        WORD T6            ;     /* 0860 */
        WORD W6            ;     /* 0860 */
    };
    union {
        WORD T7            ;     /* 0870 */
        WORD W7            ;     /* 0870 */
    };
    union {
        WORD SECOND        ;     /* 0880 */
        WORD W8            ;     /* 0880 */
    };
    union {
        WORD MINUTE        ;     /* 0890 */
        WORD W9            ;     /* 0890 */
    };
    union {
        WORD HOUR          ;     /* 08A0 */
        WORD W10           ;     /* 08A0 */
    };
    union {
        WORD DAY           ;     /* 08B0 */
        WORD W11           ;     /* 08B0 */
    };
    union {
        WORD MONTH         ;     /* 08C0 */
        WORD W12           ;     /* 08C0 */
    };
    union {
        WORD YEAR          ;     /* 08D0 */
        WORD W13           ;     /* 08D0 */
    };
    union {
        WORD WEEK          ;     /* 08E0 */
        WORD W14           ;     /* 08E0 */
    };
    union {
        WORD SPEED         ;     /* 08F0 */
        WORD W15           ;     /* 08F0 */
    };
    union {
        WORD PAGEPOINTER   ;     /* 0900 */
        WORD W16           ;     /* 0900 */
    };
    union {
        WORD POINTER       ;     /* 0910 */
        WORD W17           ;     /* 0910 */
    };
    union {
        WORD CALIB0        ;     /* 0920 */
        WORD W18           ;     /* 0920 */
    };
    union {
        WORD CALIB1        ;     /* 0930 */
        WORD W19           ;     /* 0930 */
    };
    union {
        WORD CALIB2        ;     /* 0940 */
        WORD W20           ;     /* 0940 */
    };
    union {
        WORD CALIB3        ;     /* 0950 */
        WORD W21           ;     /* 0950 */
    };
    union {
        WORD CALIB4        ;     /* 0960 */
        WORD W22           ;     /* 0960 */
    };
    union {
        WORD CALIB5        ;     /* 0970 */
        WORD W23           ;     /* 0970 */
    };
    union {
        WORD CALIB6        ;     /* 0980 */
        WORD W24           ;     /* 0980 */
    };
    union {
        WORD CALIB7        ;     /* 0990 */
        WORD W25           ;     /* 0990 */
    };
    union {
        WORD ADCMODE0      ;     /* 09A0 */
        WORD ADCMODE       ;     /* 09A0 */
        WORD OFFSET0       ;     /* 09A0 */
        WORD W26           ;     /* 09A0 */
    };
    union {
        WORD ADCMODE1      ;     /* 09B0 */
        WORD OFFSET1       ;     /* 09B0 */
        WORD W27           ;     /* 09B0 */
    };
    union {
        WORD ADCMODE2      ;     /* 09C0 */
        WORD OFFSET2       ;     /* 09C0 */
        WORD W28           ;     /* 09C0 */
    };
    union {
        WORD ADCMODE3      ;     /* 09D0 */
        WORD OFFSET3       ;     /* 09D0 */
        WORD W29           ;     /* 09D0 */
    };
    union {
        WORD ADCMODE4      ;     /* 09E0 */
        WORD OFFSET4       ;     /* 09E0 */
        WORD W30           ;     /* 09E0 */
    };
    union {
        WORD ADCMODE5      ;     /* 09F0 */
        WORD OFFSET5       ;     /* 09F0 */
        WORD W31           ;     /* 09F0 */
    };
    union {
        WORD ADCMODE6      ;     /* 0A00 */
        WORD OFFSET6       ;     /* 0A00 */
        WORD W32           ;     /* 0A00 */
    };
    union {
        WORD ADCMODE7      ;     /* 0A10 */
        WORD OFFSET7       ;     /* 0A10 */
        WORD W33           ;     /* 0A10 */
    };
    union {
        WORD POSITION      ;     /* 0A20 */
        WORD W34           ;     /* 0A20 */
    };
    union {
        WORD FORMAT        ;     /* 0A30 */
        WORD W35           ;     /* 0A30 */
    };
    union {
        WORD KBCODE        ;     /* 0A40 */
        WORD W36           ;     /* 0A40 */
    };
    union {
        WORD KBDELAY       ;     /* 0A50 */
        WORD W37           ;     /* 0A50 */
    };
    union {
        WORD KBREPEAT      ;     /* 0A60 */
        WORD W38           ;     /* 0A60 */
    };
    union {
        WORD EEWR_COUNT    ;     /* 0A70 */
        WORD W39           ;     /* 0A70 */
    };
    union {
        WORD COUNT0        ;     /* 0A80 */
        WORD W40           ;     /* 0A80 */
    };
    union {
        WORD COUNT1        ;     /* 0A90 */
        WORD W41           ;     /* 0A90 */
    };
    union {
        WORD SENDCODE      ;     /* 0AA0 */
        WORD W42           ;     /* 0AA0 */
    };
    union {
        WORD SENDADDR      ;     /* 0AB0 */
        WORD W43           ;     /* 0AB0 */
    };
    union {
        WORD MEMADDR       ;     /* 0AC0 */
        WORD W44           ;     /* 0AC0 */
    };
    union {
        WORD BITADDR       ;     /* 0AD0 */
        WORD W45           ;     /* 0AD0 */
    };
    union {
        WORD SENDDATA      ;     /* 0AE0 */
        WORD W46           ;     /* 0AE0 */
    };
    WORD W47           ;     /* 0AF0 */
    WORD W48           ;     /* 0B00 */
    WORD W49           ;     /* 0B10 */
    union {
        WORD MYADDR        ;     /* 0B20 */
        WORD W50           ;     /* 0B20 */
    };
    WORD W51           ;     /* 0B30 */
    WORD W52           ;     /* 0B40 */
    WORD W53           ;     /* 0B50 */
    WORD W54           ;     /* 0B60 */
    WORD W55           ;     /* 0B70 */
    WORD W56           ;     /* 0B80 */
    WORD W57           ;     /* 0B90 */
    WORD W58           ;     /* 0BA0 */
    WORD W59           ;     /* 0BB0 */
    WORD W60           ;     /* 0BC0 */
    WORD W61           ;     /* 0BD0 */
    WORD W62           ;     /* 0BE0 */
    WORD W63           ;     /* 0BF0 */
    WORD W64           ;     /* 0C00 */
    WORD W65           ;     /* 0C10 */
    WORD W66           ;     /* 0C20 */
    WORD W67           ;     /* 0C30 */
    WORD W68           ;     /* 0C40 */
    WORD W69           ;     /* 0C50 */
    WORD W70           ;     /* 0C60 */
    WORD W71           ;     /* 0C70 */
    WORD W72           ;     /* 0C80 */
    WORD W73           ;     /* 0C90 */
    WORD W74           ;     /* 0CA0 */
    WORD W75           ;     /* 0CB0 */
    WORD W76           ;     /* 0CC0 */
    WORD W77           ;     /* 0CD0 */
    WORD W78           ;     /* 0CE0 */
    WORD W79           ;     /* 0CF0 */
    WORD W80           ;     /* 0D00 */
    WORD W81           ;     /* 0D10 */
    WORD W82           ;     /* 0D20 */
    WORD W83           ;     /* 0D30 */
    WORD W84           ;     /* 0D40 */
    WORD W85           ;     /* 0D50 */
    WORD W86           ;     /* 0D60 */
    WORD W87           ;     /* 0D70 */
    WORD W88           ;     /* 0D80 */
    WORD W89           ;     /* 0D90 */
    WORD W90           ;     /* 0DA0 */
    WORD W91           ;     /* 0DB0 */
    WORD W92           ;     /* 0DC0 */
    WORD W93           ;     /* 0DD0 */
    WORD W94           ;     /* 0DE0 */
    WORD W95           ;     /* 0DF0 */
    WORD W96           ;     /* 0E00 */
    WORD W97           ;     /* 0E10 */
    WORD W98           ;     /* 0E20 */
    WORD W99           ;     /* 0E30 */
    WORD W100          ;     /* 0E40 */
    WORD W101          ;     /* 0E50 */
    WORD W102          ;     /* 0E60 */
    WORD W103          ;     /* 0E70 */
    WORD W104          ;     /* 0E80 */
    WORD W105          ;     /* 0E90 */
    WORD W106          ;     /* 0EA0 */
    WORD W107          ;     /* 0EB0 */
    WORD W108          ;     /* 0EC0 */
    WORD W109          ;     /* 0ED0 */
    WORD W110          ;     /* 0EE0 */
    WORD W111          ;     /* 0EF0 */
    WORD W112          ;     /* 0F00 */
    WORD W113          ;     /* 0F10 */
    WORD W114          ;     /* 0F20 */
    WORD W115          ;     /* 0F30 */
    WORD W116          ;     /* 0F40 */
    WORD W117          ;     /* 0F50 */
    WORD W118          ;     /* 0F60 */
    WORD W119          ;     /* 0F70 */
    WORD W120          ;     /* 0F80 */
    WORD W121          ;     /* 0F90 */
    WORD W122          ;     /* 0FA0 */
    WORD W123          ;     /* 0FB0 */
    WORD W124          ;     /* 0FC0 */
    WORD W125          ;     /* 0FD0 */
    WORD W126          ;     /* 0FE0 */
    WORD W127          ;     /* 0FF0 */
    union {
        WORD DX0           ;     /* 1000 */
        struct {
            WORD X0          :1;     /* 1000 */
            WORD X1          :1;     /* 1001 */
            WORD X2          :1;     /* 1002 */
            WORD X3          :1;     /* 1003 */
            WORD X4          :1;     /* 1004 */
            WORD X5          :1;     /* 1005 */
            WORD X6          :1;     /* 1006 */
            WORD X7          :1;     /* 1007 */
            WORD X8          :1;     /* 1008 */
            WORD X9          :1;     /* 1009 */
            WORD X10         :1;     /* 100A */
            WORD X11         :1;     /* 100B */
            WORD X12         :1;     /* 100C */
            WORD X13         :1;     /* 100D */
            WORD X14         :1;     /* 100E */
            WORD X15         :1;     /* 100F */
        };
    };
    union {
        WORD DX1           ;     /* 1010 */
        struct {
            WORD X16         :1;     /* 1010 */
            WORD X17         :1;     /* 1011 */
            WORD X18         :1;     /* 1012 */
            WORD X19         :1;     /* 1013 */
            WORD X20         :1;     /* 1014 */
            WORD X21         :1;     /* 1015 */
            WORD X22         :1;     /* 1016 */
            WORD X23         :1;     /* 1017 */
            WORD X24         :1;     /* 1018 */
            WORD X25         :1;     /* 1019 */
            WORD X26         :1;     /* 101A */
            WORD X27         :1;     /* 101B */
            WORD X28         :1;     /* 101C */
            WORD X29         :1;     /* 101D */
            WORD X30         :1;     /* 101E */
            WORD X31         :1;     /* 101F */
        };
    };
    union {
        WORD DY0           ;     /* 1020 */
        struct {
            WORD Y0          :1;     /* 1020 */
            WORD Y1          :1;     /* 1021 */
            WORD Y2          :1;     /* 1022 */
            WORD Y3          :1;     /* 1023 */
            WORD Y4          :1;     /* 1024 */
            WORD Y5          :1;     /* 1025 */
            WORD Y6          :1;     /* 1026 */
            WORD Y7          :1;     /* 1027 */
            WORD Y8          :1;     /* 1028 */
            WORD Y9          :1;     /* 1029 */
            WORD Y10         :1;     /* 102A */
            WORD Y11         :1;     /* 102B */
            WORD Y12         :1;     /* 102C */
            WORD Y13         :1;     /* 102D */
            WORD Y14         :1;     /* 102E */
            WORD Y15         :1;     /* 102F */
        };
    };
    union {
        WORD DY1           ;     /* 1030 */
        struct {
            WORD Y16         :1;     /* 1030 */
            WORD Y17         :1;     /* 1031 */
            WORD Y18         :1;     /* 1032 */
            WORD Y19         :1;     /* 1033 */
            WORD Y20         :1;     /* 1034 */
            WORD Y21         :1;     /* 1035 */
            WORD Y22         :1;     /* 1036 */
            WORD Y23         :1;     /* 1037 */
            WORD Y24         :1;     /* 1038 */
            WORD Y25         :1;     /* 1039 */
            WORD Y26         :1;     /* 103A */
            WORD Y27         :1;     /* 103B */
            WORD Y28         :1;     /* 103C */
            WORD Y29         :1;     /* 103D */
            WORD Y30         :1;     /* 103E */
            WORD Y31         :1;     /* 103F */
        };
    };
    union {
        WORD DM0           ;     /* 1040 */
        struct {
            WORD M0          :1;     /* 1040 */
            WORD M1          :1;     /* 1041 */
            WORD M2          :1;     /* 1042 */
            WORD M3          :1;     /* 1043 */
            WORD M4          :1;     /* 1044 */
            WORD M5          :1;     /* 1045 */
            WORD M6          :1;     /* 1046 */
            WORD M7          :1;     /* 1047 */
            WORD M8          :1;     /* 1048 */
            WORD M9          :1;     /* 1049 */
            WORD M10         :1;     /* 104A */
            WORD M11         :1;     /* 104B */
            WORD M12         :1;     /* 104C */
            WORD M13         :1;     /* 104D */
            WORD M14         :1;     /* 104E */
            WORD M15         :1;     /* 104F */
        };
    };
    union {
        WORD DM1           ;     /* 1050 */
        struct {
            WORD M16         :1;     /* 1050 */
            WORD M17         :1;     /* 1051 */
            WORD M18         :1;     /* 1052 */
            WORD M19         :1;     /* 1053 */
            WORD M20         :1;     /* 1054 */
            WORD M21         :1;     /* 1055 */
            WORD M22         :1;     /* 1056 */
            WORD M23         :1;     /* 1057 */
            WORD M24         :1;     /* 1058 */
            WORD M25         :1;     /* 1059 */
            WORD M26         :1;     /* 105A */
            WORD M27         :1;     /* 105B */
            WORD M28         :1;     /* 105C */
            WORD M29         :1;     /* 105D */
            WORD M30         :1;     /* 105E */
            WORD M31         :1;     /* 105F */
        };
    };
    union {
        WORD DM2           ;     /* 1060 */
        struct {
            WORD M32         :1;     /* 1060 */
            WORD M33         :1;     /* 1061 */
            WORD M34         :1;     /* 1062 */
            WORD M35         :1;     /* 1063 */
            WORD M36         :1;     /* 1064 */
            WORD M37         :1;     /* 1065 */
            WORD M38         :1;     /* 1066 */
            WORD M39         :1;     /* 1067 */
            WORD M40         :1;     /* 1068 */
            WORD M41         :1;     /* 1069 */
            WORD M42         :1;     /* 106A */
            WORD M43         :1;     /* 106B */
            WORD M44         :1;     /* 106C */
            WORD M45         :1;     /* 106D */
            WORD M46         :1;     /* 106E */
            WORD M47         :1;     /* 106F */
        };
    };
    union {
        WORD DM3           ;     /* 1070 */
        struct {
            WORD M48         :1;     /* 1070 */
            WORD M49         :1;     /* 1071 */
            WORD M50         :1;     /* 1072 */
            WORD M51         :1;     /* 1073 */
            WORD M52         :1;     /* 1074 */
            WORD M53         :1;     /* 1075 */
            WORD M54         :1;     /* 1076 */
            WORD M55         :1;     /* 1077 */
            WORD M56         :1;     /* 1078 */
            WORD M57         :1;     /* 1079 */
            WORD M58         :1;     /* 107A */
            WORD M59         :1;     /* 107B */
            WORD M60         :1;     /* 107C */
            WORD M61         :1;     /* 107D */
            WORD M62         :1;     /* 107E */
            WORD M63         :1;     /* 107F */
        };
    };
    union {
        WORD DM4           ;     /* 1080 */
        struct {
            WORD M64         :1;     /* 1080 */
            WORD M65         :1;     /* 1081 */
            WORD M66         :1;     /* 1082 */
            WORD M67         :1;     /* 1083 */
            WORD M68         :1;     /* 1084 */
            WORD M69         :1;     /* 1085 */
            WORD M70         :1;     /* 1086 */
            WORD M71         :1;     /* 1087 */
            WORD M72         :1;     /* 1088 */
            WORD M73         :1;     /* 1089 */
            WORD M74         :1;     /* 108A */
            WORD M75         :1;     /* 108B */
            WORD M76         :1;     /* 108C */
            WORD M77         :1;     /* 108D */
            WORD M78         :1;     /* 108E */
            WORD M79         :1;     /* 108F */
        };
    };
    union {
        WORD DM5           ;     /* 1090 */
        struct {
            WORD M80         :1;     /* 1090 */
            WORD M81         :1;     /* 1091 */
            WORD M82         :1;     /* 1092 */
            WORD M83         :1;     /* 1093 */
            WORD M84         :1;     /* 1094 */
            WORD M85         :1;     /* 1095 */
            WORD M86         :1;     /* 1096 */
            WORD M87         :1;     /* 1097 */
            WORD M88         :1;     /* 1098 */
            WORD M89         :1;     /* 1099 */
            WORD M90         :1;     /* 109A */
            WORD M91         :1;     /* 109B */
            WORD M92         :1;     /* 109C */
            WORD M93         :1;     /* 109D */
            WORD M94         :1;     /* 109E */
            WORD M95         :1;     /* 109F */
        };
    };
    union {
        WORD DM6           ;     /* 10A0 */
        struct {
            WORD M96         :1;     /* 10A0 */
            WORD M97         :1;     /* 10A1 */
            WORD M98         :1;     /* 10A2 */
            WORD M99         :1;     /* 10A3 */
            WORD M100        :1;     /* 10A4 */
            WORD M101        :1;     /* 10A5 */
            WORD M102        :1;     /* 10A6 */
            WORD M103        :1;     /* 10A7 */
            WORD M104        :1;     /* 10A8 */
            WORD M105        :1;     /* 10A9 */
            WORD M106        :1;     /* 10AA */
            WORD M107        :1;     /* 10AB */
            WORD M108        :1;     /* 10AC */
            WORD M109        :1;     /* 10AD */
            WORD M110        :1;     /* 10AE */
            WORD M111        :1;     /* 10AF */
        };
    };
    union {
        WORD DM7           ;     /* 10B0 */
        struct {
            WORD M112        :1;     /* 10B0 */
            WORD M113        :1;     /* 10B1 */
            WORD M114        :1;     /* 10B2 */
            WORD M115        :1;     /* 10B3 */
            WORD M116        :1;     /* 10B4 */
            WORD M117        :1;     /* 10B5 */
            WORD M118        :1;     /* 10B6 */
            WORD M119        :1;     /* 10B7 */
            WORD M120        :1;     /* 10B8 */
            WORD M121        :1;     /* 10B9 */
            WORD M122        :1;     /* 10BA */
            WORD M123        :1;     /* 10BB */
            WORD M124        :1;     /* 10BC */
            WORD M125        :1;     /* 10BD */
            WORD M126        :1;     /* 10BE */
            WORD M127        :1;     /* 10BF */
        };
    };
    union {
        WORD DB0           ;     /* 10C0 */
        struct {
            WORD TEN0        :1;     /* 10C0 */
            WORD TEN1        :1;     /* 10C1 */
            WORD TEN2        :1;     /* 10C2 */
            WORD TEN3        :1;     /* 10C3 */
            WORD TEN4        :1;     /* 10C4 */
            WORD TEN5        :1;     /* 10C5 */
            WORD TEN6        :1;     /* 10C6 */
            WORD TEN7        :1;     /* 10C7 */
            WORD TDM0        :1;     /* 10C8 */
            WORD TDM1        :1;     /* 10C9 */
            WORD TDM2        :1;     /* 10CA */
            WORD TDM3        :1;     /* 10CB */
            WORD TDM4        :1;     /* 10CC */
            WORD TDM5        :1;     /* 10CD */
            WORD TDM6        :1;     /* 10CE */
            WORD TDM7        :1;     /* 10CF */
        };
        struct {
            WORD B0          :1;     /* 10C0 */
            WORD B1          :1;     /* 10C1 */
            WORD B2          :1;     /* 10C2 */
            WORD B3          :1;     /* 10C3 */
            WORD B4          :1;     /* 10C4 */
            WORD B5          :1;     /* 10C5 */
            WORD B6          :1;     /* 10C6 */
            WORD B7          :1;     /* 10C7 */
            WORD B8          :1;     /* 10C8 */
            WORD B9          :1;     /* 10C9 */
            WORD B10         :1;     /* 10CA */
            WORD B11         :1;     /* 10CB */
            WORD B12         :1;     /* 10CC */
            WORD B13         :1;     /* 10CD */
            WORD B14         :1;     /* 10CE */
            WORD B15         :1;     /* 10CF */
        };
    };
    union {
        WORD DB1           ;     /* 10D0 */
        struct {
            WORD TPA0        :1;     /* 10D0 */
            WORD TPA1        :1;     /* 10D1 */
            WORD TPA2        :1;     /* 10D2 */
            WORD TPA3        :1;     /* 10D3 */
            WORD TPA4        :1;     /* 10D4 */
            WORD TPA5        :1;     /* 10D5 */
            WORD TPA6        :1;     /* 10D6 */
            WORD TPA7        :1;     /* 10D7 */
            WORD TOE0        :1;     /* 10D8 */
            WORD TOE1        :1;     /* 10D9 */
            WORD TOE2        :1;     /* 10DA */
            WORD TOE3        :1;     /* 10DB */
            WORD TOE4        :1;     /* 10DC */
            WORD TOE5        :1;     /* 10DD */
            WORD TOE6        :1;     /* 10DE */
            WORD TOE7        :1;     /* 10DF */
        };
        struct {
            WORD B16         :1;     /* 10D0 */
            WORD B17         :1;     /* 10D1 */
            WORD B18         :1;     /* 10D2 */
            WORD B19         :1;     /* 10D3 */
            WORD B20         :1;     /* 10D4 */
            WORD B21         :1;     /* 10D5 */
            WORD B22         :1;     /* 10D6 */
            WORD B23         :1;     /* 10D7 */
            WORD B24         :1;     /* 10D8 */
            WORD B25         :1;     /* 10D9 */
            WORD B26         :1;     /* 10DA */
            WORD B27         :1;     /* 10DB */
            WORD B28         :1;     /* 10DC */
            WORD B29         :1;     /* 10DD */
            WORD B30         :1;     /* 10DE */
            WORD B31         :1;     /* 10DF */
        };
    };
    union {
        WORD DB2           ;     /* 10E0 */
        struct {
            WORD TOF0        :1;     /* 10E0 */
            WORD TOF1        :1;     /* 10E1 */
            WORD TOF2        :1;     /* 10E2 */
            WORD TOF3        :1;     /* 10E3 */
            WORD TOF4        :1;     /* 10E4 */
            WORD TOF5        :1;     /* 10E5 */
            WORD TOF6        :1;     /* 10E6 */
            WORD TOF7        :1;     /* 10E7 */
            WORD CLK0        :1;     /* 10E8 */
            WORD CLK1        :1;     /* 10E9 */
            WORD CLK2        :1;     /* 10EA */
            WORD CLK3        :1;     /* 10EB */
            WORD CLK4        :1;     /* 10EC */
            WORD CLK5        :1;     /* 10ED */
            WORD CLK6        :1;     /* 10EE */
            WORD CLK7        :1;     /* 10EF */
        };
        struct {
            WORD B32         :1;     /* 10E0 */
            WORD B33         :1;     /* 10E1 */
            WORD B34         :1;     /* 10E2 */
            WORD B35         :1;     /* 10E3 */
            WORD B36         :1;     /* 10E4 */
            WORD B37         :1;     /* 10E5 */
            WORD B38         :1;     /* 10E6 */
            WORD B39         :1;     /* 10E7 */
            WORD B40         :1;     /* 10E8 */
            WORD B41         :1;     /* 10E9 */
            WORD B42         :1;     /* 10EA */
            WORD B43         :1;     /* 10EB */
            WORD B44         :1;     /* 10EC */
            WORD B45         :1;     /* 10ED */
            WORD B46         :1;     /* 10EE */
            WORD B47         :1;     /* 10EF */
        };
    };
    union {
        WORD DB3           ;     /* 10F0 */
        struct {
            WORD HOLD        :1;     /* 10F0 */
            WORD CLRSEC      :1;     /* 10F1 */
            WORD KBREPEN     :1;     /* 10F2 */
            WORD KBSOUND     :1;     /* 10F3 */
            WORD LOADGEN     :1;     /* 10F4 */
            WORD FASTADC     :1;     /* 10F5 */
            WORD EESAVE      :1;     /* 10F6 */
            WORD EELOAD      :1;     /* 10F7 */
            WORD B56         :1;     /* 10F8 */
            WORD SENDBIT     :1;     /* 10F9 */
            WORD CALIBSAVE   :1;     /* 10FA */
            WORD SLAVEMODE   :1;     /* 10FB */
            WORD B60         :1;     /* 10FC */
            WORD B61         :1;     /* 10FD */
            WORD B62         :1;     /* 10FE */
            WORD B63         :1;     /* 10FF */
        };
        struct {
            WORD B48         :1;     /* 10F0 */
            WORD B49         :1;     /* 10F1 */
            WORD B50         :1;     /* 10F2 */
            WORD B51         :1;     /* 10F3 */
            WORD B52         :1;     /* 10F4 */
            WORD B53         :1;     /* 10F5 */
            WORD B54         :1;     /* 10F6 */
            WORD B55         :1;     /* 10F7 */
            WORD UB00        :1;     /* 10F8 */
            WORD B57         :1;     /* 10F9 */
            WORD B58         :1;     /* 10FA */
            WORD B59         :1;     /* 10FB */
            WORD UB01        :1;     /* 10FC */
            WORD UB02        :1;     /* 10FD */
            WORD UB03        :1;     /* 10FE */
            WORD UB04        :1;     /* 10FF */
        };
    };
    union {
        WORD DB4           ;     /* 1100 */
        struct {
            WORD B64         :1;     /* 1100 */
            WORD B65         :1;     /* 1101 */
            WORD B66         :1;     /* 1102 */
            WORD B67         :1;     /* 1103 */
            WORD B68         :1;     /* 1104 */
            WORD B69         :1;     /* 1105 */
            WORD B70         :1;     /* 1106 */
            WORD B71         :1;     /* 1107 */
            WORD B72         :1;     /* 1108 */
            WORD B73         :1;     /* 1109 */
            WORD B74         :1;     /* 110A */
            WORD B75         :1;     /* 110B */
            WORD B76         :1;     /* 110C */
            WORD B77         :1;     /* 110D */
            WORD B78         :1;     /* 110E */
            WORD B79         :1;     /* 110F */
        };
    };
    union {
        WORD DB5           ;     /* 1110 */
        struct {
            WORD B80         :1;     /* 1110 */
            WORD B81         :1;     /* 1111 */
            WORD B82         :1;     /* 1112 */
            WORD B83         :1;     /* 1113 */
            WORD B84         :1;     /* 1114 */
            WORD B85         :1;     /* 1115 */
            WORD B86         :1;     /* 1116 */
            WORD B87         :1;     /* 1117 */
            WORD B88         :1;     /* 1118 */
            WORD B89         :1;     /* 1119 */
            WORD B90         :1;     /* 111A */
            WORD B91         :1;     /* 111B */
            WORD B92         :1;     /* 111C */
            WORD B93         :1;     /* 111D */
            WORD B94         :1;     /* 111E */
            WORD B95         :1;     /* 111F */
        };
    };
    union {
        WORD DB6           ;     /* 1120 */
        struct {
            WORD B96         :1;     /* 1120 */
            WORD B97         :1;     /* 1121 */
            WORD B98         :1;     /* 1122 */
            WORD B99         :1;     /* 1123 */
            WORD B100        :1;     /* 1124 */
            WORD B101        :1;     /* 1125 */
            WORD B102        :1;     /* 1126 */
            WORD B103        :1;     /* 1127 */
            WORD B104        :1;     /* 1128 */
            WORD B105        :1;     /* 1129 */
            WORD B106        :1;     /* 112A */
            WORD B107        :1;     /* 112B */
            WORD B108        :1;     /* 112C */
            WORD B109        :1;     /* 112D */
            WORD B110        :1;     /* 112E */
            WORD B111        :1;     /* 112F */
        };
    };
    union {
        WORD DB7           ;     /* 1130 */
        struct {
            WORD B112        :1;     /* 1130 */
            WORD B113        :1;     /* 1131 */
            WORD B114        :1;     /* 1132 */
            WORD B115        :1;     /* 1133 */
            WORD B116        :1;     /* 1134 */
            WORD B117        :1;     /* 1135 */
            WORD B118        :1;     /* 1136 */
            WORD B119        :1;     /* 1137 */
            WORD B120        :1;     /* 1138 */
            WORD B121        :1;     /* 1139 */
            WORD B122        :1;     /* 113A */
            WORD B123        :1;     /* 113B */
            WORD B124        :1;     /* 113C */
            WORD TURBO       :1;     /* 113D */
            WORD RESET       :1;     /* 113E */
            WORD B127        :1;     /* 113F */
        };
        struct {
            WORD UB05        :1;     /* 1130 */
            WORD UB06        :1;     /* 1131 */
            WORD UB07        :1;     /* 1132 */
            WORD UB08        :1;     /* 1133 */
            WORD UB09        :1;     /* 1134 */
            WORD UB0A        :1;     /* 1135 */
            WORD UB0B        :1;     /* 1136 */
            WORD UB0C        :1;     /* 1137 */
            WORD UB0D        :1;     /* 1138 */
            WORD UB0E        :1;     /* 1139 */
            WORD UB0F        :1;     /* 113A */
            WORD UB10        :1;     /* 113B */
            WORD UB11        :1;     /* 113C */
            WORD B125        :1;     /* 113D */
            WORD B126        :1;     /* 113E */
            WORD UB12        :1;     /* 113F */
        };
    };
    WORD UW00[2];
    WORD I32           ;     /* 1160 */
    WORD I33           ;     /* 1170 */
    WORD I34           ;     /* 1180 */
    WORD I35           ;     /* 1190 */
    WORD I36           ;     /* 11A0 */
    WORD I37           ;     /* 11B0 */
    WORD I38           ;     /* 11C0 */
    WORD I39           ;     /* 11D0 */
    WORD I40           ;     /* 11E0 */
    WORD I41           ;     /* 11F0 */
    WORD I42           ;     /* 1200 */
    WORD I43           ;     /* 1210 */
    WORD I44           ;     /* 1220 */
    WORD I45           ;     /* 1230 */
    WORD I46           ;     /* 1240 */
    WORD I47           ;     /* 1250 */
    WORD I48           ;     /* 1260 */
    WORD I49           ;     /* 1270 */
    WORD I50           ;     /* 1280 */
    WORD I51           ;     /* 1290 */
    WORD I52           ;     /* 12A0 */
    WORD I53           ;     /* 12B0 */
    WORD I54           ;     /* 12C0 */
    WORD I55           ;     /* 12D0 */
    WORD I56           ;     /* 12E0 */
    WORD I57           ;     /* 12F0 */
    WORD I58           ;     /* 1300 */
    WORD I59           ;     /* 1310 */
    WORD I60           ;     /* 1320 */
    WORD I61           ;     /* 1330 */
    WORD I62           ;     /* 1340 */
    WORD I63           ;     /* 1350 */
    WORD O32           ;     /* 1360 */
    WORD O33           ;     /* 1370 */
    WORD O34           ;     /* 1380 */
    WORD O35           ;     /* 1390 */
    WORD O36           ;     /* 13A0 */
    WORD O37           ;     /* 13B0 */
    WORD O38           ;     /* 13C0 */
    WORD O39           ;     /* 13D0 */
    WORD O40           ;     /* 13E0 */
    WORD O41           ;     /* 13F0 */
    WORD O42           ;     /* 1400 */
    WORD O43           ;     /* 1410 */
    WORD O44           ;     /* 1420 */
    WORD O45           ;     /* 1430 */
    WORD O46           ;     /* 1440 */
    WORD O47           ;     /* 1450 */
    WORD O48           ;     /* 1460 */
    WORD O49           ;     /* 1470 */
    WORD O50           ;     /* 1480 */
    WORD O51           ;     /* 1490 */
    WORD O52           ;     /* 14A0 */
    WORD O53           ;     /* 14B0 */
    WORD O54           ;     /* 14C0 */
    WORD O55           ;     /* 14D0 */
    WORD O56           ;     /* 14E0 */
    WORD O57           ;     /* 14F0 */
    WORD O58           ;     /* 1500 */
    WORD O59           ;     /* 1510 */
    WORD O60           ;     /* 1520 */
    WORD O61           ;     /* 1530 */
    WORD O62           ;     /* 1540 */
    WORD O63           ;     /* 1550 */
    WORD UW01[6];
    WORD W128          ;     /* 15C0 */
    WORD W129          ;     /* 15D0 */
    WORD W130          ;     /* 15E0 */
    WORD W131          ;     /* 15F0 */
    WORD W132          ;     /* 1600 */
    WORD W133          ;     /* 1610 */
    WORD W134          ;     /* 1620 */
    WORD W135          ;     /* 1630 */
    WORD W136          ;     /* 1640 */
    WORD W137          ;     /* 1650 */
    WORD W138          ;     /* 1660 */
    WORD W139          ;     /* 1670 */
    WORD W140          ;     /* 1680 */
    WORD W141          ;     /* 1690 */
    WORD W142          ;     /* 16A0 */
    WORD W143          ;     /* 16B0 */
    WORD W144          ;     /* 16C0 */
    WORD W145          ;     /* 16D0 */
    WORD W146          ;     /* 16E0 */
    WORD W147          ;     /* 16F0 */
    WORD W148          ;     /* 1700 */
    WORD W149          ;     /* 1710 */
    WORD W150          ;     /* 1720 */
    WORD W151          ;     /* 1730 */
    WORD W152          ;     /* 1740 */
    WORD W153          ;     /* 1750 */
    WORD W154          ;     /* 1760 */
    WORD W155          ;     /* 1770 */
    WORD W156          ;     /* 1780 */
    WORD W157          ;     /* 1790 */
    WORD W158          ;     /* 17A0 */
    WORD W159          ;     /* 17B0 */
    WORD W160          ;     /* 17C0 */
    WORD W161          ;     /* 17D0 */
    WORD W162          ;     /* 17E0 */
    WORD W163          ;     /* 17F0 */
    WORD W164          ;     /* 1800 */
    WORD W165          ;     /* 1810 */
    WORD W166          ;     /* 1820 */
    WORD W167          ;     /* 1830 */
    WORD W168          ;     /* 1840 */
    WORD W169          ;     /* 1850 */
    WORD W170          ;     /* 1860 */
    WORD W171          ;     /* 1870 */
    WORD W172          ;     /* 1880 */
    WORD W173          ;     /* 1890 */
    WORD W174          ;     /* 18A0 */
    WORD W175          ;     /* 18B0 */
    WORD W176          ;     /* 18C0 */
    WORD W177          ;     /* 18D0 */
    WORD W178          ;     /* 18E0 */
    WORD W179          ;     /* 18F0 */
    WORD W180          ;     /* 1900 */
    WORD W181          ;     /* 1910 */
    WORD W182          ;     /* 1920 */
    WORD W183          ;     /* 1930 */
    WORD W184          ;     /* 1940 */
    WORD W185          ;     /* 1950 */
    WORD W186          ;     /* 1960 */
    WORD W187          ;     /* 1970 */
    WORD W188          ;     /* 1980 */
    WORD W189          ;     /* 1990 */
    WORD W190          ;     /* 19A0 */
    WORD W191          ;     /* 19B0 */
    WORD W192          ;     /* 19C0 */
    WORD W193          ;     /* 19D0 */
    WORD W194          ;     /* 19E0 */
    WORD W195          ;     /* 19F0 */
    WORD W196          ;     /* 1A00 */
    WORD W197          ;     /* 1A10 */
    WORD W198          ;     /* 1A20 */
    WORD W199          ;     /* 1A30 */
    WORD W200          ;     /* 1A40 */
    WORD W201          ;     /* 1A50 */
    WORD W202          ;     /* 1A60 */
    WORD W203          ;     /* 1A70 */
    WORD W204          ;     /* 1A80 */
    WORD W205          ;     /* 1A90 */
    WORD W206          ;     /* 1AA0 */
    WORD W207          ;     /* 1AB0 */
    WORD W208          ;     /* 1AC0 */
    WORD W209          ;     /* 1AD0 */
    WORD W210          ;     /* 1AE0 */
    WORD W211          ;     /* 1AF0 */
    WORD W212          ;     /* 1B00 */
    WORD W213          ;     /* 1B10 */
    WORD W214          ;     /* 1B20 */
    WORD W215          ;     /* 1B30 */
    WORD W216          ;     /* 1B40 */
    WORD W217          ;     /* 1B50 */
    WORD W218          ;     /* 1B60 */
    WORD W219          ;     /* 1B70 */
    WORD W220          ;     /* 1B80 */
    WORD W221          ;     /* 1B90 */
    WORD W222          ;     /* 1BA0 */
    WORD W223          ;     /* 1BB0 */
    WORD W224          ;     /* 1BC0 */
    WORD W225          ;     /* 1BD0 */
    WORD W226          ;     /* 1BE0 */
    WORD W227          ;     /* 1BF0 */
    WORD W228          ;     /* 1C00 */
    WORD W229          ;     /* 1C10 */
    WORD W230          ;     /* 1C20 */
    WORD W231          ;     /* 1C30 */
    WORD W232          ;     /* 1C40 */
    WORD W233          ;     /* 1C50 */
    WORD W234          ;     /* 1C60 */
    WORD W235          ;     /* 1C70 */
    WORD W236          ;     /* 1C80 */
    WORD W237          ;     /* 1C90 */
    WORD W238          ;     /* 1CA0 */
    WORD W239          ;     /* 1CB0 */
    WORD W240          ;     /* 1CC0 */
    WORD W241          ;     /* 1CD0 */
    WORD W242          ;     /* 1CE0 */
    WORD W243          ;     /* 1CF0 */
    WORD W244          ;     /* 1D00 */
    WORD W245          ;     /* 1D10 */
    WORD W246          ;     /* 1D20 */
    WORD W247          ;     /* 1D30 */
    union {
        WORD DX2           ;     /* 1D40 */
        WORD W248          ;     /* 1D40 */
    };
    union {
        WORD DX3           ;     /* 1D50 */
        WORD W249          ;     /* 1D50 */
    };
    union {
        WORD DX4           ;     /* 1D60 */
        WORD W250          ;     /* 1D60 */
        struct {
            WORD X32         :1;     /* 1D60 */
            WORD X33         :1;     /* 1D61 */
            WORD X34         :1;     /* 1D62 */
            WORD X35         :1;     /* 1D63 */
            WORD X36         :1;     /* 1D64 */
            WORD X37         :1;     /* 1D65 */
            WORD X38         :1;     /* 1D66 */
            WORD X39         :1;     /* 1D67 */
            WORD X40         :1;     /* 1D68 */
            WORD X41         :1;     /* 1D69 */
            WORD X42         :1;     /* 1D6A */
            WORD X43         :1;     /* 1D6B */
            WORD X44         :1;     /* 1D6C */
            WORD X45         :1;     /* 1D6D */
            WORD X46         :1;     /* 1D6E */
            WORD X47         :1;     /* 1D6F */
        };
    };
    union {
        WORD DX5           ;     /* 1D70 */
        WORD W251          ;     /* 1D70 */
        struct {
            WORD X48         :1;     /* 1D70 */
            WORD X49         :1;     /* 1D71 */
            WORD X50         :1;     /* 1D72 */
            WORD X51         :1;     /* 1D73 */
            WORD X52         :1;     /* 1D74 */
            WORD X53         :1;     /* 1D75 */
            WORD X54         :1;     /* 1D76 */
            WORD X55         :1;     /* 1D77 */
            WORD X56         :1;     /* 1D78 */
            WORD X57         :1;     /* 1D79 */
            WORD X58         :1;     /* 1D7A */
            WORD X59         :1;     /* 1D7B */
            WORD X60         :1;     /* 1D7C */
            WORD X61         :1;     /* 1D7D */
            WORD X62         :1;     /* 1D7E */
            WORD X63         :1;     /* 1D7F */
        };
    };
    union {
        WORD DX6           ;     /* 1D80 */
        WORD W252          ;     /* 1D80 */
        struct {
            WORD X64         :1;     /* 1D80 */
            WORD X65         :1;     /* 1D81 */
            WORD X66         :1;     /* 1D82 */
            WORD X67         :1;     /* 1D83 */
            WORD X68         :1;     /* 1D84 */
            WORD X69         :1;     /* 1D85 */
            WORD X70         :1;     /* 1D86 */
            WORD X71         :1;     /* 1D87 */
            WORD X72         :1;     /* 1D88 */
            WORD X73         :1;     /* 1D89 */
            WORD X74         :1;     /* 1D8A */
            WORD X75         :1;     /* 1D8B */
            WORD X76         :1;     /* 1D8C */
            WORD X77         :1;     /* 1D8D */
            WORD X78         :1;     /* 1D8E */
            WORD X79         :1;     /* 1D8F */
        };
    };
    union {
        WORD DX7           ;     /* 1D90 */
        WORD W253          ;     /* 1D90 */
        struct {
            WORD X80         :1;     /* 1D90 */
            WORD X81         :1;     /* 1D91 */
            WORD X82         :1;     /* 1D92 */
            WORD X83         :1;     /* 1D93 */
            WORD X84         :1;     /* 1D94 */
            WORD X85         :1;     /* 1D95 */
            WORD X86         :1;     /* 1D96 */
            WORD X87         :1;     /* 1D97 */
            WORD X88         :1;     /* 1D98 */
            WORD X89         :1;     /* 1D99 */
            WORD X90         :1;     /* 1D9A */
            WORD X91         :1;     /* 1D9B */
            WORD X92         :1;     /* 1D9C */
            WORD X93         :1;     /* 1D9D */
            WORD X94         :1;     /* 1D9E */
            WORD X95         :1;     /* 1D9F */
        };
    };
    union {
        WORD DX8           ;     /* 1DA0 */
        WORD W254          ;     /* 1DA0 */
        struct {
            WORD X96         :1;     /* 1DA0 */
            WORD X97         :1;     /* 1DA1 */
            WORD X98         :1;     /* 1DA2 */
            WORD X99         :1;     /* 1DA3 */
            WORD X100        :1;     /* 1DA4 */
            WORD X101        :1;     /* 1DA5 */
            WORD X102        :1;     /* 1DA6 */
            WORD X103        :1;     /* 1DA7 */
            WORD X104        :1;     /* 1DA8 */
            WORD X105        :1;     /* 1DA9 */
            WORD X106        :1;     /* 1DAA */
            WORD X107        :1;     /* 1DAB */
            WORD X108        :1;     /* 1DAC */
            WORD X109        :1;     /* 1DAD */
            WORD X110        :1;     /* 1DAE */
            WORD X111        :1;     /* 1DAF */
        };
    };
    union {
        WORD DX9           ;     /* 1DB0 */
        WORD W255          ;     /* 1DB0 */
        struct {
            WORD X112        :1;     /* 1DB0 */
            WORD X113        :1;     /* 1DB1 */
            WORD X114        :1;     /* 1DB2 */
            WORD X115        :1;     /* 1DB3 */
            WORD X116        :1;     /* 1DB4 */
            WORD X117        :1;     /* 1DB5 */
            WORD X118        :1;     /* 1DB6 */
            WORD X119        :1;     /* 1DB7 */
            WORD X120        :1;     /* 1DB8 */
            WORD X121        :1;     /* 1DB9 */
            WORD X122        :1;     /* 1DBA */
            WORD X123        :1;     /* 1DBB */
            WORD X124        :1;     /* 1DBC */
            WORD X125        :1;     /* 1DBD */
            WORD X126        :1;     /* 1DBE */
            WORD X127        :1;     /* 1DBF */
        };
    };
    union {
        WORD DX10          ;     /* 1DC0 */
        struct {
            WORD X128        :1;     /* 1DC0 */
            WORD X129        :1;     /* 1DC1 */
            WORD X130        :1;     /* 1DC2 */
            WORD X131        :1;     /* 1DC3 */
            WORD X132        :1;     /* 1DC4 */
            WORD X133        :1;     /* 1DC5 */
            WORD X134        :1;     /* 1DC6 */
            WORD X135        :1;     /* 1DC7 */
            WORD X136        :1;     /* 1DC8 */
            WORD X137        :1;     /* 1DC9 */
            WORD X138        :1;     /* 1DCA */
            WORD X139        :1;     /* 1DCB */
            WORD X140        :1;     /* 1DCC */
            WORD X141        :1;     /* 1DCD */
            WORD X142        :1;     /* 1DCE */
            WORD X143        :1;     /* 1DCF */
        };
    };
    union {
        WORD DX11          ;     /* 1DD0 */
        struct {
            WORD X144        :1;     /* 1DD0 */
            WORD X145        :1;     /* 1DD1 */
            WORD X146        :1;     /* 1DD2 */
            WORD X147        :1;     /* 1DD3 */
            WORD X148        :1;     /* 1DD4 */
            WORD X149        :1;     /* 1DD5 */
            WORD X150        :1;     /* 1DD6 */
            WORD X151        :1;     /* 1DD7 */
            WORD X152        :1;     /* 1DD8 */
            WORD X153        :1;     /* 1DD9 */
            WORD X154        :1;     /* 1DDA */
            WORD X155        :1;     /* 1DDB */
            WORD X156        :1;     /* 1DDC */
            WORD X157        :1;     /* 1DDD */
            WORD X158        :1;     /* 1DDE */
            WORD X159        :1;     /* 1DDF */
        };
    };
    union {
        WORD DX12          ;     /* 1DE0 */
        struct {
            WORD X160        :1;     /* 1DE0 */
            WORD X161        :1;     /* 1DE1 */
            WORD X162        :1;     /* 1DE2 */
            WORD X163        :1;     /* 1DE3 */
            WORD X164        :1;     /* 1DE4 */
            WORD X165        :1;     /* 1DE5 */
            WORD X166        :1;     /* 1DE6 */
            WORD X167        :1;     /* 1DE7 */
            WORD X168        :1;     /* 1DE8 */
            WORD X169        :1;     /* 1DE9 */
            WORD X170        :1;     /* 1DEA */
            WORD X171        :1;     /* 1DEB */
            WORD X172        :1;     /* 1DEC */
            WORD X173        :1;     /* 1DED */
            WORD X174        :1;     /* 1DEE */
            WORD X175        :1;     /* 1DEF */
        };
    };
    union {
        WORD DX13          ;     /* 1DF0 */
        struct {
            WORD X176        :1;     /* 1DF0 */
            WORD X177        :1;     /* 1DF1 */
            WORD X178        :1;     /* 1DF2 */
            WORD X179        :1;     /* 1DF3 */
            WORD X180        :1;     /* 1DF4 */
            WORD X181        :1;     /* 1DF5 */
            WORD X182        :1;     /* 1DF6 */
            WORD X183        :1;     /* 1DF7 */
            WORD X184        :1;     /* 1DF8 */
            WORD X185        :1;     /* 1DF9 */
            WORD X186        :1;     /* 1DFA */
            WORD X187        :1;     /* 1DFB */
            WORD X188        :1;     /* 1DFC */
            WORD X189        :1;     /* 1DFD */
            WORD X190        :1;     /* 1DFE */
            WORD X191        :1;     /* 1DFF */
        };
    };
    union {
        WORD DX14          ;     /* 1E00 */
        struct {
            WORD X192        :1;     /* 1E00 */
            WORD X193        :1;     /* 1E01 */
            WORD X194        :1;     /* 1E02 */
            WORD X195        :1;     /* 1E03 */
            WORD X196        :1;     /* 1E04 */
            WORD X197        :1;     /* 1E05 */
            WORD X198        :1;     /* 1E06 */
            WORD X199        :1;     /* 1E07 */
            WORD X200        :1;     /* 1E08 */
            WORD X201        :1;     /* 1E09 */
            WORD X202        :1;     /* 1E0A */
            WORD X203        :1;     /* 1E0B */
            WORD X204        :1;     /* 1E0C */
            WORD X205        :1;     /* 1E0D */
            WORD X206        :1;     /* 1E0E */
            WORD X207        :1;     /* 1E0F */
        };
    };
    union {
        WORD DX15          ;     /* 1E10 */
        struct {
            WORD X208        :1;     /* 1E10 */
            WORD X209        :1;     /* 1E11 */
            WORD X210        :1;     /* 1E12 */
            WORD X211        :1;     /* 1E13 */
            WORD X212        :1;     /* 1E14 */
            WORD X213        :1;     /* 1E15 */
            WORD X214        :1;     /* 1E16 */
            WORD X215        :1;     /* 1E17 */
            WORD X216        :1;     /* 1E18 */
            WORD X217        :1;     /* 1E19 */
            WORD X218        :1;     /* 1E1A */
            WORD X219        :1;     /* 1E1B */
            WORD X220        :1;     /* 1E1C */
            WORD X221        :1;     /* 1E1D */
            WORD X222        :1;     /* 1E1E */
            WORD X223        :1;     /* 1E1F */
        };
    };
    union {
        WORD DY2           ;     /* 1E20 */
        struct {
            WORD X224        :1;     /* 1E20 */
            WORD X225        :1;     /* 1E21 */
            WORD X226        :1;     /* 1E22 */
            WORD X227        :1;     /* 1E23 */
            WORD X228        :1;     /* 1E24 */
            WORD X229        :1;     /* 1E25 */
            WORD X230        :1;     /* 1E26 */
            WORD X231        :1;     /* 1E27 */
            WORD X232        :1;     /* 1E28 */
            WORD X233        :1;     /* 1E29 */
            WORD X234        :1;     /* 1E2A */
            WORD X235        :1;     /* 1E2B */
            WORD X236        :1;     /* 1E2C */
            WORD X237        :1;     /* 1E2D */
            WORD X238        :1;     /* 1E2E */
            WORD X239        :1;     /* 1E2F */
        };
    };
    union {
        WORD DY3           ;     /* 1E30 */
        struct {
            WORD X240        :1;     /* 1E30 */
            WORD X241        :1;     /* 1E31 */
            WORD X242        :1;     /* 1E32 */
            WORD X243        :1;     /* 1E33 */
            WORD X244        :1;     /* 1E34 */
            WORD X245        :1;     /* 1E35 */
            WORD X246        :1;     /* 1E36 */
            WORD X247        :1;     /* 1E37 */
            WORD X248        :1;     /* 1E38 */
            WORD X249        :1;     /* 1E39 */
            WORD X250        :1;     /* 1E3A */
            WORD X251        :1;     /* 1E3B */
            WORD X252        :1;     /* 1E3C */
            WORD X253        :1;     /* 1E3D */
            WORD X254        :1;     /* 1E3E */
            WORD X255        :1;     /* 1E3F */
        };
    };
    union {
        WORD DY4           ;     /* 1E40 */
        struct {
            WORD Y32         :1;     /* 1E40 */
            WORD Y33         :1;     /* 1E41 */
            WORD Y34         :1;     /* 1E42 */
            WORD Y35         :1;     /* 1E43 */
            WORD Y36         :1;     /* 1E44 */
            WORD Y37         :1;     /* 1E45 */
            WORD Y38         :1;     /* 1E46 */
            WORD Y39         :1;     /* 1E47 */
            WORD Y40         :1;     /* 1E48 */
            WORD Y41         :1;     /* 1E49 */
            WORD Y42         :1;     /* 1E4A */
            WORD Y43         :1;     /* 1E4B */
            WORD Y44         :1;     /* 1E4C */
            WORD Y45         :1;     /* 1E4D */
            WORD Y46         :1;     /* 1E4E */
            WORD Y47         :1;     /* 1E4F */
        };
    };
    union {
        WORD DY5           ;     /* 1E50 */
        struct {
            WORD Y48         :1;     /* 1E50 */
            WORD Y49         :1;     /* 1E51 */
            WORD Y50         :1;     /* 1E52 */
            WORD Y51         :1;     /* 1E53 */
            WORD Y52         :1;     /* 1E54 */
            WORD Y53         :1;     /* 1E55 */
            WORD Y54         :1;     /* 1E56 */
            WORD Y55         :1;     /* 1E57 */
            WORD Y56         :1;     /* 1E58 */
            WORD Y57         :1;     /* 1E59 */
            WORD Y58         :1;     /* 1E5A */
            WORD Y59         :1;     /* 1E5B */
            WORD Y60         :1;     /* 1E5C */
            WORD Y61         :1;     /* 1E5D */
            WORD Y62         :1;     /* 1E5E */
            WORD Y63         :1;     /* 1E5F */
        };
    };
    union {
        WORD DY6           ;     /* 1E60 */
        struct {
            WORD Y64         :1;     /* 1E60 */
            WORD Y65         :1;     /* 1E61 */
            WORD Y66         :1;     /* 1E62 */
            WORD Y67         :1;     /* 1E63 */
            WORD Y68         :1;     /* 1E64 */
            WORD Y69         :1;     /* 1E65 */
            WORD Y70         :1;     /* 1E66 */
            WORD Y71         :1;     /* 1E67 */
            WORD Y72         :1;     /* 1E68 */
            WORD Y73         :1;     /* 1E69 */
            WORD Y74         :1;     /* 1E6A */
            WORD Y75         :1;     /* 1E6B */
            WORD Y76         :1;     /* 1E6C */
            WORD Y77         :1;     /* 1E6D */
            WORD Y78         :1;     /* 1E6E */
            WORD Y79         :1;     /* 1E6F */
        };
    };
    union {
        WORD DY7           ;     /* 1E70 */
        struct {
            WORD Y80         :1;     /* 1E70 */
            WORD Y81         :1;     /* 1E71 */
            WORD Y82         :1;     /* 1E72 */
            WORD Y83         :1;     /* 1E73 */
            WORD Y84         :1;     /* 1E74 */
            WORD Y85         :1;     /* 1E75 */
            WORD Y86         :1;     /* 1E76 */
            WORD Y87         :1;     /* 1E77 */
            WORD Y88         :1;     /* 1E78 */
            WORD Y89         :1;     /* 1E79 */
            WORD Y90         :1;     /* 1E7A */
            WORD Y91         :1;     /* 1E7B */
            WORD Y92         :1;     /* 1E7C */
            WORD Y93         :1;     /* 1E7D */
            WORD Y94         :1;     /* 1E7E */
            WORD Y95         :1;     /* 1E7F */
        };
    };
    union {
        WORD DY8           ;     /* 1E80 */
        struct {
            WORD Y96         :1;     /* 1E80 */
            WORD Y97         :1;     /* 1E81 */
            WORD Y98         :1;     /* 1E82 */
            WORD Y99         :1;     /* 1E83 */
            WORD Y100        :1;     /* 1E84 */
            WORD Y101        :1;     /* 1E85 */
            WORD Y102        :1;     /* 1E86 */
            WORD Y103        :1;     /* 1E87 */
            WORD Y104        :1;     /* 1E88 */
            WORD Y105        :1;     /* 1E89 */
            WORD Y106        :1;     /* 1E8A */
            WORD Y107        :1;     /* 1E8B */
            WORD Y108        :1;     /* 1E8C */
            WORD Y109        :1;     /* 1E8D */
            WORD Y110        :1;     /* 1E8E */
            WORD Y111        :1;     /* 1E8F */
        };
    };
    union {
        WORD DY9           ;     /* 1E90 */
        struct {
            WORD Y112        :1;     /* 1E90 */
            WORD Y113        :1;     /* 1E91 */
            WORD Y114        :1;     /* 1E92 */
            WORD Y115        :1;     /* 1E93 */
            WORD Y116        :1;     /* 1E94 */
            WORD Y117        :1;     /* 1E95 */
            WORD Y118        :1;     /* 1E96 */
            WORD Y119        :1;     /* 1E97 */
            WORD Y120        :1;     /* 1E98 */
            WORD Y121        :1;     /* 1E99 */
            WORD Y122        :1;     /* 1E9A */
            WORD Y123        :1;     /* 1E9B */
            WORD Y124        :1;     /* 1E9C */
            WORD Y125        :1;     /* 1E9D */
            WORD Y126        :1;     /* 1E9E */
            WORD Y127        :1;     /* 1E9F */
        };
    };
    union {
        WORD DY10          ;     /* 1EA0 */
        struct {
            WORD Y128        :1;     /* 1EA0 */
            WORD Y129        :1;     /* 1EA1 */
            WORD Y130        :1;     /* 1EA2 */
            WORD Y131        :1;     /* 1EA3 */
            WORD Y132        :1;     /* 1EA4 */
            WORD Y133        :1;     /* 1EA5 */
            WORD Y134        :1;     /* 1EA6 */
            WORD Y135        :1;     /* 1EA7 */
            WORD Y136        :1;     /* 1EA8 */
            WORD Y137        :1;     /* 1EA9 */
            WORD Y138        :1;     /* 1EAA */
            WORD Y139        :1;     /* 1EAB */
            WORD Y140        :1;     /* 1EAC */
            WORD Y141        :1;     /* 1EAD */
            WORD Y142        :1;     /* 1EAE */
            WORD Y143        :1;     /* 1EAF */
        };
    };
    union {
        WORD DY11          ;     /* 1EB0 */
        struct {
            WORD Y144        :1;     /* 1EB0 */
            WORD Y145        :1;     /* 1EB1 */
            WORD Y146        :1;     /* 1EB2 */
            WORD Y147        :1;     /* 1EB3 */
            WORD Y148        :1;     /* 1EB4 */
            WORD Y149        :1;     /* 1EB5 */
            WORD Y150        :1;     /* 1EB6 */
            WORD Y151        :1;     /* 1EB7 */
            WORD Y152        :1;     /* 1EB8 */
            WORD Y153        :1;     /* 1EB9 */
            WORD Y154        :1;     /* 1EBA */
            WORD Y155        :1;     /* 1EBB */
            WORD Y156        :1;     /* 1EBC */
            WORD Y157        :1;     /* 1EBD */
            WORD Y158        :1;     /* 1EBE */
            WORD Y159        :1;     /* 1EBF */
        };
    };
    union {
        WORD DY12          ;     /* 1EC0 */
        struct {
            WORD Y160        :1;     /* 1EC0 */
            WORD Y161        :1;     /* 1EC1 */
            WORD Y162        :1;     /* 1EC2 */
            WORD Y163        :1;     /* 1EC3 */
            WORD Y164        :1;     /* 1EC4 */
            WORD Y165        :1;     /* 1EC5 */
            WORD Y166        :1;     /* 1EC6 */
            WORD Y167        :1;     /* 1EC7 */
            WORD Y168        :1;     /* 1EC8 */
            WORD Y169        :1;     /* 1EC9 */
            WORD Y170        :1;     /* 1ECA */
            WORD Y171        :1;     /* 1ECB */
            WORD Y172        :1;     /* 1ECC */
            WORD Y173        :1;     /* 1ECD */
            WORD Y174        :1;     /* 1ECE */
            WORD Y175        :1;     /* 1ECF */
        };
    };
    union {
        WORD DY13          ;     /* 1ED0 */
        struct {
            WORD Y176        :1;     /* 1ED0 */
            WORD Y177        :1;     /* 1ED1 */
            WORD Y178        :1;     /* 1ED2 */
            WORD Y179        :1;     /* 1ED3 */
            WORD Y180        :1;     /* 1ED4 */
            WORD Y181        :1;     /* 1ED5 */
            WORD Y182        :1;     /* 1ED6 */
            WORD Y183        :1;     /* 1ED7 */
            WORD Y184        :1;     /* 1ED8 */
            WORD Y185        :1;     /* 1ED9 */
            WORD Y186        :1;     /* 1EDA */
            WORD Y187        :1;     /* 1EDB */
            WORD Y188        :1;     /* 1EDC */
            WORD Y189        :1;     /* 1EDD */
            WORD Y190        :1;     /* 1EDE */
            WORD Y191        :1;     /* 1EDF */
        };
    };
    union {
        WORD DY14          ;     /* 1EE0 */
        struct {
            WORD Y192        :1;     /* 1EE0 */
            WORD Y193        :1;     /* 1EE1 */
            WORD Y194        :1;     /* 1EE2 */
            WORD Y195        :1;     /* 1EE3 */
            WORD Y196        :1;     /* 1EE4 */
            WORD Y197        :1;     /* 1EE5 */
            WORD Y198        :1;     /* 1EE6 */
            WORD Y199        :1;     /* 1EE7 */
            WORD Y200        :1;     /* 1EE8 */
            WORD Y201        :1;     /* 1EE9 */
            WORD Y202        :1;     /* 1EEA */
            WORD Y203        :1;     /* 1EEB */
            WORD Y204        :1;     /* 1EEC */
            WORD Y205        :1;     /* 1EED */
            WORD Y206        :1;     /* 1EEE */
            WORD Y207        :1;     /* 1EEF */
        };
    };
    union {
        WORD DY15          ;     /* 1EF0 */
        struct {
            WORD Y208        :1;     /* 1EF0 */
            WORD Y209        :1;     /* 1EF1 */
            WORD Y210        :1;     /* 1EF2 */
            WORD Y211        :1;     /* 1EF3 */
            WORD Y212        :1;     /* 1EF4 */
            WORD Y213        :1;     /* 1EF5 */
            WORD Y214        :1;     /* 1EF6 */
            WORD Y215        :1;     /* 1EF7 */
            WORD Y216        :1;     /* 1EF8 */
            WORD Y217        :1;     /* 1EF9 */
            WORD Y218        :1;     /* 1EFA */
            WORD Y219        :1;     /* 1EFB */
            WORD Y220        :1;     /* 1EFC */
            WORD Y221        :1;     /* 1EFD */
            WORD Y222        :1;     /* 1EFE */
            WORD Y223        :1;     /* 1EFF */
        };
    };
    union {
        WORD UW02;
        struct {
            WORD Y224        :1;     /* 1F00 */
            WORD Y225        :1;     /* 1F01 */
            WORD Y226        :1;     /* 1F02 */
            WORD Y227        :1;     /* 1F03 */
            WORD Y228        :1;     /* 1F04 */
            WORD Y229        :1;     /* 1F05 */
            WORD Y230        :1;     /* 1F06 */
            WORD Y231        :1;     /* 1F07 */
            WORD Y232        :1;     /* 1F08 */
            WORD Y233        :1;     /* 1F09 */
            WORD Y234        :1;     /* 1F0A */
            WORD Y235        :1;     /* 1F0B */
            WORD Y236        :1;     /* 1F0C */
            WORD Y237        :1;     /* 1F0D */
            WORD Y238        :1;     /* 1F0E */
            WORD Y239        :1;     /* 1F0F */
        };
    };
    WORD GAP[15];   /* Doplnit do 1kB */
};
#endif // PES_SYSTEM_H
