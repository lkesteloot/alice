#include <stdio.h>
#include <string.h>

typedef enum {NONE, JUST_BIT_6, ALL_8_BITS} Effect;

int do_effect(int previousColor, int color, Effect effect)
{
    int outputColor = color;
    switch(effect) {
        case NONE:
            break;
        case JUST_BIT_6:
            outputColor = outputColor | (previousColor & 0x40);
            break;
        case ALL_8_BITS:
            for(int b = 0; b < 8; b++)
                outputColor = outputColor | (previousColor & (1 << b));
            break;
    }
    return outputColor;
}

int main(int argc, char **argv) {
    Effect effect = JUST_BIT_6;
    while(argc > 1) {
        if(strcmp(argv[1], "--all8bits") == 0) {
            effect = ALL_8_BITS;
            argc--; argv++;
        } else if(strcmp(argv[1], "--justramp") == 0) {
            effect = NONE;
            argc--; argv++;
        }
    }
    printf("P6 800 480 255\n");
    for (int y = 0; y < 240; y++) {
        int previousColor = 0;
        for (int x = 0; x < 800; x++) {
            int color = x * 256 / 800;
            int outputColor = do_effect(previousColor, color, effect);
            printf("%c%c%c", outputColor, outputColor, outputColor);
            previousColor = color;
        }
    }
    for (int y = 0; y < 240; y++) {
        int previousColor = 255;
        for (int x = 0; x < 800; x++) {
            int color = 255 - x * 256 / 800;
            int outputColor = do_effect(previousColor, color, effect);
            printf("%c%c%c", outputColor, outputColor, outputColor);
            previousColor = color;
        }
    }

    return 0;
}
