
module SDRAM_test(
    input           reset_n,                //  Module reset.
    inout   [15:0]  dram_dq,                //  SDRAM Data bus 16 Bits
    output  [12:0]  dram_addr,              //  SDRAM Address bus 13 Bits
    output          dram_ldqm,              //  SDRAM Low-byte Data Mask
    output          dram_udqm,              //  SDRAM High-byte Data Mask
    output          dram_we_n,              //  SDRAM Write Enable
    output          dram_cas_n,             //  SDRAM Column Address Strobe
    output          dram_ras_n,             //  SDRAM Row Address Strobe
    output          dram_cs_n,              //  SDRAM Chip Select
    output          dram_ba_0,              //  SDRAM Bank Address 0
    output          dram_ba_1,              //  SDRAM Bank Address 1
    input           dram_clk,               //  SDRAM Clock
    output          dram_cke,               //  SDRAM Clock Enable
    output  [15:0]  debug_number            //  16-bit number displayed on 7-segment display.
);

// States.
/// localparam STATE_INIT = 0;
/// localparam STATE_INIT = 0;

reg [31:0] counter;
reg [15:0] debug_number_reg;
assign debug_number = debug_number_reg;

initial begin
    counter <= 0;
    debug_number_reg <= 0;
end

always @(posedge dram_clk or negedge reset_n) begin
    if (!reset_n) begin
        counter <= 0;
        debug_number_reg <= 0;
    end else begin
        if (counter == 166000000) begin
            counter <= 0;
            debug_number_reg <= debug_number_reg + 1;
        end else begin
            counter <= counter + 1;
        end
    end
end

endmodule
 
