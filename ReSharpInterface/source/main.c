#include <3ds.h>
#include <stdio.h>
#include <sys/stat.h> // Requis pour mkdir et stat
#include <errno.h>    // Requis pour analyser les erreurs (EEXIST)
 
int main(int argc, char **argv)
{
    // Initialize services
    gfxInitDefault();
 
    //Initialize console on top screen. Using NULL as the second argument tells the console library to use the internal console structure as current one
    consoleInit(GFX_TOP, NULL);
    // Resharp version Title
     printf("\x1b[0;15H");
    printf("\x1b[47;31mReSharp3DS Interface V1\x1b\n[0m");
 
    //verify if the ReSharp3ds is create or not
    const char* dir_path = "sdmc:/ReSharp3ds";
    int result = mkdir(dir_path, 0777);

    if (result == 0) {
        printf("The ReSharp3ds have been successfully created !\n");
    } 
    else {
        // if we find the folder we send a message on the log
        if (errno == EEXIST) {
            printf("We find the ReSharp3ds folder :)\n");
        } else {
        // if we can't created the folder we send the error code on the log
            printf("Error: Unable to create the folder.\n");
            printf("Error code : %d\n", errno);
        }
    }


 
    printf("\x1b[30;16HPress Start to exit.");
    // Main loop
    while (aptMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();
 
        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown();
 
        if (kDown & KEY_START) break; // break in order to return to hbmenu
 
        // Flush and swap framebuffers
        gfxFlushBuffers();
        gfxSwapBuffers();
 
        //Wait for VBlank
        gspWaitForVBlank();
    }
 
    // Exit services
    gfxExit();
 
    return 0;
}