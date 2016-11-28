
module SDRAM_test(
    input           reset_n,                //  Module reset.
    inout   [15:0]  dram_dq,                //  SDRAM Data bus 16 Bits
    output  [11:0]  dram_addr,              //  SDRAM Address bus 12 Bits
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
localparam STATE_WAIT = 1;  // Needs state_wait_count and state_wait_next_state.
localparam STATE_PRECHARGE_ALL = 2;
localparam STATE_PRECHARGE_WAIT = 3;
localparam STATE_REFRESH = 4; // Needs state_wait_next_state.
localparam STATE_REFRESH_WAIT = 5;
localparam STATE_INIT_REFRESH_1 = 6;
localparam STATE_INIT_REFRESH_2 = 7;
localparam STATE_LOAD_MODE = 8;
localparam STATE_LOAD_MODE_WAIT = 9;
localparam STATE_READY = 15;
reg [3:0] state;
reg [31:0] state_wait_count;
reg [3:0] state_wait_next_state;

// Put some of the outputs into registers.
localparam CMD_LOAD_MODE = 3'b000;
localparam CMD_AUTO_REFRESH = 3'b001;
localparam CMD_PRECHARGE = 3'b010;
localparam CMD_ACTIVE = 3'b011;
localparam CMD_WRITE = 3'b100;
localparam CMD_READ = 3'b101;
localparam CMD_BURST_TERMINATE = 3'b110;
localparam CMD_NOP = 3'b111;
reg [11:0] dram_addr_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_ldqm_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_udqm_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_cs_n_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_cke_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg [2:0] dram_cmd /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
assign dram_addr = dram_addr_reg;
assign dram_ldqm = dram_ldqm_reg;
assign dram_udqm = dram_udqm_reg;
assign dram_cs_n = dram_cs_n_reg;
assign dram_cke = dram_cke_reg;
assign { dram_ras_n, dram_cas_n, dram_we_n } = dram_cmd;

// Debugging.
reg [31:0] counter;
reg [15:0] seconds;
assign debug_number = { state[3:0], seconds[11:0] };

initial begin
    counter <= 0;
    seconds <= 0;

    state <= STATE_INIT;

    dram_addr_reg <= 12'b0;
    dram_ldqm_reg <= 1;
    dram_udqm_reg <= 1;
    dram_cs_n_reg <= 1;
    dram_cke_reg <= 0;
    dram_cmd <= CMD_NOP;
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
always @(posedge dram_clk or negedge reset_n) begin
    if (!reset_n) begin
        state <= STATE_INIT;
        dram_addr_reg <= 12'b0;
        dram_ldqm_reg <= 1;
        dram_udqm_reg <= 1;
        dram_cs_n_reg <= 1;
        dram_cke_reg <= 0;
        dram_cmd <= CMD_NOP;
    end else begin
        case (state)
            STATE_INIT: begin
                // Wait until the PLL has locked.
                if (dram_clk_locked) begin
                    // Tell the SDRAM to pay attention to the clock.
                    dram_cs_n_reg <= 0;
                    dram_cke_reg <= 1;

                    // The manual says to wait 100us, we wait 200us to be sure.
                    dram_cmd <= CMD_NOP;
                    state_wait_count <= 33200;  // 200us * 166 MHz.
                    state_wait_next_state <= STATE_PRECHARGE_ALL;
                    state <= STATE_WAIT;
                end
            end

            STATE_WAIT: begin
                // Wait state_wait_count clocks, then go to state_wait_next_state.
                // Never call with state_wait_count == 0.

                // We compare against 1 because just the fact that we're
                // invoked at all adds one clock.
                if (state_wait_count == 1) begin
                    state <= state_wait_next_state;
                end else begin
                    state_wait_count <= state_wait_count - 1;
                end
            end

            STATE_PRECHARGE_ALL: begin
                // Precharge all banks.
                dram_cmd <= CMD_PRECHARGE;
                dram_addr_reg <= 12'b010000000000; // All banks, ignore dram_ba_[01].
                state <= STATE_PRECHARGE_WAIT;
            end

            STATE_PRECHARGE_WAIT: begin
                // Wait after a precharge.

                // Must wait tRP = 15ns = 3 clocks. That includes the original
                // command, so only 2 clocks of NOP.
                dram_cmd <= CMD_NOP;
                state_wait_count <= 2;
                state_wait_next_state <= STATE_INIT_REFRESH_1;
                state <= STATE_WAIT;
            end

            STATE_REFRESH: begin
                // Do an auto refresh cycle. Goes to state_wait_next_state afterward.
                dram_cmd <= CMD_AUTO_REFRESH;
                state <= STATE_REFRESH_WAIT;
            end

            STATE_REFRESH_WAIT: begin
                // Wait until the refresh is done.
                dram_cmd <= CMD_NOP;
                state_wait_count <= 10;
                // state_wait_next_state is already set.
                state <= STATE_WAIT;
            end

            STATE_INIT_REFRESH_1: begin
                // First refresh in the initialization cycle.
                state_wait_next_state <= STATE_INIT_REFRESH_2;
                state <= STATE_REFRESH;
            end

            STATE_INIT_REFRESH_2: begin
                // Second refresh in the initialization cycle.
                state_wait_next_state <= STATE_LOAD_MODE;
                state <= STATE_REFRESH;
            end

            STATE_LOAD_MODE: begin
                // Load the mode register.
                dram_addr_reg <= {
                    1'b0,      // Reserved.
                    1'b0,      // Burst length applies to read and write (we don't burst).
                    2'b00,     // Standard operation.
                    3'b011,    // CAS = 3.
                    1'b0,      // Sequential.
                    3'b000     // Burst length = 1 (no bursting).
                };
                dram_cmd <= CMD_LOAD_MODE;
                state <= STATE_LOAD_MODE_WAIT;
            end

            STATE_LOAD_MODE_WAIT: begin
                // Wait after loading the mode register.
                dram_cmd <= CMD_NOP;
                state_wait_next_state <= STATE_READY;
                state_wait_count <= 3;
                state <= STATE_WAIT;
            end

            STATE_READY: begin
            end
        endcase
    end
end

endmodule
 
