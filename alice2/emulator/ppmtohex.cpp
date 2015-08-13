#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <algorithm>


static void skipComments(FILE *fp, char ***comments, size_t *commentCount)
{
    int c;
    char line[512];

    while((c = fgetc(fp)) == '#') {
        fgets(line, sizeof(line) - 1, fp);
	line[strlen(line) - 1] = '\0';
	if(comments != NULL) {
	    *comments =
	        (char **)realloc(*comments, sizeof(char *) * (*commentCount + 1));
	    (*comments)[*commentCount] = strdup(line);
	}
	if(commentCount != NULL) {
	    (*commentCount) ++;
	}
    }
    ungetc(c, fp);
}


int pnmRead(FILE *file, unsigned int *w, unsigned int *h, float **pixels,
    char ***comments, size_t *commentCount)
{
    unsigned char	dummyByte;
    int			i;
    float		max;
    char		token;
    int			width, height;
    float		*rgbPixels;

    if(commentCount != NULL)
	*commentCount = 0;
    if(comments != NULL)
	*comments = NULL;

    fscanf(file, " ");

    skipComments(file, comments, commentCount);

    if(fscanf(file, "P%c ", &token) != 1) {
         fprintf(stderr, "pnmRead: Had trouble reading PNM tag\n");
	 return(0);
    }

    skipComments(file, comments, commentCount);

    if(fscanf(file, "%d ", &width) != 1) {
         fprintf(stderr, "pnmRead: Had trouble reading PNM width\n");
	 return(0);
    }

    skipComments(file, comments, commentCount);

    if(fscanf(file, "%d ", &height) != 1) {
         fprintf(stderr, "pnmRead: Had trouble reading PNM height\n");
	 return(0);
    }

    skipComments(file, comments, commentCount);

    if(token != '1' && token != '4')
        if(fscanf(file, "%f", &max) != 1) {
             fprintf(stderr, "pnmRead: Had trouble reading PNM max value\n");
	     return(0);
        }

    rgbPixels = (float *)malloc(width * height * 4 * sizeof(float));
    if(rgbPixels == NULL) {
         fprintf(stderr, "pnmRead: Couldn't allocate %zd bytes\n",
	     width * height * 4 * sizeof(float));
         fprintf(stderr, "pnmRead: (For a %d by %d image)\n", width,
	     height);
	 return(0);
    }

    if(token != '4')
	skipComments(file, comments, commentCount);

    if(token != '4')
    fread(&dummyByte, 1, 1, file);	/* chuck white space */

    if(token == '1') {
	for(i = 0; i < width * height; i++) {
	    int pixel;
	    fscanf(file, "%d", &pixel);
	    pixel = 1 - pixel;
	    rgbPixels[i * 4 + 0] = pixel;
	    rgbPixels[i * 4 + 1] = pixel;
	    rgbPixels[i * 4 + 2] = pixel;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    } else if(token == '2') {
	for(i = 0; i < width * height; i++) {
	    int pixel;
	    fscanf(file, "%d", &pixel);
	    rgbPixels[i * 4 + 0] = pixel / max;
	    rgbPixels[i * 4 + 1] = pixel / max;
	    rgbPixels[i * 4 + 2] = pixel / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    } else if(token == '3') {
	for(i = 0; i < width * height; i++) {
	    int r, g, b;
	    fscanf(file, "%d %d %d", &r, &g, &b);
	    rgbPixels[i * 4 + 0] = r / max;
	    rgbPixels[i * 4 + 1] = g / max;
	    rgbPixels[i * 4 + 2] = b / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    } else if(token == '4') {
        int bitnum = 0;

	for(i = 0; i < width * height; i++) {
	    unsigned char pixel;
	    unsigned char value;

	    if(bitnum == 0)
	        fread(&value, 1, 1, file);

	    pixel = (1 - ((value >> (7 - bitnum)) & 1));
	    rgbPixels[i * 4 + 0] = pixel;
	    rgbPixels[i * 4 + 1] = pixel;
	    rgbPixels[i * 4 + 2] = pixel;
	    rgbPixels[i * 4 + 3] = 1.0;

	    if(++bitnum == 8 || ((i + 1) % width) == 0)
	        bitnum = 0;
	}
    } else if(token == '5') {
	for(i = 0; i < width * height; i++) {
	    unsigned char pixel;
	    fread(&pixel, 1, 1, file);
	    rgbPixels[i * 4 + 0] = pixel / max;
	    rgbPixels[i * 4 + 1] = pixel / max;
	    rgbPixels[i * 4 + 2] = pixel / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    } else if(token == '6') {
	for(i = 0; i < width * height; i++) {
	    unsigned char rgb[3];
	    fread(rgb, 3, 1, file);
	    rgbPixels[i * 4 + 0] = rgb[0] / max;
	    rgbPixels[i * 4 + 1] = rgb[1] / max;
	    rgbPixels[i * 4 + 2] = rgb[2] / max;
	    rgbPixels[i * 4 + 3] = 1.0;
	}
    }
    *w = width;
    *h = height;
    *pixels = rgbPixels;
    return(1);
}


void pnmWrite(FILE *fp, int w, int h, float *pixels)
{
    int i;

    fprintf(fp, "P6 %d %d 255\n", w, h);
    for(i = 0; i < w * h; i++)
    {
        fputc(pixels[i * 4 + 0] * 255, fp);
        fputc(pixels[i * 4 + 1] * 255, fp);
        fputc(pixels[i * 4 + 2] * 255, fp);
    }
}

const int video_columns = 176;
const int video_rows = 262;
const int video_row_bytes = video_columns / 8;
const int video_byte_count = video_rows * video_row_bytes;
typedef unsigned char video_buffer[video_byte_count];

void video_set_pixel(video_buffer buffer, int x, int y, bool c)
{
    if(x < 0 || x >= video_columns)
        return;
    if(y < 0 || y >= video_rows)
        return;
    int byte = x / 8;
    int bit = 7 - x % 8;
    if(c)
        buffer[y * video_row_bytes + byte] |= (1 << bit);
    else
        buffer[y * video_row_bytes + byte] &= ~(1 << bit);
}

bool pnm_get_pixel(int width, int height, float *pixels, int x, int y, float& r, float& g, float& b)
{
    if(x < 0 || x >= width)
        return false;
    if(y < 0 || y >= height)
        return false;
    float *p = pixels + (x + y * width) * 4;
    r = p[0];
    g = p[1];
    b = p[2];
    return true;
}

int main(int argc, char **argv)
{
    if(argc != 1) {
        fprintf(stderr, "%s < image.pnm > output.hex\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    float *pixels;
    unsigned int width, height;
    int success = pnmRead(stdin, &width, &height, &pixels, NULL, NULL);
    if(!success) {
        fprintf(stderr, "couldn't open %s for reading\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int pnm_offset_x = width / 2 - video_columns / 2;
    int pnm_offset_y = height / 2 - video_rows / 2;

    video_buffer video;
    for(int j = 0; j < video_rows; j++)
        for(int i = 0; i < video_columns; i++) {
            float r, g, b;
            bool success = pnm_get_pixel(width, height, pixels, i + pnm_offset_x, j + pnm_offset_y, r, g, b);
            if(!success) {
                r = g = b = 0.0;
            }
            video_set_pixel(video, i, j, sqrtf(r * r + g * g + b * b) > .5);
        }
        
    int addr = 0x4000;
    for(int i = 0; i < video_byte_count;) {
        int length = std::min(32, video_byte_count - i);
        printf(":%02X%04X00", length, addr + i);
        for(int j = 0; j < length; j++)
            printf("%02X", video[i + j]);
        printf("%02X\n", 0);
        i += length;
    }
    printf(":00000001FF");
}

/* vi:tabstop=8
 */

