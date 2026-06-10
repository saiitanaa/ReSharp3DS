#ifndef UPDATER_H
#define UPDATER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct UpdateInfo
{
    char version[64];
    char normalizedVersion[64];
    char url3dsx[512];
    char urlcia[512];
    char urlelf[512];
    char notes[256];
    bool hasUpdate;
};

bool FileExists(const char* path);
bool DirectoryExists(const char* path);
bool EnsureDirectory(const char* path);

bool DownloadFile(const char* url, const char* outputPath);

bool CheckForUpdate(
    const char* currentVersion,
    const char* apiUrl,
    UpdateInfo* outInfo
);

#ifdef __cplusplus
}
#endif

#endif
