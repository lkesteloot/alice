
// Module for testing the behavior of the scfifo.
module Test_FIFO(
    input wire clock,
    input wire reset_n,
    output reg [31:0] debug_value0,
    output reg [31:0] debug_value1,
    output reg [31:0] debug_value2
);

    reg [31:0] counter;
    wire [31:0] step = counter - 32'd50_000_000;

    // FIFO being tested.
    reg [3:0] fifo_write_data;
    wire fifo_empty;
    wire fifo_full;
    wire [4:0] fifo_size;
    wire [3:0] fifo_read_data;
    reg fifo_read;
    reg fifo_write;
    scfifo fifo(
            .aclr(!reset_n),
            .clock(clock),
            .data(fifo_write_data),
            .empty(fifo_empty),
            .full(fifo_full),
            .usedw(fifo_size),
            .q(fifo_read_data),
            .rdreq(fifo_read),
            .wrreq(fifo_write));
    defparam fifo.add_ram_output_register = "OFF",
             fifo.intended_device_family = "CYCLONEV",
             fifo.lpm_numwords = 32,
             fifo.lpm_showahead = "OFF",
             fifo.lpm_type = "scfifo",
             fifo.lpm_width = 4,
             fifo.lpm_widthu = 5,
             fifo.overflow_checking = "ON",
             fifo.underflow_checking = "ON",
             fifo.use_eab = "ON";

    always @(posedge clock or negedge reset_n) begin
        if (!reset_n) begin
            counter <= 1'b0;
            fifo_write_data <= 1'b0;
            fifo_read <= 1'b0;
            fifo_write <= 1'b0;
            debug_value0 <= 1'b0;
            debug_value1 <= 1'b0;
            debug_value2 <= 1'b0;
        end else begin
            if (counter == 32'hFFFF_FFFF) begin
                // Nothing, end of test.
            end else begin
                if (counter < 32'd50_000_000) begin
                    // Wait one second.
                end else if (step < 8) begin
                    fifo_write <= 1'b1;
                    fifo_write_data <= step[3:0];
                    debug_value0 <= (debug_value0 << 4) | fifo_size[3:0];
                end else if (step == 8) begin
                    fifo_write <= 1'b0;
                end else if (step >= 100 && step <= 109) begin
                    fifo_read <= step < 108;
                    if (step > 101) begin
                        debug_value1 <= (debug_value1 << 4) | fifo_read_data;
                        debug_value2 <= (debug_value2 << 4) | fifo_size[3:0];
                    end
                end

                counter <= counter + 1'b1;
            end
        end
    end

endmodule
