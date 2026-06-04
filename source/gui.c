#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "gui.h"

void printf_center(int line, const char* text) {
    int width = 50;
    int length = strlen(text);
    int column = (width - length) / 2;

    if (column < 0) column = 0;
    printf("\x1b[%d;%dH%s", line + 1, column + 1, text);
}

void run_gui(void)
{

    printf_center(1, "\x1b[35m>_ ReSharp3DS Runtime v1.4.3\x1b[0m");
    printf_center(2, "\x1b[36mhttps://github.com/saysaa/ReSharp3DS\x1b[0m");
    printf("\x1b[5;1H--------------------------------------------------");

    const char* dir_path = "sdmc:/ReSharp3DS";
    const char* app_path = "sdmc:/ReSharp3DS/app.pe";
    const char* mscorlib_path = "sdmc:/ReSharp3DS/mscorlib.pe";

    printf("\x1b[7;1HFolder path   : %s", dir_path);
    printf("\x1b[8;1HApp path      : %s", app_path);
    printf("\x1b[9;1HMscorlib path : %s", mscorlib_path);

    int result = mkdir(dir_path, 0777);

    if (result == 0) {
        printf_center(12, "\x1b[32mReSharp3DS has been created ! Place app.pe & mscorlib.pe in the folder\x1b[0m");
    }
    else {
        if (errno == EEXIST) {
            printf_center(12, "\x1b[32mReSharp3DS folder found\x1b[0m");
            printf_center(13, "Press A to launch ReSharp3DS Runtime");
        }
        else {
            printf_center(12, "\x1b[31mError: Unable to create the folder.\x1b[0m");
            char errormsg[50];
            sprintf(errormsg, "\x1b[31mError code : %d\x1b[0m", errno);
            printf_center(13, errormsg);
        }
    }

    printf_center(27, "Press Start to exit.");
}