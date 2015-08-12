#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <rfb/rfb.h>
#include <rfb/keysym.h>


#include "z80emu.h"
#include "simulator.h"
#include "8x16.h"

const bool debug = false;

bool Z80_INTERRUPT_FETCH = false;
unsigned short Z80_INTERRUPT_FETCH_DATA;

std::vector<board_base*> boards;

// 14 offset from left, 9 offset from right
const int video_board_width = 176;
const int video_board_height = 262;
const int video_board_hsync_columns = 14;
const int video_board_vsync_rows = 9;

const int rfb_pixel_scale_x = 4;
const int rfb_pixel_scale_y = 2;

const int font_scale = 2;
const int lcd_rows = 2;
const int lcd_columns = 16;
const int lcd_chars_height = lcd_rows * (fontheight + 1) * font_scale;

const int rfb_width = (video_board_width - video_board_hsync_columns) * rfb_pixel_scale_x;
const int rfb_height = (video_board_height - video_board_vsync_rows) * rfb_pixel_scale_y + lcd_chars_height;

void set_rfb_pixel(char *rfb_bytes, int rfb_width, int rfb_height, int x, int y, int r, int g, int b)
{
    if(x < 0 || x >= rfb_width || y < 0 || y >= rfb_height)
        return;
    unsigned char *p = (unsigned char *)rfb_bytes + (y * rfb_width + x) * 4;
    p[0] = r;
    p[1] = g;
    p[2] = b;
    p[3] = 255;
}

void draw_glyph(rfbScreenInfoPtr server, int rfb_width, int rfb_height,
    int x, int y, char c)
{
    int    gx, gy;
    unsigned char    *glyph;

    glyph = fontbits + fontheight * c;
    for(gx = 0; gx < fontwidth + 1; gx++)
        for(gy = 0; gy < fontheight + 1; gy++) {
            int v;
            if(gx < fontwidth && gy < fontheight && (glyph[gy + gx / 8] & (1 << (7 - (gx % 8)))))
                v = 32;
            else
                v = 224;
            for(int j = 0; j < font_scale; j++)
                for(int i = 0; i < font_scale; i++) {
                int col = x + i + gx * font_scale;
                int row = y + j + gy * font_scale;
                set_rfb_pixel(server->frameBuffer, rfb_width, rfb_height, col, row, v, v, v);
            }
        }
        rfbMarkRectAsModified(server, x, y, x + fontwidth * 2 + 1, y + fontheight * 2 + 1);

}

void set_video_pixel_in_rfb(char *rfb_bytes, int rfb_width, int rfb_height, int video_x, int video_y, bool set)
{
    int rfb_x = video_x * rfb_pixel_scale_x;
    int rfb_y = video_y * rfb_pixel_scale_y;
    int v = set ? 255 : 0;

    for(int j = 0; j < rfb_pixel_scale_y; j++)
        for(int i = 0; i < rfb_pixel_scale_x; i++) {
            set_rfb_pixel(rfb_bytes, rfb_width, rfb_height, rfb_x + i, rfb_y + j, v, v, v);
        }
}

bool quit = false;

struct IOboard : board_base
{
    const int PIC_port = 0x04;
    const int CMD_SER = 0x01;
    const int CMD_KBD = 0x02;
    const int CMD_TIM = 0x03;
    std::vector<unsigned char> queue;

    enum { NONE, PENDING, SIGNALED } interrupt_status;

    IOboard() :
        interrupt_status(NONE)
    { }

    virtual bool io_read(int addr, unsigned char &data)
    {
        if(addr == PIC_port) {
            bool handled = false;
            printf("request to read from IO\n");
            if(queue.size() > 0) {
                data = queue[0];
                printf("read 0x%02X from queue\n", data);
                queue.erase(queue.begin());
                handled = true;
            }
            if(queue.size() > 0)
                interrupt_status = PENDING;
            else {
                printf("cleared interrupt\n");
                interrupt_status = NONE;
            }
            return handled;
        }
        return false;
    }

