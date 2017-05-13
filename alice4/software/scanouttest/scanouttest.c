
#include <stdio.h>

int main() {
    printf("P6 256 256 255\n");
    for (int y = 0; y < 128; y++) {
        int color = 0;
        int previousColor = 0;
        for (int x = 0; x < 256; x++) {
            int outputColor = color | (previousColor & 0x40);
            printf("%c%c%c", outputColor, outputColor, outputColor);
            previousColor = color;
            color++;
        }
    }
    for (int y = 0; y < 128; y++) {
        int color = 255;
        int previousColor = 255;
        for (int x = 0; x < 256; x++) {
            int outputColor = color | (previousColor & 0x40);
            printf("%c%c%c", outputColor, outputColor, outputColor);
            previousColor = color;
            color--;
        }
    }

    return 0;
}
