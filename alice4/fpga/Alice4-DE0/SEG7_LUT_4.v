module SEG7_LUT_4 (
    output [6:0]    oSEG0,
    output          oSEG0_DP,
    output [6:0]    oSEG1,
    output          oSEG1_DP,
    output [6:0]    oSEG2,
    output          oSEG2_DP,
    output [6:0]    oSEG3,
    output          oSEG3_DP,
    input [15:0]    iDIG
);

SEG7_LUT u0(oSEG0, oSEG0_DP, iDIG[3:0]);
SEG7_LUT u1(oSEG1, oSEG1_DP, iDIG[7:4]);
SEG7_LUT u2(oSEG2, oSEG2_DP, iDIG[11:8]);
SEG7_LUT u3(oSEG3, oSEG3_DP, iDIG[15:12]);

endmodule