    virtual bool board_get_interrupt(int& irq)
    {
        if(interrupt_status == PENDING) {
            printf("signal irq 0\n");
            irq = 0;
            interrupt_status = SIGNALED;
            return true;
        }
        return false;
    }

    void enqueue_AT_keycode(unsigned char key)
    {
        queue.push_back(CMD_KBD);
        queue.push_back(key);
        if(interrupt_status == NONE)
            interrupt_status = PENDING;
    }
};

IOboard *IO_board_instance;

static void handleKey(rfbBool down, rfbKeySym key, rfbClientPtr cl)
{
    if(down) {
        if(key==XK_Escape) {
            rfbCloseClient(cl);
            quit = true;
        } else if(key==XK_F12) {
            /* close down server, disconnecting clients */
            rfbShutdownServer(cl->screen,TRUE);
            quit = true;
        } else if(key==XK_F11) {
            /* close down server, but wait for all clients to disconnect */
            rfbShutdownServer(cl->screen,FALSE);
            quit = true;
        } else if(key>=' ' && key<0x100) {
            printf("key %02x : %c\n", key, key);
            if(key == 'a') {
                IO_board_instance->enqueue_AT_keycode(28);
            }
        }
    } else {
        if(key == 'a') {
            IO_board_instance->enqueue_AT_keycode(0xf0);
            IO_board_instance->enqueue_AT_keycode(28);
        }
    }
}


struct ROMboard : board_base
{
    unsigned char rom_bytes[16384];
    int base_addr;
    ROMboard(unsigned char b[16384]) 
    {
        memcpy(rom_bytes, b, sizeof(rom_bytes));
        base_addr = 0x0000;
    }
    virtual bool memory_read(int addr, unsigned char &data)
    {
        if(addr >= base_addr && addr < base_addr + sizeof(rom_bytes)) {
            data = rom_bytes[addr - base_addr];
            if(debug) printf("read 0x%04X -> 0x%02X from ROM\n", addr, data);
            return true;
        }
        return false;
    }
};

struct RAMboard : board_base
{
    unsigned char ram_bytes[32768];
    int base_addr;
    RAMboard() 
    {
        memset(ram_bytes, 0, sizeof(ram_bytes));
        base_addr = 0x8000;
    }
    virtual bool memory_read(int addr, unsigned char &data)
    {
        if(addr >= base_addr && addr < base_addr + sizeof(ram_bytes)) {
            data = ram_bytes[addr - base_addr];
            if(debug) printf("read 0x%04X -> 0x%02X from RAM\n", addr, data);
            return true;
        }
        return false;
    }
    virtual bool memory_write(int addr, unsigned char data)
    {
        if(addr >= base_addr && addr < base_addr + sizeof(ram_bytes)) {
            ram_bytes[addr - base_addr] = data;
            if(debug) printf("wrote 0x%02X to RAM 0x%04X\n", data, addr);
            return true;
        }
        return false;
    }
};

const int LCLEAR = 0x01; // LCD clear instruction
const int LRESET = 0x38; // LCD reset to normal instruction
const int LHALF1 = 0x80; // LCD move cursor to char 1
const int LHALF2 = 0xc0; // LCD move cursor to char 9

struct LCDboard : board_base
{
    const int LCDINST = 0x2; //  LCD instruction I/O port
    const int LCDDATA = 0x3; // LCD data I/O port

    int lcd[lcd_rows * lcd_columns];
    int cursor;
    rfbScreenInfoPtr rfb_server;

    LCDboard(rfbScreenInfoPtr server_) :
        rfb_server(server_)
    {
        cursor = 0;
        memset(lcd, ' ', sizeof(lcd));
        for(int i = 0; i < lcd_rows * lcd_columns; i++)
            draw_lcd_character(i);
    }

