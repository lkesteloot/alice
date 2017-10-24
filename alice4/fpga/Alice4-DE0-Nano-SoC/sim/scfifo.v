//START_MODULE_NAME------------------------------------------------------------
//
// Module Name     :  scfifo
//
// Description     :  Single Clock FIFO
//
// Limitation      :  
//
// Results expected:
//
//END_MODULE_NAME--------------------------------------------------------------

// BEGINNING OF MODULE
`timescale 1 ps / 1 ps

// MODULE DECLARATION
module scfifo ( data, 
                clock, 
                wrreq, 
                rdreq, 
                aclr, 
                sclr,
                q, 
                eccstatus,
                usedw, 
                full, 
                empty, 
                almost_full, 
                almost_empty);

/* verilator lint_off WIDTH */
/* verilator lint_off MULTIDRIVEN */
/* verilator lint_off SYNCASYNCNET */ // For aclr

// GLOBAL PARAMETER DECLARATION
    parameter lpm_width               = 1;
    parameter lpm_widthu              = 1;
    parameter lpm_numwords            = 2;
    parameter lpm_showahead           = "OFF";
    parameter lpm_type                = "scfifo";
    parameter lpm_hint                = "USE_EAB=ON";
    parameter intended_device_family  = "Stratix";
    parameter underflow_checking      = "ON";
    parameter overflow_checking       = "ON";
    parameter allow_rwcycle_when_full = "OFF";
    parameter use_eab                 = "ON";
    parameter add_ram_output_register = "OFF";
    parameter almost_full_value       = 0;
    parameter almost_empty_value      = 0;
    parameter maximum_depth           = 0;    
    parameter enable_ecc              = "FALSE";  

// LOCAL_PARAMETERS_BEGIN

    parameter showahead_area          = ((lpm_showahead == "ON")  && (add_ram_output_register == "OFF"));
    parameter showahead_speed         = ((lpm_showahead == "ON")  && (add_ram_output_register == "ON"));
    parameter legacy_speed            = ((lpm_showahead == "OFF") && (add_ram_output_register == "ON"));
    parameter ram_block_type = "AUTO";

// LOCAL_PARAMETERS_END

// INPUT PORT DECLARATION
    input  [lpm_width-1:0] data;
    input  clock;
    input  wrreq;
    input  rdreq;
    input  aclr;
    input  sclr;

// OUTPUT PORT DECLARATION
    output [lpm_width-1:0] q;
    output [lpm_widthu-1:0] usedw;
    output full;
    output empty;
    output almost_full;
    output almost_empty;
    output [1:0] eccstatus;

// INTERNAL REGISTERS DECLARATION
    reg [lpm_width-1:0] mem_data [(1<<lpm_widthu):0];
    reg [lpm_widthu-1:0] count_id;
    reg [lpm_widthu-1:0] read_id;
    reg [lpm_widthu-1:0] write_id;
    
    wire valid_rreq;
    reg valid_wreq;
    reg write_flag;
    reg full_flag;
    reg empty_flag;
    reg almost_full_flag;
    reg almost_empty_flag;
    reg [lpm_width-1:0] tmp_q;
    reg stratix_family;
    reg set_q_to_x;
    reg set_q_to_x_by_empty;

    reg [lpm_widthu-1:0] write_latency1; 
    reg [lpm_widthu-1:0] write_latency2; 
    reg [lpm_widthu-1:0] write_latency3; 
    integer wrt_count;
        
    reg empty_latency1; 
    reg empty_latency2; 
    
    reg [(1<<lpm_widthu)-1:0] data_ready;
    reg [(1<<lpm_widthu)-1:0] data_shown;
    
// INTERNAL TRI DECLARATION
    tri0 aclr;

// LOCAL INTEGER DECLARATION
    integer i;

// COMPONENT INSTANTIATIONS
    ALTERA_DEVICE_FAMILIES dev ();

// INITIAL CONSTRUCT BLOCK
    initial
    begin

        stratix_family = (dev.FEATURE_FAMILY_STRATIX(intended_device_family));    
        if (lpm_width <= 0)
        begin
            $display ("Error! LPM_WIDTH must be greater than 0.");
            $display ("Time: %0t  Instance: %m", $time);
        end
        if ((lpm_widthu !=1) && (lpm_numwords > (1 << lpm_widthu)))
        begin
            $display ("Error! LPM_NUMWORDS must equal to the ceiling of log2(LPM_WIDTHU).");
            $display ("Time: %0t  Instance: %m", $time);
        end
        if (dev.IS_VALID_FAMILY(intended_device_family) == 0)
        begin
            $display ("Error! Unknown INTENDED_DEVICE_FAMILY=%s.", intended_device_family);
            $display ("Time: %0t  Instance: %m", $time);
        end
        if((add_ram_output_register != "ON") && (add_ram_output_register != "OFF"))
        begin
            $display ("Error! add_ram_output_register must be ON or OFF.");          
            $display ("Time: %0t  Instance: %m", $time);
        end         
        for (i = 0; i < (1<<lpm_widthu); i = i + 1)
        begin
            if (dev.FEATURE_FAMILY_HAS_STRATIXIII_STYLE_RAM(intended_device_family))
                mem_data[i] = {lpm_width{1'b0}};
            else if (dev.FEATURE_FAMILY_STRATIX(intended_device_family))
            begin
                if ((add_ram_output_register == "ON") || (use_eab == "OFF"))
                    mem_data[i] = {lpm_width{1'b0}};
                else
                    mem_data[i] = {lpm_width{1'bx}};
            end
            else
                mem_data[i] = {lpm_width{1'b0}};
        end

        if (dev.FEATURE_FAMILY_HAS_STRATIXIII_STYLE_RAM(intended_device_family))
            tmp_q = {lpm_width{1'b0}};
        else if (dev.FEATURE_FAMILY_STRATIX(intended_device_family))
        begin
            if ((add_ram_output_register == "ON") || (use_eab == "OFF"))
                tmp_q = {lpm_width{1'b0}};
            else    
                tmp_q = {lpm_width{1'bx}};
        end
        else
            tmp_q = {lpm_width{1'b0}};
            
        write_flag = 1'b0;
        count_id = 0;
        read_id = 0;
        write_id = 0;
        full_flag = 1'b0;
        empty_flag = 1'b1;
        empty_latency1 = 1'b1; 
        empty_latency2 = 1'b1;                 
        set_q_to_x = 1'b0;
        set_q_to_x_by_empty = 1'b0;
        wrt_count = 0;        

        if (almost_full_value == 0)
            almost_full_flag = 1'b1;
        else
            almost_full_flag = 1'b0;

        if (almost_empty_value == 0)
            almost_empty_flag = 1'b0;
        else
            almost_empty_flag = 1'b1;
    end

    assign valid_rreq = (underflow_checking == "OFF")? rdreq : (rdreq && ~empty_flag);

    always @(wrreq or rdreq or full_flag)
    begin
        if (overflow_checking == "OFF")
            valid_wreq = wrreq;
        else if (allow_rwcycle_when_full == "ON")
                valid_wreq = wrreq && (!full_flag || rdreq);
        else
            valid_wreq = wrreq && !full_flag;
    end

    always @(posedge clock or posedge aclr)
    begin        
        if (aclr)
        begin
            if (add_ram_output_register == "ON")
                tmp_q <= {lpm_width{1'b0}};
            else if ((lpm_showahead == "ON") && (use_eab == "ON"))
            begin
                tmp_q <= {lpm_width{1'bX}};
            end
            else
            begin
                if (!stratix_family)
                begin
                    tmp_q <= {lpm_width{1'b0}};
                end
                else
                    tmp_q <= {lpm_width{1'bX}};
            end

            read_id <= 0;
            count_id <= 0;
            full_flag <= 1'b0;
            empty_flag <= 1'b1;
            empty_latency1 <= 1'b1; 
            empty_latency2 <= 1'b1;
            set_q_to_x <= 1'b0;
            set_q_to_x_by_empty <= 1'b0;
            wrt_count <= 0;
            
            if (almost_full_value > 0)
                almost_full_flag <= 1'b0;
            if (almost_empty_value > 0)
                almost_empty_flag <= 1'b1;

            write_id <= 0;
            
            if ((use_eab == "ON") && (stratix_family) && ((showahead_speed) || (showahead_area) || (legacy_speed)))
            begin
                write_latency1 <= 1'bx;
                write_latency2 <= 1'bx;
                data_shown <= {lpm_width{1'b0}};
                if (add_ram_output_register == "ON")
                    tmp_q <= {lpm_width{1'b0}};
                else
                    tmp_q <= {lpm_width{1'bX}};
            end            
        end
        else
        begin
            if (sclr)
            begin
                if (add_ram_output_register == "ON")
                    tmp_q <= {lpm_width{1'b0}};
                else
                    tmp_q <= {lpm_width{1'bX}};

                read_id <= 0;
                count_id <= 0;
                full_flag <= 1'b0;
                empty_flag <= 1'b1;
                empty_latency1 <= 1'b1; 
                empty_latency2 <= 1'b1;
                set_q_to_x <= 1'b0;
                set_q_to_x_by_empty <= 1'b0;
                wrt_count <= 0;

                if (almost_full_value > 0)
                    almost_full_flag <= 1'b0;
                if (almost_empty_value > 0)
                    almost_empty_flag <= 1'b1;

                if (!stratix_family)
                begin
                    if (valid_wreq)
                    begin
                        write_flag <= 1'b1;
                    end
                    else
                        write_id <= 0;
                end
                else
                begin
                    write_id <= 0;
                end

                if ((use_eab == "ON") && (stratix_family) && ((showahead_speed) || (showahead_area) || (legacy_speed)))
                begin
                    write_latency1 <= 1'bx;
                    write_latency2 <= 1'bx;
                    data_shown <= {lpm_width{1'b0}};                    
                    if (add_ram_output_register == "ON")
                        tmp_q <= {lpm_width{1'b0}};
                    else
                        tmp_q <= {lpm_width{1'bX}};
                end            
            end
            else 
            begin
                //READ operation    
                if (valid_rreq)
                begin
                    if (!(set_q_to_x || set_q_to_x_by_empty))
                    begin  
                        if (!valid_wreq)
                            wrt_count <= wrt_count - 1;

                        if (!valid_wreq)
                        begin
                            full_flag <= 1'b0;

                            if (count_id <= 0)
                                count_id <= {lpm_widthu{1'b1}};
                            else
                                count_id <= count_id - 1;
                        end                

                        if ((use_eab == "ON") && stratix_family && (showahead_speed || showahead_area || legacy_speed))
                        begin
                            if ((wrt_count == 1 && valid_rreq && !valid_wreq) || ((wrt_count == 1 ) && valid_wreq && valid_rreq))
                            begin
                                empty_flag <= 1'b1;
                            end
                            else
                            begin
                                if (showahead_speed)
                                begin
                                    if (data_shown[write_latency2] == 1'b0)
                                    begin
                                        empty_flag <= 1'b1;
                                    end
                                end
                                else if (showahead_area || legacy_speed)
                                begin
                                    if (data_shown[write_latency1] == 1'b0)
                                    begin
                                        empty_flag <= 1'b1;
                                    end
                                end
                            end
                        end
                        else
                        begin
                            if (!valid_wreq)
                            begin
                                if ((count_id == 1) && !(full_flag))
                                    empty_flag <= 1'b1;
                            end
                        end

                        if (empty_flag)
                        begin
                            if (underflow_checking == "ON")
                            begin
                                if ((use_eab == "OFF") || (!stratix_family))
                                    tmp_q <= {lpm_width{1'b0}};
                            end
                            else
                            begin
                                set_q_to_x_by_empty <= 1'b1;
                                $display ("Warning : Underflow occurred! Fifo output is unknown until the next reset is asserted.");
                                $display ("Time: %0t  Instance: %m", $time);
                            end
                        end
                        else if (read_id >= ((1<<lpm_widthu) - 1))
                        begin
                            if (lpm_showahead == "ON")
                            begin
                                if ((use_eab == "ON") && stratix_family && (showahead_speed || showahead_area))                        
                                begin
                                    if (showahead_speed)
                                    begin
                                        if ((write_latency2 == 0) || (data_ready[0] == 1'b1))
                                        begin
                                            if (data_shown[0] == 1'b1)
                                            begin
                                                tmp_q <= mem_data[0];
                                                data_shown[0] <= 1'b0;
                                                data_ready[0] <= 1'b0;
                                            end
                                        end
                                    end
                                    else
                                    begin
                                        if ((count_id == 1) && !(full_flag))
                                        begin
                                            if (underflow_checking == "ON")
                                            begin
                                                if ((use_eab == "OFF") || (!stratix_family))
                                                    tmp_q <= {lpm_width{1'b0}};
                                            end
                                            else
                                                tmp_q <= {lpm_width{1'bX}};
                                        end
                                        else if ((write_latency1 == 0) || (data_ready[0] == 1'b1))
                                        begin
                                            if (data_shown[0] == 1'b1)
                                            begin
                                                tmp_q <= mem_data[0];
                                                data_shown[0] <= 1'b0;
                                                data_ready[0] <= 1'b0;
                                            end
                                        end                            
                                    end
                                end
                                else
                                begin
                                    if ((count_id == 1) && !(full_flag))
                                    begin
                                        if (valid_wreq)
                                            tmp_q <= data;
                                        else
                                            if (underflow_checking == "ON")
                                            begin
                                                if ((use_eab == "OFF") || (!stratix_family))
                                                    tmp_q <= {lpm_width{1'b0}};
                                            end
                                            else
                                                tmp_q <= {lpm_width{1'bX}};
                                    end 
                                    else
                                        tmp_q <= mem_data[0];
                                end
                            end
                            else
                            begin
                                if ((use_eab == "ON") && stratix_family && legacy_speed)
                                begin
                                    if ((write_latency1 == read_id) || (data_ready[read_id] == 1'b1))
                                    begin
                                        if (data_shown[read_id] == 1'b1)
                                        begin
                                            tmp_q <= mem_data[read_id];
                                            data_shown[read_id] <= 1'b0;
                                            data_ready[read_id] <= 1'b0;
                                        end
                                    end
                                    else
                                    begin
                                        tmp_q <= {lpm_width{1'bX}};
                                    end                                  
                                end
                                else
                                    tmp_q <= mem_data[read_id];
                            end

                            read_id <= 0;
                        end // end if (read_id >= ((1<<lpm_widthu) - 1))
                        else
                        begin
                            if (lpm_showahead == "ON")
                            begin
                                if ((use_eab == "ON") && stratix_family && (showahead_speed || showahead_area))
                                begin
                                    if (showahead_speed)
                                    begin
                                        if ((write_latency2 == read_id+1) || (data_ready[read_id+1] == 1'b1))
                                        begin
                                            if (data_shown[read_id+1] == 1'b1)
                                            begin
                                                tmp_q <= mem_data[read_id + 1];
                                                data_shown[read_id+1] <= 1'b0;
                                                data_ready[read_id+1] <= 1'b0;
                                            end
                                        end
                                    end
                                    else
                                    begin
                                        if ((count_id == 1) && !(full_flag))
                                        begin
                                            if (underflow_checking == "ON")
                                            begin
                                                if ((use_eab == "OFF") || (!stratix_family))
                                                    tmp_q <= {lpm_width{1'b0}};
                                            end
                                            else
                                                tmp_q <= {lpm_width{1'bX}};
                                        end
                                        else if ((write_latency1 == read_id+1) || (data_ready[read_id+1] == 1'b1))
                                        begin
                                            if (data_shown[read_id+1] == 1'b1)
                                            begin
                                                tmp_q <= mem_data[read_id + 1];
                                                data_shown[read_id+1] <= 1'b0;
                                                data_ready[read_id+1] <= 1'b0;
                                            end
                                        end
                                    end
                                end
                                else
                                begin
                                    if ((count_id == 1) && !(full_flag))
                                    begin
                                        if ((use_eab == "OFF") && stratix_family)
                                        begin
                                            if (valid_wreq)
                                            begin
                                                tmp_q <= data;
                                            end
                                            else
                                            begin
                                                if (underflow_checking == "ON")
                                                begin
                                                    if ((use_eab == "OFF") || (!stratix_family))
                                                        tmp_q <= {lpm_width{1'b0}};
                                                end
                                                else
                                                    tmp_q <= {lpm_width{1'bX}};
                                            end
                                        end
                                        else
                                        begin
                                            tmp_q <= {lpm_width{1'bX}};
                                        end
                                    end
                                    else
                                        tmp_q <= mem_data[read_id + 1];
                                end
                            end
                            else
                            begin
                                if ((use_eab == "ON") && stratix_family && legacy_speed)
                                begin
                                    if ((write_latency1 == read_id) || (data_ready[read_id] == 1'b1))
                                    begin
                                        if (data_shown[read_id] == 1'b1)
                                        begin
                                            tmp_q <= mem_data[read_id];
                                            data_shown[read_id] <= 1'b0;
                                            data_ready[read_id] <= 1'b0;
                                        end
                                    end
                                    else
                                    begin
                                        tmp_q <= {lpm_width{1'bX}};
                                    end                                
                                end
                                else
                                    tmp_q <= mem_data[read_id];
                            end

                            read_id <= read_id + 1;            
                        end
                    end
                end

                // WRITE operation
                if (valid_wreq)
                begin
                    if (!(set_q_to_x || set_q_to_x_by_empty))
                    begin
                        if (full_flag && (overflow_checking == "OFF"))
                        begin
                            set_q_to_x <= 1'b1;
                            $display ("Warning : Overflow occurred! Fifo output is unknown until the next reset is asserted.");
                            $display ("Time: %0t  Instance: %m", $time);
                        end
                        else
                        begin
                            mem_data[write_id] <= data;
                            write_flag <= 1'b1;
    
                            if (!((use_eab == "ON") && stratix_family && (showahead_speed || showahead_area || legacy_speed)))
                            begin
                                empty_flag <= 1'b0;
                            end
                            else
                            begin
                                empty_latency1 <= 1'b0;
                            end
    
                            if (!valid_rreq)                
                                wrt_count <= wrt_count + 1;
    
                            if (!valid_rreq)
                            begin
                                if (count_id >= (1 << lpm_widthu) - 1)
                                    count_id <= 0;
                                else
                                    count_id <= count_id + 1;               
                            end
                            else
                            begin
                                if (allow_rwcycle_when_full == "OFF")
                                    full_flag <= 1'b0;
                            end
    
                            if (!(stratix_family) || (stratix_family && !(showahead_speed || showahead_area || legacy_speed)))
                            begin                
                                if (!valid_rreq)
                                    if ((count_id == lpm_numwords - 1) && (empty_flag == 1'b0))
                                        full_flag <= 1'b1;
                            end
                            else
                            begin   
                                if (!valid_rreq)
                                    if (count_id == lpm_numwords - 1)
                                        full_flag <= 1'b1;
                            end
    
                            if (lpm_showahead == "ON")
                            begin
                                if ((use_eab == "ON") && stratix_family && (showahead_speed || showahead_area))
                                begin
                                    write_latency1 <= write_id;                    
                                    data_shown[write_id] <= 1'b1;
                                    data_ready[write_id] <= 1'bx;
                                end
                                else
                                begin 
                                    if ((use_eab == "OFF") && stratix_family && (count_id == 0) && (!full_flag))
                                    begin
                                        tmp_q <= data;
                                    end
                                    else
                                    begin
                                        if ((!empty_flag) && (!valid_rreq))
                                        begin
                                            tmp_q <= mem_data[read_id];
                                        end
                                    end
                                end
                            end
                            else
                            begin
                                if ((use_eab == "ON") && stratix_family && legacy_speed) 
                                begin
                                    write_latency1 <= write_id;                    
                                    data_shown[write_id] <= 1'b1;
                                    data_ready[write_id] <= 1'bx;
                                end
                            end
                        end
                    end   
                end    

                if (almost_full_value == 0)
                    almost_full_flag <= 1'b1;
                else if (lpm_numwords > almost_full_value)
                begin
                    if (almost_full_flag)
                    begin
                        if ((count_id == almost_full_value) && !wrreq && rdreq)
                            almost_full_flag <= 1'b0;
                    end
                    else
                    begin
                        if ((almost_full_value == 1) && (count_id == 0) && wrreq)
                            almost_full_flag <= 1'b1;
                        else if ((almost_full_value > 1) && (count_id == almost_full_value - 1)
                                && wrreq && !rdreq)
                            almost_full_flag <= 1'b1;
                    end
                end

                if (almost_empty_value == 0)
                    almost_empty_flag <= 1'b0;
                else if (lpm_numwords > almost_empty_value)
                begin
                    if (almost_empty_flag)
                    begin
                        if ((almost_empty_value == 1) && (count_id == 0) && wrreq)
                            almost_empty_flag <= 1'b0;
                        else if ((almost_empty_value > 1) && (count_id == almost_empty_value - 1)
                                && wrreq && !rdreq)
                            almost_empty_flag <= 1'b0;
                    end
                    else
                    begin
                        if ((count_id == almost_empty_value) && !wrreq && rdreq)
                            almost_empty_flag <= 1'b1;
                    end
                end
            end

            if ((use_eab == "ON") && stratix_family)
            begin
                if (showahead_speed)
                begin
                    write_latency2 <= write_latency1;
                    write_latency3 <= write_latency2;
                    if (write_latency3 !== write_latency2)
                        data_ready[write_latency2] <= 1'b1;
                                    
                    empty_latency2 <= empty_latency1;

                    if (data_shown[write_latency2]==1'b1)
                    begin
                        if ((read_id == write_latency2) || aclr || sclr)
                        begin
                            if (!(aclr === 1'b1) && !(sclr === 1'b1))                        
                            begin
                                if (write_latency2 !== 1'bx)
                                begin
                                    tmp_q <= mem_data[write_latency2];
                                    data_shown[write_latency2] <= 1'b0;
                                    data_ready[write_latency2] <= 1'b0;
    
                                    if (!valid_rreq)
                                        empty_flag <= empty_latency2;
                                end
                            end
                        end
                    end
                end
                else if (showahead_area)
                begin
                    write_latency2 <= write_latency1;
                    if (write_latency2 !== write_latency1)
                        data_ready[write_latency1] <= 1'b1;

                    if (data_shown[write_latency1]==1'b1)
                    begin
                        if ((read_id == write_latency1) || aclr || sclr)
                        begin
                            if (!(aclr === 1'b1) && !(sclr === 1'b1))
                            begin
                                if (write_latency1 !== 1'bx)
                                begin
                                    tmp_q <= mem_data[write_latency1];
                                    data_shown[write_latency1] <= 1'b0;
                                    data_ready[write_latency1] <= 1'b0;

                                    if (!valid_rreq)
                                    begin
                                        empty_flag <= empty_latency1;
                                    end
                                end
                            end
                        end
                    end                            
                end
                else
                begin
                    if (legacy_speed)
                    begin
                        write_latency2 <= write_latency1;
                        if (write_latency2 !== write_latency1)
                            data_ready[write_latency1] <= 1'b1;

                            empty_flag <= empty_latency1;

                        if ((wrt_count == 1 && !valid_wreq && valid_rreq) || aclr || sclr)
                        begin
                            empty_flag <= 1'b1;
                            empty_latency1 <= 1'b1;
                        end
                        else
                        begin
                            if ((wrt_count == 1) && valid_wreq && valid_rreq)
                            begin
                                empty_flag <= 1'b1;
                            end
                        end
                    end
                end
            end
        end
    end

    always @(negedge clock)
    begin
        if (write_flag)
        begin
            write_flag <= 1'b0;

            if (sclr || aclr || (write_id >= ((1 << lpm_widthu) - 1)))
                write_id <= 0;
            else
                write_id <= write_id + 1;
        end

        if (!(stratix_family))
        begin
            if (!empty)
            begin
                if ((lpm_showahead == "ON") && ($time > 0))
                    tmp_q <= mem_data[read_id];
            end
        end
    end

    always @(full_flag)
    begin
        if (lpm_numwords == almost_full_value)
            if (full_flag)
                almost_full_flag = 1'b1;
            else
                almost_full_flag = 1'b0;

        if (lpm_numwords == almost_empty_value)
            if (full_flag)
                almost_empty_flag = 1'b0;
            else
                almost_empty_flag = 1'b1;
    end

// CONTINOUS ASSIGNMENT   
    assign q = (set_q_to_x || set_q_to_x_by_empty)? {lpm_width{1'bX}} : tmp_q;
    assign full = (set_q_to_x || set_q_to_x_by_empty)? 1'bX : full_flag;
    assign empty = (set_q_to_x || set_q_to_x_by_empty)? 1'bX : empty_flag;
    assign usedw = (set_q_to_x || set_q_to_x_by_empty)? {lpm_widthu{1'bX}} : count_id;
    assign almost_full = (set_q_to_x || set_q_to_x_by_empty)? 1'bX : almost_full_flag;
    assign almost_empty = (set_q_to_x || set_q_to_x_by_empty)? 1'bX : almost_empty_flag;
    assign eccstatus = {2'b0};

/* verilator lint_on WIDTH */
/* verilator lint_on MULTIDRIVEN */
/* verilator lint_on SYNCASYNCNET */ // For aclr

endmodule // scfifo
// END OF MODULE
