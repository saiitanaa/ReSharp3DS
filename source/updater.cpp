#include "updater.h"

#include <3ds.h>

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#define DOWNLOAD_BUFFER_SIZE (32 * 1024)
#define DOWNLOAD_TMP_SUFFIX ".tmp"
#define MAX_REDIRECTS 5

// Update-check / update-download code is disabled for now and commented with //.
// FileExists / DirectoryExists / EnsureDirectory / DownloadFile stay enabled.

// static const char* LOCAL_MANIFEST_PATH = "sdmc:/ReSharp3DS/bin/latest.txt";

bool FileExists(const char* path)
{
    if (path == NULL || path[0] == '\0')
    {
        return false;
    }

    struct stat st;
    return stat(path, &st) == 0 && !S_ISDIR(st.st_mode);
}

bool DirectoryExists(const char* path)
{
    if (path == NULL || path[0] == '\0')
    {
        return false;
    }

    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

bool EnsureDirectory(const char* path)
{
    if (path == NULL || path[0] == '\0')
    {
        return false;
    }

    if (DirectoryExists(path))
    {
        return true;
    }

    int result = mkdir(path, 0777);

    if (result == 0)
    {
        return true;
    }

    return errno == EEXIST;
}

static bool MakeTemporaryPath(const char* outputPath, char* tmpPath, size_t tmpPathSize)
{
    if (outputPath == NULL || outputPath[0] == '\0')
    {
        return false;
    }

    if (tmpPath == NULL || tmpPathSize == 0)
    {
        return false;
    }

    int written = snprintf(
        tmpPath,
        tmpPathSize,
        "%s%s",
        outputPath,
        DOWNLOAD_TMP_SUFFIX
    );

    return written > 0 && written < (int)tmpPathSize;
}

static void DeleteFileIfExists(const char* path)
{
    if (path != NULL && path[0] != '\0')
    {
        remove(path);
    }
}

static void CloseHttp(httpcContext* context)
{
    if (context != NULL)
    {
        httpcCloseContext(context);
    }

    httpcExit();
}

static bool DrainHttpBody(httpcContext* context)
{
    static u8 buffer[DOWNLOAD_BUFFER_SIZE];

    u32 downloaded = 0;
    u32 contentSize = 0;

    Result rc = httpcGetDownloadSizeState(context, &downloaded, &contentSize);

    if (R_FAILED(rc) || contentSize == 0)
    {
        return true;
    }

    while (downloaded < contentSize)
    {
        u32 chunkDownloaded = 0;
        u32 remaining = contentSize - downloaded;
        u32 chunkSize = remaining;

        if (chunkSize > DOWNLOAD_BUFFER_SIZE)
        {
            chunkSize = DOWNLOAD_BUFFER_SIZE;
        }

        rc = httpcDownloadData(
            context,
            buffer,
            chunkSize,
            &chunkDownloaded
        );

        if (R_FAILED(rc))
        {
            return false;
        }

        if (chunkDownloaded == 0)
        {
            return false;
        }

        downloaded += chunkDownloaded;
    }

    return true;
}

static bool DownloadFileInternal(const char* url, const char* outputPath, int redirectDepth)
{
    if (url == NULL || url[0] == '\0')
    {
        printf("[HTTP] empty URL\n");
        return false;
    }

    if (outputPath == NULL || outputPath[0] == '\0')
    {
        printf("[HTTP] empty output path\n");
        return false;
    }

    if (redirectDepth > MAX_REDIRECTS)
    {
        printf("[HTTP] too many redirects\n");
        return false;
    }

    char tmpPath[512];

    if (!MakeTemporaryPath(outputPath, tmpPath, sizeof(tmpPath)))
    {
        printf("[HTTP] temporary path too long\n");
        return false;
    }

    DeleteFileIfExists(tmpPath);

    printf("[HTTP] init\n");

    Result rc = httpcInit(0);

    if (R_FAILED(rc))
    {
        printf("[HTTP] httpcInit failed: 0x%08lX\n", (unsigned long)rc);
        return false;
    }

    httpcContext context;
    memset(&context, 0, sizeof(context));

    printf("[HTTP] open: %s\n", url);

    rc = httpcOpenContext(
        &context,
        HTTPC_METHOD_GET,
        url,
        0
    );

    if (R_FAILED(rc))
    {
        printf("[HTTP] httpcOpenContext failed: 0x%08lX\n", (unsigned long)rc);
        httpcExit();
        return false;
    }

    httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
    httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
    httpcAddRequestHeaderField(&context, "User-Agent", "ReSharp3DS");
    httpcAddRequestHeaderField(&context, "Accept", "*/*");

    rc = httpcBeginRequest(&context);

    if (R_FAILED(rc))
    {
        printf("[HTTP] httpcBeginRequest failed: 0x%08lX\n", (unsigned long)rc);
        CloseHttp(&context);
        return false;
    }

    u32 statusCode = 0;
    rc = httpcGetResponseStatusCode(&context, &statusCode);

    if (R_FAILED(rc))
    {
        printf("[HTTP] status failed: 0x%08lX\n", (unsigned long)rc);
        DrainHttpBody(&context);
        CloseHttp(&context);
        return false;
    }

    printf("[HTTP] status=%lu\n", (unsigned long)statusCode);

    if (statusCode >= 300 && statusCode < 400)
    {
        char location[512];
        location[0] = '\0';

        Result headerRc = httpcGetResponseHeader(
            &context,
            "Location",
            location,
            sizeof(location)
        );

        DrainHttpBody(&context);
        CloseHttp(&context);

        if (R_FAILED(headerRc) || location[0] == '\0')
        {
            printf("[HTTP] redirect without Location\n");
            return false;
        }

        printf("[HTTP] redirect -> %s\n", location);

        return DownloadFileInternal(location, outputPath, redirectDepth + 1);
    }

    if (statusCode != 200)
    {
        printf("[HTTP] bad status code\n");
        DrainHttpBody(&context);
        CloseHttp(&context);
        return false;
    }

    u32 downloaded = 0;
    u32 contentSize = 0;

    rc = httpcGetDownloadSizeState(&context, &downloaded, &contentSize);

    if (R_FAILED(rc))
    {
        printf("[HTTP] size failed: 0x%08lX\n", (unsigned long)rc);
        CloseHttp(&context);
        return false;
    }

    if (contentSize == 0)
    {
        printf("[HTTP] content size is 0\n");
        CloseHttp(&context);
        return false;
    }

    printf("[HTTP] size=%lu bytes\n", (unsigned long)contentSize);

    FILE* file = fopen(tmpPath, "wb");

    if (!file)
    {
        printf("[HTTP] fopen failed: %s\n", tmpPath);
        DrainHttpBody(&context);
        CloseHttp(&context);
        return false;
    }

    static u8 buffer[DOWNLOAD_BUFFER_SIZE];

    downloaded = 0;

    while (downloaded < contentSize)
    {
        u32 chunkDownloaded = 0;
        u32 remaining = contentSize - downloaded;
        u32 chunkSize = remaining;

        if (chunkSize > DOWNLOAD_BUFFER_SIZE)
        {
            chunkSize = DOWNLOAD_BUFFER_SIZE;
        }

        rc = httpcDownloadData(
            &context,
            buffer,
            chunkSize,
            &chunkDownloaded
        );

        if (R_FAILED(rc))
        {
            printf("[HTTP] download failed: 0x%08lX\n", (unsigned long)rc);
            fclose(file);
            DeleteFileIfExists(tmpPath);
            CloseHttp(&context);
            return false;
        }

        if (chunkDownloaded == 0)
        {
            printf("[HTTP] download stopped unexpectedly\n");
            fclose(file);
            DeleteFileIfExists(tmpPath);
            CloseHttp(&context);
            return false;
        }

        size_t written = fwrite(buffer, 1, chunkDownloaded, file);

        if (written != chunkDownloaded)
        {
            printf("[HTTP] fwrite failed\n");
            fclose(file);
            DeleteFileIfExists(tmpPath);
            CloseHttp(&context);
            return false;
        }

        downloaded += chunkDownloaded;

        printf(
            "[HTTP] downloaded %lu/%lu\n",
            (unsigned long)downloaded,
            (unsigned long)contentSize
        );

        gspWaitForVBlank();
    }

    fclose(file);
    CloseHttp(&context);

    DeleteFileIfExists(outputPath);

    if (rename(tmpPath, outputPath) != 0)
    {
        printf("[HTTP] rename failed errno=%d\n", errno);
        DeleteFileIfExists(tmpPath);
        return false;
    }

    printf("[HTTP] downloaded OK: %s\n", outputPath);

    return FileExists(outputPath);
}

bool DownloadFile(const char* url, const char* outputPath)
{
    return DownloadFileInternal(url, outputPath, 0);
}

// static void ClearUpdateInfo(UpdateInfo* info)
// {
//     if (!info)
//     {
//         return;
//     }

//     memset(info, 0, sizeof(UpdateInfo));
//     info->hasUpdate = false;
// }

// static void TrimLineEnd(char* line)
// {
//     if (!line)
//     {
//         return;
//     }

//     int len = strlen(line);

//     while (len > 0)
//     {
//         char c = line[len - 1];

//         if (c == '\n' || c == '\r' || c == ' ' || c == '\t')
//         {
//             line[len - 1] = '\0';
//             len--;
//         }
//         else
//         {
//             break;
//         }
//     }
// }

// static void CopyValue(char* destination, size_t destinationSize, const char* value)
// {
//     if (!destination || destinationSize == 0)
//     {
//         return;
//     }

//     if (!value)
//     {
//         destination[0] = '\0';
//         return;
//     }

//     snprintf(destination, destinationSize, "%s", value);
// }

// static bool ParseUpdateManifest(const char* path, UpdateInfo* info)
// {
//     if (!path || !info)
//     {
//         return false;
//     }

//     FILE* file = fopen(path, "rb");

//     if (!file)
//     {
//         printf("[UPDATE] cannot open manifest\n");
//         return false;
//     }

//     char line[768];

//     while (fgets(line, sizeof(line), file))
//     {
//         TrimLineEnd(line);

//         if (strncmp(line, "version=", 8) == 0)
//         {
//             CopyValue(info->version, sizeof(info->version), line + 8);
//         }
//         else if (strncmp(line, "url_3dsx=", 9) == 0)
//         {
//             CopyValue(info->url3dsx, sizeof(info->url3dsx), line + 9);
//         }
//         else if (strncmp(line, "url_cia=", 8) == 0)
//         {
//             CopyValue(info->urlcia, sizeof(info->urlcia), line + 8);
//         }
//         else if (strncmp(line, "notes=", 6) == 0)
//         {
//             CopyValue(info->notes, sizeof(info->notes), line + 6);
//         }
//     }

//     fclose(file);

//     if (info->version[0] == '\0')
//     {
//         printf("[UPDATE] manifest missing version\n");
//         return false;
//     }

//     return true;
// }

// bool CheckForUpdate(
//     const char* currentVersion,
//     const char* manifestUrl,
//     UpdateInfo* outInfo
// )
// {
//     if (!currentVersion || !manifestUrl || !outInfo)
//     {
//         return false;
//     }

//     ClearUpdateInfo(outInfo);

//     EnsureDirectory("sdmc:/ReSharp3DS");
//     EnsureDirectory("sdmc:/ReSharp3DS/bin");

//     printf("[UPDATE] checking update...\n");

//     bool downloaded = DownloadFile(manifestUrl, LOCAL_MANIFEST_PATH);

//     if (!downloaded)
//     {
//         printf("[UPDATE] manifest download failed\n");

//         if (!FileExists(LOCAL_MANIFEST_PATH))
//         {
//             printf("[UPDATE] no local latest.txt fallback\n");
//             return false;
//         }

//         printf("[UPDATE] using local latest.txt fallback\n");
//     }

//     if (!ParseUpdateManifest(LOCAL_MANIFEST_PATH, outInfo))
//     {
//         printf("[UPDATE] manifest parse failed\n");
//         return false;
//     }

//     outInfo->hasUpdate = strcmp(currentVersion, outInfo->version) != 0;

//     printf(
//         "[UPDATE] current=%s latest=%s update=%d\n",
//         currentVersion,
//         outInfo->version,
//         outInfo->hasUpdate ? 1 : 0
//     );

//     return true;
// }

// bool Download3DSXUpdate(
//     const UpdateInfo* info,
//     const char* outputPath
// )
// {
//     if (!info || !outputPath)
//     {
//         return false;
//     }

//     if (!info->hasUpdate)
//     {
//         printf("[UPDATE] no 3dsx update available\n");
//         return false;
//     }

//     if (info->url3dsx[0] == '\0')
//     {
//         printf("[UPDATE] missing url_3dsx\n");
//         return false;
//     }

//     printf("[UPDATE] downloading 3dsx...\n");

//     return DownloadFile(info->url3dsx, outputPath);
// }

// bool DownloadCIAUpdate(
//     const UpdateInfo* info,
//     const char* outputPath
// )
// {
//     if (!info || !outputPath)
//     {
//         return false;
//     }

//     if (!info->hasUpdate)
//     {
//         printf("[UPDATE] no cia update available\n");
//         return false;
//     }

//     if (info->urlcia[0] == '\0')
//     {
//         printf("[UPDATE] missing url_cia\n");
//         return false;
//     }

//     EnsureDirectory("sdmc:/cias");

//     printf("[UPDATE] downloading cia...\n");

//     return DownloadFile(info->urlcia, outputPath);
// }