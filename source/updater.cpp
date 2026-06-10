#include "updater.h"

#include <3ds.h>

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#define DOWNLOAD_BUFFER_SIZE (32 * 1024)
#define DOWNLOAD_TMP_SUFFIX ".tmp"
#define MAX_REDIRECTS 5

static const char* LOCAL_UPDATE_JSON_PATH = "sdmc:/ReSharp3DS/bin/version.json";

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

static void ClearUpdateInfo(UpdateInfo* info)
{
    if (!info)
    {
        return;
    }

    memset(info, 0, sizeof(UpdateInfo));
    info->hasUpdate = false;
}

static void NormalizeVersion(
    const char* version,
    char* output,
    size_t outputSize)
{
    if (output == NULL || outputSize == 0)
    {
        return;
    }

    output[0] = '\0';

    if (version == NULL)
    {
        return;
    }

    snprintf(output, outputSize, "%s", version);

    char* beta = strstr(output, "-beta.");

    if (beta != NULL)
    {
        *beta = '\0';
    }
}

static bool ParseSemverNumbers(const char* version, int* major, int* minor, int* patch)
{
    if (!version || !major || !minor || !patch)
    {
        return false;
    }

    if (version[0] == 'v' || version[0] == 'V')
    {
        version++;
    }

    *major = 0;
    *minor = 0;
    *patch = 0;

    int read = sscanf(version, "%d.%d.%d", major, minor, patch);

    return read == 3;
}

static int CompareNormalizedVersions(const char* current, const char* latest)
{
    int cMajor = 0;
    int cMinor = 0;
    int cPatch = 0;

    int lMajor = 0;
    int lMinor = 0;
    int lPatch = 0;

    bool okCurrent = ParseSemverNumbers(current, &cMajor, &cMinor, &cPatch);
    bool okLatest = ParseSemverNumbers(latest, &lMajor, &lMinor, &lPatch);

    if (!okCurrent || !okLatest)
    {
        return strcmp(current ? current : "", latest ? latest : "");
    }

    if (lMajor != cMajor)
    {
        return lMajor > cMajor ? 1 : -1;
    }

    if (lMinor != cMinor)
    {
        return lMinor > cMinor ? 1 : -1;
    }

    if (lPatch != cPatch)
    {
        return lPatch > cPatch ? 1 : -1;
    }

    return 0;
}

static bool ReadWholeFile(const char* path, char* output, size_t outputSize)
{
    if (!path || !output || outputSize == 0)
    {
        return false;
    }

    output[0] = '\0';

    FILE* file = fopen(path, "rb");

    if (!file)
    {
        return false;
    }

    size_t read = fread(output, 1, outputSize - 1, file);
    fclose(file);

    output[read] = '\0';

    return read > 0;
}

static bool ExtractJsonString(
    const char* json,
    const char* key,
    char* output,
    size_t outputSize)
{
    if (!json || !key || !output || outputSize == 0)
    {
        return false;
    }

    output[0] = '\0';

    char pattern[80];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);

    const char* p = strstr(json, pattern);

    if (p == NULL)
    {
        return false;
    }

    p += strlen(pattern);

    while (*p != '\0' && *p != ':')
    {
        p++;
    }

    if (*p != ':')
    {
        return false;
    }

    p++;

    while (*p != '\0' && isspace((unsigned char)*p))
    {
        p++;
    }

    if (*p != '"')
    {
        return false;
    }

    p++;

    size_t index = 0;

    while (*p != '\0' && *p != '"' && index + 1 < outputSize)
    {
        output[index++] = *p++;
    }

    output[index] = '\0';

    return index > 0;
}

bool CheckForUpdate(
    const char* currentVersion,
    const char* apiUrl,
    UpdateInfo* outInfo)
{
    if (!currentVersion || !apiUrl || !outInfo)
    {
        return false;
    }

    ClearUpdateInfo(outInfo);

    EnsureDirectory("sdmc:/ReSharp3DS");
    EnsureDirectory("sdmc:/ReSharp3DS/bin");

    printf("[UPDATE] checking version JSON API...\n");

    bool downloaded = DownloadFile(apiUrl, LOCAL_UPDATE_JSON_PATH);

    if (!downloaded)
    {
        printf("[UPDATE] API download failed\n");

        if (!FileExists(LOCAL_UPDATE_JSON_PATH))
        {
            printf("[UPDATE] no local version.json fallback\n");
            return false;
        }

        printf("[UPDATE] using local version.json fallback\n");
    }

    char json[2048];

    if (!ReadWholeFile(LOCAL_UPDATE_JSON_PATH, json, sizeof(json)))
    {
        printf("[UPDATE] cannot read version.json\n");
        return false;
    }

    if (!ExtractJsonString(json, "version", outInfo->version, sizeof(outInfo->version)))
    {
        printf("[UPDATE] version.json missing version\n");
        return false;
    }

    ExtractJsonString(json, "url_3dsx", outInfo->url3dsx, sizeof(outInfo->url3dsx));
    ExtractJsonString(json, "3dsx", outInfo->url3dsx, sizeof(outInfo->url3dsx));

    ExtractJsonString(json, "url_cia", outInfo->urlcia, sizeof(outInfo->urlcia));
    ExtractJsonString(json, "cia", outInfo->urlcia, sizeof(outInfo->urlcia));

    ExtractJsonString(json, "url_elf", outInfo->urlelf, sizeof(outInfo->urlelf));
    ExtractJsonString(json, "elf", outInfo->urlelf, sizeof(outInfo->urlelf));

    ExtractJsonString(json, "notes", outInfo->notes, sizeof(outInfo->notes));

    char currentNormalized[64];
    char latestNormalized[64];

    NormalizeVersion(currentVersion, currentNormalized, sizeof(currentNormalized));
    NormalizeVersion(outInfo->version, latestNormalized, sizeof(latestNormalized));

    snprintf(outInfo->normalizedVersion, sizeof(outInfo->normalizedVersion), "%s", latestNormalized);

    int compare = CompareNormalizedVersions(currentNormalized, latestNormalized);

    outInfo->hasUpdate = compare > 0;

    printf(
        "[UPDATE] current=%s normalized=%s latest=%s normalized=%s update=%d\n",
        currentVersion,
        currentNormalized,
        outInfo->version,
        latestNormalized,
        outInfo->hasUpdate ? 1 : 0
    );

    return true;
}
