#include <algorithm>
#include <fcntl.h>
#include <unistd.h>
#include <rfb/rfb.h>
#include <rfb/keysym.h>

#undef max
#undef min

#include "touchscreen.h"

#define FRAMEBUFFER_WIDTH 800
#define FRAMEBUFFER_HEIGHT 480
#define FRAMEBUFFER_BYTES_PER_PIXEL 4

unsigned char *fb;

static const int samples_sqrt = 4;
static const int samples = samples_sqrt * samples_sqrt;

void circle(float x, float y, float r, float w, int red, int green, int blue)
{
    float rmin = r - w / 2.0;
    float rmax = r + w / 2.0;
    float rmin_ = r - w / 2.0 - 1.415;
    float rmax_ = r + w / 2.0 + 1.415;

    int xmin = std::max(0.0f, x - r - w);
    int ymin = std::max(0.0f, y - r - w);
    int xmax = std::min(FRAMEBUFFER_WIDTH - 1.0f, x + r + w + 1);
    int ymax = std::min(FRAMEBUFFER_HEIGHT - 1.0f, y + r + w + 1);

    for(int j = ymin; j <= ymax; j++) {
        float dy = j - y;

        for(int i = xmin; i <= xmax; i++) {

            float dx = i - x;
            float d = dx * dx + dy * dy;

            // Early out if definitely inside inner radius or outside
            // outer radius
            if((d < rmin_ * rmin_) || (d > rmax_ * rmax_))
                continue;

            int hits = 0;

            for(int a = 0; a < samples_sqrt; a++) {

                float da = dx - .5 + a / (float)samples_sqrt;

                for(int b = 0; b < samples_sqrt; b++) {

                    float db = dy - .5 + b / (float)samples_sqrt;
                    float d = da * da + db * db;

                    if((d > rmin * rmin) && (d < rmax * rmax))
                        hits ++;
                }
            }

            if(hits > 0) {
                unsigned char *pixel = fb + (j * FRAMEBUFFER_WIDTH + i) * FRAMEBUFFER_BYTES_PER_PIXEL;
#if 0
                pixel[0] = (red * hits + pixel[0] * (samples - hits)) / samples;
                pixel[1] = (green * hits + pixel[1] * (samples - hits)) / samples;
                pixel[2] = (blue * hits + pixel[2] * (samples - hits)) / samples;
#else
                pixel[0] = red * hits / samples;
                pixel[1] = green * hits / samples;
                pixel[2] = blue * hits / samples;
#endif
            }
        }
    }
}

static bool quit = false;

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
	} else if(key==XK_F11) {
	    /* close down server, but wait for all clients to disconnect */
	    rfbShutdownServer(cl->screen,FALSE);
	    quit = true;
	} else {
	}
    } else {
    }
}

int mouseX = -1, mouseY = -1, buttonmask;
float oldpressure;

static const float radius = 48;
static const float width = 4;

void draw_new_circle(rfbScreenInfoPtr screen, int x, int y, float pressure)
{
    float r = radius * oldpressure;
    circle(mouseX, mouseY, r, width, 0, 0, 0);
    rfbMarkRectAsModified(screen, mouseX - r - width, mouseY - r - width, mouseX + r + width + 1, mouseY + r + width + 1);

    mouseX = x;
    mouseY = y;
    oldpressure = pressure;
    r = radius * pressure;

    circle(mouseX, mouseY, r, width, 255, 255, 255);
    rfbMarkRectAsModified(screen, mouseX - r - width, mouseY - r - width, mouseX + r + width + 1, mouseY + r + width + 1);
}


static void handlePointer(int buttonMask,int x,int y,rfbClientPtr cl)
{
   // ClientData* cd = cl->clientData;
 
#if 0
   if(x>=0 && y>=0 && x<maxx && y<maxy) {
      if(buttonMask) {
         int i,j,x1,x2,y1,y2;
         if(cd->oldButton==buttonMask) { /* draw a line */
            drawline((unsigned char*)cl->screen->frameBuffer,cl->screen->paddedWidthInBytes,bpp,
                     x,y,cd->oldx,cd->oldy);
            x1=x; y1=y;
            if(x1>cd->oldx) x1++; else cd->oldx++;
            if(y1>cd->oldy) y1++; else cd->oldy++;
            rfbMarkRectAsModified(cl->screen,x,y,cd->oldx,cd->oldy);
         } else { /* draw a point (diameter depends on button) */
            int w=cl->screen->paddedWidthInBytes;
            x1=x-buttonMask; if(x1<0) x1=0;
            x2=x+buttonMask; if(x2>maxx) x2=maxx;
            y1=y-buttonMask; if(y1<0) y1=0;
            y2=y+buttonMask; if(y2>maxy) y2=maxy;
            for(i=x1*bpp;i<x2*bpp;i++)
              for(j=y1;j<y2;j++)
                cl->screen->frameBuffer[j*w+i]=(char)0xff;
            rfbMarkRectAsModified(cl->screen,x1,y1,x2,y2);
         }
         /* we could get a selection like that:
          rfbGotXCutText(cl->screen,"Hallo",5);
          */
      } else
        cd->oldButton=0;
      cd->oldx=x; cd->oldy=y; cd->oldButton=buttonMask;
   }
#endif

    draw_new_circle(cl->screen, x, y, 1.0);

    rfbDefaultPtrAddEvent(buttonMask,x,y,cl);
}

const char* devname = "/dev/i2c-1";

