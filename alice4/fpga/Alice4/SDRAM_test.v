
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
    input           dram_clk_locked,        //  SDRAM Clock PLL is locked.
    output  [15:0]  debug_number            //  16-bit number displayed on 7-segment display.
);

// State machine.
localparam STATE_INIT = 0;
localparam STATE_READY = 1;
reg [3:0] state;
reg [3:0] state_nxt;

// Put some of the outputs into registers.
reg dram_ldqm_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_udqm_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_cs_n_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_cke_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_ldqm_reg_nxt;
reg dram_udqm_reg_nxt;
reg dram_cs_n_reg_nxt;
reg dram_cke_reg_nxt;
assign dram_ldqm = dram_ldqm_reg;
assign dram_udqm = dram_udqm_reg;
assign dram_cs_n = dram_cs_n_reg;
assign dram_cke = dram_cke_reg;

// Debugging.
reg [31:0] counter;
reg [15:0] seconds;
assign debug_number = { state[3:0], seconds[11:0] };

initial begin
    counter <= 0;
    seconds <= 0;

    state <= STATE_INIT;

    dram_cke_reg <= 0;
    dram_cs_n_reg <= 1;
    dram_ldqm_reg <= 1;
    dram_udqm_reg <= 1;
end

// Show a counter in seconds.
always @(posedge dram_clk or negedge reset_n) begin
    if (!reset_n) begin
        counter <= 0;
        seconds <= 0;
    end else begin
        if (counter == 166000000) begin
            counter <= 0;
            seconds <= seconds + 1;
        end else begin
            counter <= counter + 1;
        end
    end
end

// State machine.
always @(*) begin
    state_nxt <= state;
    dram_ldqm_reg_nxt <= dram_ldqm_reg;
    dram_udqm_reg_nxt <= dram_udqm_reg;
    dram_cs_n_reg_nxt <= dram_cs_n_reg;
    dram_cke_reg_nxt <= dram_cke_reg;

    case (state)
        STATE_INIT: begin
            // This state waits until the PLL has locked.
            if (dram_clk_locked) begin
                // Tell the SDRAM to pay attention to the clock.
                dram_cs_n_reg_nxt <= 0;
                dram_cke_reg_nxt <= 1;
                state_nxt <= STATE_READY;
            end
        end

        STATE_READY: begin
        end
    endcase
end

always @(posedge dram_clk or negedge reset_n) begin
    if (!reset_n) begin
        state <= STATE_INIT;
        dram_cke_reg <= 0;
        dram_cs_n_reg <= 1;
        dram_ldqm_reg <= 1;
        dram_udqm_reg <= 1;
    end else begin
        state <= state_nxt;
        dram_ldqm_reg <= dram_ldqm_reg_nxt;
        dram_udqm_reg <= dram_udqm_reg_nxt;
        dram_cs_n_reg <= dram_cs_n_reg_nxt;
        dram_cke_reg <= dram_cke_reg_nxt;
    end
end

endmodule
 
