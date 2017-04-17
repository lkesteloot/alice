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

    touchscreen_init();

    while(!quit) {
	int x, y;
	float z;

        TouchscreenEvent e;
	while((e = touchscreen_read(&x, &y, &z)) != TOUCHSCREEN_IDLE) {
	    switch(e) {
		case TOUCHSCREEN_START:
		    printf("start %d %d %f\n", x, y, z);
		    draw_new_circle(server, x, y, z / 16.0);
		    break;
		case TOUCHSCREEN_DRAG:
		    printf("drag %d %d %f\n", x, y, z);
		    draw_new_circle(server, x, y, z / 16.0);
		    break;
		case TOUCHSCREEN_STOP:
		    printf("stop\n");
		    break;
		case TOUCHSCREEN_IDLE:
		default:
		    break;
	    }
	}

	// usleep(100000);

	rfbProcessEvents(server, 10000);
    }
}