uint32_t read_u32(int fd, int reg)
{
    unsigned char buf[4];
    buf[0] = reg;
    if (write(fd, buf, 1) != 1) {
        fprintf(stderr, "write register number failed\n");
	exit(EXIT_FAILURE);
    }
  
    if (read(fd, buf, 4) != 4) {
        fprintf(stderr, "read register value failed\n");
	exit(EXIT_FAILURE);
    }
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

uint16_t read_u16(int fd, int reg)
{
    unsigned char buf[2];
    buf[0] = reg;
    if (write(fd, buf, 1) != 1) {
        fprintf(stderr, "write register number failed\n");
	exit(EXIT_FAILURE);
    }
  
    if (read(fd, buf, 2) != 2) {
        fprintf(stderr, "read register value failed\n");
	exit(EXIT_FAILURE);
    }
    return (buf[0] << 8) | buf[1];
}

uint8_t read_u8(int fd, int reg)
{
    unsigned char buf[1];
    buf[0] = reg;
    if (write(fd, buf, 1) != 1) {
        fprintf(stderr, "write register number failed\n");
	exit(EXIT_FAILURE);
    }
  
    if (read(fd, buf, 1) != 1) {
        fprintf(stderr, "read register value failed\n");
	exit(EXIT_FAILURE);
    }
    return buf[0];
}

void write_u16(int fd, int reg, uint16_t value)
{
    unsigned char buf[3];
    buf[0] = reg;
    buf[1] = value >> 8;
    buf[2] = value & 0xFF;
    if (write(fd, buf, 3) != 3) {
        fprintf(stderr, "write register number and value failed\n");
	exit(EXIT_FAILURE);
    }
}

void write_u8(int fd, int reg, uint8_t value)
{
    unsigned char buf[2];
    buf[0] = reg;
    buf[1] = value;
    if (write(fd, buf, 2) != 2) {
        fprintf(stderr, "write register number and value failed\n");
	exit(EXIT_FAILURE);
    }
}

void STMPE610_init(int fd)
{ 
    write_u8(fd, STMPE610_SYS_CTRL1, STMPE610_SOFT_RESET);
    usleep(1000);
    write_u8(fd, STMPE610_SYS_CTRL1, 0);

    write_u8(fd, STMPE610_SYS_CTRL2, 0x0C);

    write_u8(fd, STMPE610_TSC_CTRL, 0x00);
    write_u8(fd, STMPE610_TSC_CTRL, 0x01);

    write_u8(fd, STMPE610_INT_EN, 0x01); // TOUCH_DETECT-Interrupt are enabled

    write_u8(fd, STMPE610_ADC_CTRL1, 0x60);
    write_u8(fd, STMPE610_ADC_CTRL2, 0x02);

    // write_u8(fd, STMPE610_GPIO_AF, 0x00);

    write_u8(fd, STMPE610_TSC_CFG, 0xA4);

    write_u8(fd, STMPE610_FIFO_TH, 0x01);
    write_u8(fd, STMPE610_FIFO_STA, 0x01); // Clear the FIFO
    write_u8(fd, STMPE610_FIFO_STA, 0x00); // FIFO back to operation mode

    write_u8(fd, STMPE610_TSC_FRACT_XYZ, 0x04);// Data Format for Z-Value
    write_u8(fd, STMPE610_TSC_I_DRIVE, 0x01); // Choose 20mA driving channel

    write_u8(fd, STMPE610_INT_STA, 0xFF); // Clear all interrupt status
    write_u8(fd, STMPE610_INT_CTRL, 0x05); // Enable the interrupts

    // define initial Touch Window
    // write_u16(fd, STMPE610_WDW_TR_X, 0x0FFF); // Top Right X
    // write_u16(fd, STMPE610_WDW_TR_Y, 0x0FFF); // Top Right Y
    // write_u16(fd, STMPE610_WDW_BL_X, 0x0000); // Bottom Left X
    // write_u16(fd, STMPE610_WDW_BL_Y, 0x0000); // Bottom Left Y

}


int main()
{
    int rfbargc = 0;
    char **rfbargv = 0;

    int bpp = 4;

    rfbScreenInfoPtr server = rfbGetScreen(&rfbargc, rfbargv, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 8, 3, bpp);
    fb = new unsigned char[FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT * bpp];

    server->frameBuffer = (char *)fb;
    server->kbdAddEvent = handleKey;
    server->ptrAddEvent = handlePointer;

    rfbInitServer(server);

    int file;
  
    file = open(devname, O_RDWR);
    if (file < 0) {
        perror("open");
	exit(EXIT_FAILURE);
    }

    int addr = STMPE610_ADDRESS; /* The I2C address */

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("ioctl");
	exit(EXIT_FAILURE);
    }

    uint16_t id = read_u16(file, 0x00);

    printf("id is 0x%04X\n", id);

    STMPE610_init(file);


    while(!quit) {
	uint8_t tsc_ctrl = read_u8(file, STMPE610_TSC_CTRL);
	uint8_t fifo_size;
	while((fifo_size = read_u8(file, STMPE610_FIFO_SIZE)) > 0) {
	    uint8_t d0 = read_u8(file, STMPE610_TSC_DATA_NOAUTOINC);
	    uint8_t d1 = read_u8(file, STMPE610_TSC_DATA_NOAUTOINC);
	    uint8_t d2 = read_u8(file, STMPE610_TSC_DATA_NOAUTOINC);
	    uint8_t d3 = read_u8(file, STMPE610_TSC_DATA_NOAUTOINC);


	    int x = 4096 - 1 - ((d0 << 4) | (d1 >> 4));
	    int y = 4096 - 1 - (((d1 & 0xF) << 8) | d2);
	    float z = 16.0 - d3 / 16.0;

	    draw_new_circle(server, x * 800 / 4096, y * 480 / 4096, z / 16.0);
	}
	write_u8(file, STMPE610_INT_STA, 0xff);
	// usleep(100000);

	rfbProcessEvents(server, 10000);
    }
}
