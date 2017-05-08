#include <algorithm>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include <rfb/rfb.h>
#include <rfb/keysym.h>

#undef max
#undef min


// Increase resolution (move decimal point)
// Calibrate?
// Convert to degrees?

#define ADXL345_ADDRESS 0x53

#define ADXL345_THRESH_TAP 0x1D
#define ADXL345_OFSX 0x1E
#define ADXL345_OFSY 0x1F
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2A
#define ADXL345_ACT_TAP_STATUS 0x2B
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

const char* devname = "/dev/i2c-0";

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

uint16_t read_u16_le(int fd, int reg)
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
    return (buf[1] << 8) | buf[0];
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

void ADXL345_init(int fd)
{ 
    write_u8(fd, ADXL345_DATA_FORMAT, 0x0B); // +-16g, 13-bit
    write_u8(fd, ADXL345_POWER_CTL, 0x08); // start measurement
    //write_u8(fd, ADXL345_INT_ENABLE, 0x80); // enable DATA_READY interrupt
}

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

    // draw_new_circle(cl->screen, x, y, 1.0);

    rfbDefaultPtrAddEvent(buttonMask,x,y,cl);
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

    int addr = ADXL345_ADDRESS; /* The I2C address */

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("ioctl");
	exit(EXIT_FAILURE);
    }

    uint16_t id = read_u16(file, 0x00);

    printf("id is 0x%04X\n", id);

    ADXL345_init(file);

    int tilt_was_valid = false; 

    while(1) {
	int16_t xdata = read_u16_le(file, ADXL345_DATAX0);
	int16_t ydata = read_u16_le(file, ADXL345_DATAY0);
	int16_t zdata = read_u16_le(file, ADXL345_DATAZ0);
	float xg = xdata / 256.0;
	float yg = ydata / 256.0;
	float zg = zdata / 256.0;
	float theta_y = -atan2(yg, zg);
	float theta_x = atan2(xg, zg);
	float y_angle_center = 40.0 / 180.0 * M_PI;
	float y_angle_range = 25.0 / 180.0 * M_PI;
	float x_angle_center = 0.0 / 180.0 * M_PI;
	float x_angle_range = 25.0 / 180.0 * M_PI;
	int tilt_is_valid =
	    (theta_y > y_angle_center - y_angle_range) && 
	    (theta_y < y_angle_center + y_angle_range) && 
	    (theta_x > x_angle_center - x_angle_range) && 
	    (theta_x < x_angle_center + x_angle_range);
	if(tilt_is_valid != tilt_was_valid) {
	    if(tilt_is_valid)
		printf("TILT_VALID\n");
	    else
		printf("TILT_INVALID\n");
	    tilt_was_valid = tilt_is_valid;
	}
	if(tilt_is_valid) {
	    float tilt_x_valuator = (theta_x - x_angle_center) / x_angle_range;
	    float tilt_y_valuator = (theta_y - y_angle_center) / y_angle_range;
	    // printf("    %f, %f\n", tilt_x_valuator, tilt_y_valuator);
	    draw_new_circle(server,
		(tilt_x_valuator + 1) * .5 * FRAMEBUFFER_WIDTH,
		(tilt_y_valuator + 1) * .5 * FRAMEBUFFER_HEIGHT,
		1.0);
	}
	// usleep(100000);
	rfbProcessEvents(server, 10000);
    }
}
