
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
localparam STATE_INIT = 5'h00;
localparam STATE_WAIT = 5'h01;  // Needs state_wait_count and state_wait_next_state.
localparam STATE_PRECHARGE_ALL = 5'h02;
localparam STATE_PRECHARGE_WAIT = 5'h03;
localparam STATE_REFRESH = 5'h04; // Needs state_wait_next_state.
localparam STATE_REFRESH_WAIT = 5'h05;
localparam STATE_INIT_REFRESH_1 = 5'h06;
localparam STATE_INIT_REFRESH_2 = 5'h07;
localparam STATE_LOAD_MODE = 5'h08;
localparam STATE_LOAD_MODE_WAIT = 5'h09;
localparam STATE_ACTIVATE = 5'h0a;
localparam STATE_ACTIVATE_WAIT = 5'h0b;
localparam STATE_WRITE = 5'h0c;
localparam STATE_WRITE_STOP = 5'h0d;
localparam STATE_READ_START = 5'h0e;
localparam STATE_READ_WAIT_1 = 5'h0f;
localparam STATE_READ_WAIT_2 = 5'h10;
localparam STATE_READ_WAIT_3 = 5'h11;
localparam STATE_READY = 5'h1f;
reg [4:0] state;
reg [31:0] state_wait_count;
reg [4:0] state_wait_next_state;

// Put some of the outputs into registers.
localparam CMD_LOAD_MODE = 3'b000;
localparam CMD_AUTO_REFRESH = 3'b001;
localparam CMD_PRECHARGE = 3'b010;
localparam CMD_ACTIVE = 3'b011;
localparam CMD_WRITE = 3'b100;
localparam CMD_READ = 3'b101;
localparam CMD_BURST_TERMINATE = 3'b110;
localparam CMD_NOP = 3'b111;
localparam DQM_ENABLE = 2'b00;
localparam DQM_DISABLE = 2'b11;
reg [15:0] dram_data_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON ; FAST_OUTPUT_ENABLE_REGISTER=ON"  */;
reg [11:0] dram_addr_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg [1:0] dram_dqm_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_cs_n_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg [1:0] dram_ba_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg dram_cke_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
reg [2:0] dram_cmd /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_REGISTER=ON" */;
// I don't know why this one has to be fast output enable register:
reg dram_oe_reg /* synthesis ALTERA_ATTRIBUTE = "FAST_OUTPUT_ENABLE_REGISTER=ON"  */;
assign dram_dq = dram_oe_reg ? dram_data_reg : {16{1'bz}};
assign dram_addr = dram_addr_reg;
assign { dram_udqm, dram_ldqm } = dram_dqm_reg;
assign dram_cs_n = dram_cs_n_reg;
assign { dram_ba_1, dram_ba_0 } = dram_ba_reg;
assign dram_cke = dram_cke_reg;
assign { dram_ras_n, dram_cas_n, dram_we_n } = dram_cmd;

// Debugging.
reg [31:0] counter;
reg [15:0] seconds;
// assign debug_number = { 3'b0, state[4:0], state == STATE_READY ? seconds[7:0] : { 3'b0, state_wait_next_state } };
assign debug_number = { 3'b0, state[4:0], seconds[7:0] };

// Initialize our variables.
initial begin
    counter <= 0;
    seconds <= 0;

    state <= STATE_INIT;
    state_wait_count <= 1;
    state_wait_next_state <= STATE_INIT;

    dram_addr_reg <= 12'b0;
    dram_dqm_reg <= DQM_DISABLE;
    dram_cs_n_reg <= 1;
    dram_cke_reg <= 0;
    dram_cmd <= CMD_NOP;
    dram_oe_reg <= 1'b0;
end

// Show a counter in seconds.
always @(posedge dram_clk or negedge reset_n) begin
    if (!reset_n) begin
        counter <= 0;
        // seconds <= 0;
    end else begin
        if (counter == 166000000) begin
            counter <= 0;
            // seconds <= seconds + 1;
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
        dram_dqm_reg <= DQM_DISABLE;
        dram_cs_n_reg <= 1;
        dram_cke_reg <= 0;
        dram_oe_reg <= 1'b0;
        dram_cmd <= CMD_NOP;
    end else begin
        case (state)
            STATE_INIT: begin
                // Wait until the PLL has locked.
                if (dram_clk_locked) begin
                    // Tell the SDRAM to pay attention to the clock.
                    dram_cs_n_reg <= 0;
                    dram_cke_reg <= 1;

                    // The manual says to wait 100us; we wait 200us to be sure.
                    dram_cmd <= CMD_NOP;
                    state_wait_count <= 33200;  // 200us * 166 MHz.
                    state_wait_next_state <= STATE_PRECHARGE_ALL;
                    state <= STATE_WAIT;
                end
            end

            STATE_WAIT: begin
                // Wait state_wait_count clocks, then go to state_wait_next_state.
                // Never call with state_wait_count == 0.
                seconds <= state_wait_next_state;

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
                dram_addr_reg <= 12'b010000000000; // All banks, ignore dram_ba_reg.
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
                    2'b00,     // Reserved.
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
                state_wait_next_state <= STATE_ACTIVATE;
                state_wait_count <= 3;
                state <= STATE_WAIT;
            end

            STATE_ACTIVATE: begin
                // Activate a row.
                dram_cmd <= CMD_ACTIVE;
                dram_addr_reg <= 12'b000000000000; // Row.
                dram_ba_reg <= 2'd0; // Bank.
                state <= STATE_ACTIVATE_WAIT;
            end

            STATE_ACTIVATE_WAIT: begin
                // Wait for activation to finish.
                dram_cmd <= CMD_NOP;
                state_wait_count <= 3; // tRCD
                state_wait_next_state <= STATE_WRITE;
                state <= STATE_WAIT;
            end

            STATE_WRITE: begin
                // Write one word.
                dram_cmd <= CMD_WRITE;
                dram_addr_reg <= 12'b000000000000; // Column.
                dram_ba_reg <= 2'd0; // Bank.
                dram_dqm_reg <= DQM_ENABLE;
                dram_data_reg <= 16'h1234;
                dram_oe_reg <= 1'b1;
                state <= STATE_WRITE_STOP;
            end

            STATE_WRITE_STOP: begin
                // Stop writing.
                dram_cmd <= CMD_NOP;
                dram_oe_reg <= 1'b0;
                dram_dqm_reg <= DQM_DISABLE;
                dram_data_reg <= 16'h5678;
                state <= STATE_READ_START;
            end

            STATE_READ_START: begin
                // Initiate the read of a word.
                dram_cmd <= CMD_READ;
                dram_addr_reg <= 12'b000000000000; // Column.
                dram_ba_reg <= 2'd0; // Bank.
                state <= STATE_READ_WAIT_1;
            end

            STATE_READ_WAIT_1: begin
                // First clock after read.

                // Enable output pins. This is delayed by two clocks, so we
                // must do it now.
                dram_dqm_reg <= DQM_ENABLE;
                state <= STATE_READ_WAIT_2;
            end

            STATE_READ_WAIT_2: begin
                // Second clock after read.

                // Disable output pins.
                dram_dqm_reg <= DQM_DISABLE;
                state <= STATE_READ_WAIT_3;
            end

            STATE_READ_WAIT_3: begin
                // Third clock after read. Data is ready.

                seconds <= dram_dq;
                state <= STATE_READY;
            end

            STATE_READY: begin
                dram_cmd <= CMD_NOP;
            end
        endcase
    end
end

endmodule
 