    virtual bool io_write(int addr, unsigned char data)
    {
        if(addr == LCDINST) {
            if(debug) printf("LCD instruction byte: 0x%02X\n", data);
            switch(data) {
                case LCLEAR:
                    // cursor = 0;
                    // memset(lcd, ' ', sizeof(lcd));
                    // for(int i = 0; i < lcd_rows * lcd_columns; i++)
                        // draw_lcd_character(i);
                    // break;
                case LRESET:
                    cursor = 0;
                    break;
                case LHALF1:
                    cursor = 0;
                    break;
                case LHALF2:
                    cursor = lcd_rows;
                    break;
            }
            return true;
        }
        if(addr == LCDDATA) {
            if(debug)
                printf("LCD data byte: 0x%02X (%c)\n", data, data);
            else {
                // printf("%c", data);
                lcd[cursor] = data;
                if(cursor < lcd_columns * lcd_rows)
                    draw_lcd_character(cursor);
                cursor ++;
                fflush(stdout);
            }
            return true;
        }
        return false;
    }

    void draw_lcd_character(int which)
    {
        int y = which / lcd_columns;
        int x = which - y * lcd_columns;
        int c = lcd[which];
        draw_glyph(rfb_server, rfb_width, rfb_height,
            (rfb_width - lcd_columns * (fontwidth + 1) * font_scale) / 2 + (fontwidth + 1) * font_scale * x,
            rfb_height - 1 - (fontheight + 1) * font_scale * (lcd_rows - y),
            c);
    }
};

int main(int argc, char **argv)
{
    if(argc < 2) {
        fprintf(stderr, "%s rom.bin\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *fp = fopen(argv[1], "rb");
    if(fp == NULL) {
        fprintf(stderr, "failed to open %s for reading\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    unsigned char b[16384];
    size_t length = fread(b, 1, sizeof(b), fp);
    if(length < 16384) {
        fprintf(stderr, "ROM read from %s was unexpectedly short (%zd bytes)\n", argv[1], length);
        exit(EXIT_FAILURE);
    }
    fclose(fp);

    rfbScreenInfoPtr server = rfbGetScreen(&argc,argv,rfb_width,rfb_height,8,3,4);
    unsigned char* rfb_bytes = new unsigned char[rfb_width*rfb_height*4];
    server->frameBuffer = (char *)rfb_bytes;
    server->kbdAddEvent = handleKey;

    boards.push_back(new ROMboard(b));
    boards.push_back(new RAMboard());
    boards.push_back(new LCDboard(server));
    boards.push_back(IO_board_instance = new IOboard());

    Z80_STATE state;

    Z80Reset(&state);

    rfbInitServer(server);
    rfbProcessEvents(server, 1000);

    int prevcycles = 0, cycles;
    while(!quit && (cycles = Z80Emulate(&state, 10000)) > 0)
    {
        if(prevcycles / 10000 != (prevcycles + cycles) / 10000) {
            if(debug)printf("emulated %d cycles\n", cycles);
            prevcycles += cycles;
        }
        rfbProcessEvents(server, 1000);

        for(auto it = boards.begin(); it != boards.end(); it++) {
            int irq;
            if((*it)->board_get_interrupt(irq)) {
                printf("send interrupt %d\n", irq);
                // Pretend to be 8259 configured for Alice:
                Z80_INTERRUPT_FETCH = true;
                Z80_INTERRUPT_FETCH_DATA = 0x3f00 + irq * 4;
                Z80Interrupt(&state, 0xCD);
                break;
            }
        }
    }

#if 0
    unsigned char buffer[128 * 1024];
    size_t length;

    length = fread(buffer, 1, 128 * 1024, timing_rom);

    for(int j = video_board_vsync_rows; j < video_board_height; j++)
        for(int i = video_board_hsync_columns; i < video_board_width; i++) {
            unsigned char *p = buffer + j * video_board_width + i;
            bool v = (p[0] & 0x40);
            set_video_pixel_in_rfb(rfb_bytes, rfb_width, rfb_height, i - video_board_hsync_columns, j - video_board_vsync_rows, v);
        }
#endif

    return(0);
}

