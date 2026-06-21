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
#define MAX_REDIRECTS 8
#define MAX_URL_SIZE 1024

// ------------------------------------------------------------
// Filesystem
// ------------------------------------------------------------

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

// ------------------------------------------------------------
// URL helpers
// ------------------------------------------------------------

static bool StartsWith(const char* text, const char* prefix)
{
    if (!text || !prefix)
    {
        return false;
    }

    return strncmp(text, prefix, strlen(prefix)) == 0;
}

static bool ResolveRedirectUrl(
    const char* currentUrl,
    const char* location,
    char* output,
    size_t outputSize)
{
    if (!currentUrl || !location || !output || outputSize == 0)
    {
        return false;
    }

    output[0] = '\0';

    if (StartsWith(location, "http://") || StartsWith(location, "https://"))
    {
        int written = snprintf(output, outputSize, "%s", location);
        return written > 0 && written < (int)outputSize;
    }

    if (location[0] == '/')
    {
        const char* schemeEnd = strstr(currentUrl, "://");

        if (!schemeEnd)
        {
            return false;
        }

        const char* hostStart = schemeEnd + 3;
        const char* hostEnd = strchr(hostStart, '/');

        if (!hostEnd)
        {
            int written = snprintf(output, outputSize, "%s%s", currentUrl, location);
            return written > 0 && written < (int)outputSize;
        }

        int baseLen = (int)(hostEnd - currentUrl);

        int written = snprintf(
            output,
            outputSize,
            "%.*s%s",
            baseLen,
            currentUrl,
            location
        );

        return written > 0 && written < (int)outputSize;
    }

    char base[MAX_URL_SIZE];
    snprintf(base, sizeof(base), "%s", currentUrl);

    char* slash = strrchr(base, '/');

    if (!slash)
    {
        return false;
    }

    slash[1] = '\0';

    int written = snprintf(output, outputSize, "%s%s", base, location);
    return written > 0 && written < (int)outputSize;
}

static bool ExtractReleaseTagFromUrl(
    const char* url,
    char* output,
    size_t outputSize)
{
    if (!url || !output || outputSize == 0)
    {
        return false;
    }

    output[0] = '\0';

    const char* marker = "/releases/tag/";
    const char* p = strstr(url, marker);

    if (!p)
    {
        return false;
    }

    p += strlen(marker);

    size_t index = 0;

    while (*p != '\0' &&
           *p != '/' &&
           *p != '?' &&
           *p != '#' &&
           index + 1 < outputSize)
    {
        output[index++] = *p++;
    }

    output[index] = '\0';

    return index > 0;
}

static bool BuildLatestDownloadUrl(
    const char* latestReleaseUrl,
    const char* assetName,
    char* output,
    size_t outputSize)
{
    if (!latestReleaseUrl || !assetName || !output || outputSize == 0)
    {
        return false;
    }

    output[0] = '\0';

    const char* releases = strstr(latestReleaseUrl, "/releases/");

    if (!releases)
    {
        return false;
    }

    int repoUrlLen = (int)(releases - latestReleaseUrl);

    int written = snprintf(
        output,
        outputSize,
        "%.*s/releases/latest/download/%s",
        repoUrlLen,
        latestReleaseUrl,
        assetName
    );

    return written > 0 && written < (int)outputSize;
}

// ------------------------------------------------------------
// HTTP
// ------------------------------------------------------------

static void CloseHttp(httpcContext* context)
{
    if (context != NULL)
    {
        httpcCloseContext(context);
    }

    httpcExit();
}

static bool OpenHttpGet(
    const char* url,
    httpcContext* context,
    u32* statusCode)
{
    if (!url || !context || !statusCode)
    {
        return false;
    }

    *statusCode = 0;
    memset(context, 0, sizeof(httpcContext));

    Result rc = httpcInit(0);

    if (R_FAILED(rc))
    {
        printf("[HTTP] httpcInit failed: 0x%08lX\n", (unsigned long)rc);
        return false;
    }

    printf("[HTTP] open: %s\n", url);

    rc = httpcOpenContext(
        context,
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

    // GitHub est en HTTPS.
    // Sur 3DS, ça évite que la vérification certificat casse le téléchargement.
    httpcSetSSLOpt(context, SSLCOPT_DisableVerify);

    httpcSetKeepAlive(context, HTTPC_KEEPALIVE_DISABLED);
    httpcAddRequestHeaderField(context, "User-Agent", "ReSharp3DS");
    httpcAddRequestHeaderField(context, "Accept", "*/*");
    httpcAddRequestHeaderField(context, "Connection", "close");

    rc = httpcBeginRequest(context);

    if (R_FAILED(rc))
    {
        printf("[HTTP] httpcBeginRequest failed: 0x%08lX\n", (unsigned long)rc);
        CloseHttp(context);
        return false;
    }

    rc = httpcGetResponseStatusCode(context, statusCode);

    if (R_FAILED(rc))
    {
        printf("[HTTP] status failed: 0x%08lX\n", (unsigned long)rc);
        CloseHttp(context);
        return false;
    }

    printf("[HTTP] status=%lu\n", (unsigned long)*statusCode);

    return true;
}

static bool DrainHttpBody(httpcContext* context)
{
    if (!context)
    {
        return false;
    }

    static u8 buffer[DOWNLOAD_BUFFER_SIZE];

    while (true)
    {
        u32 downloaded = 0;

        Result rc = httpcDownloadData(
            context,
            buffer,
            DOWNLOAD_BUFFER_SIZE,
            &downloaded
        );

        if (rc == 0)
        {
            return true;
        }

        if (rc != (Result)HTTPC_RESULTCODE_DOWNLOADPENDING)
        {
            return false;
        }

        gspWaitForVBlank();
    }
}

static bool GetLatestReleaseTag(
    const char* latestReleaseUrl,
    char* tagOutput,
    size_t tagOutputSize)
{
    if (!latestReleaseUrl || !tagOutput || tagOutputSize == 0)
    {
        return false;
    }

    tagOutput[0] = '\0';

    char currentUrl[MAX_URL_SIZE];
    snprintf(currentUrl, sizeof(currentUrl), "%s", latestReleaseUrl);

    for (int redirect = 0; redirect <= MAX_REDIRECTS; redirect++)
    {
        httpcContext context;
        u32 statusCode = 0;

        if (!OpenHttpGet(currentUrl, &context, &statusCode))
        {
            return false;
        }

        if (statusCode >= 300 && statusCode < 400)
        {
            char location[512];
            char resolved[MAX_URL_SIZE];

            location[0] = '\0';
            resolved[0] = '\0';

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
                printf("[UPDATE] redirect without Location\n");
                return false;
            }

            if (!ResolveRedirectUrl(currentUrl, location, resolved, sizeof(resolved)))
            {
                printf("[UPDATE] cannot resolve redirect URL\n");
                return false;
            }

            printf("[UPDATE] redirect -> %s\n", resolved);

            if (ExtractReleaseTagFromUrl(resolved, tagOutput, tagOutputSize))
            {
                return true;
            }

            snprintf(currentUrl, sizeof(currentUrl), "%s", resolved);
            continue;
        }

        if (statusCode == 200)
        {
            DrainHttpBody(&context);
            CloseHttp(&context);

            if (ExtractReleaseTagFromUrl(currentUrl, tagOutput, tagOutputSize))
            {
                return true;
            }

            printf("[UPDATE] latest release tag not found\n");
            return false;
        }

        DrainHttpBody(&context);
        CloseHttp(&context);

        printf("[UPDATE] bad latest release status\n");
        return false;
    }

    printf("[UPDATE] too many redirects while checking latest release\n");
    return false;
}

static bool DownloadFileInternal(
    const char* url,
    const char* outputPath,
    int redirectDepth)
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

    httpcContext context;
    u32 statusCode = 0;

    if (!OpenHttpGet(url, &context, &statusCode))
    {
        return false;
    }

    if (statusCode >= 300 && statusCode < 400)
    {
        char location[512];
        char resolved[MAX_URL_SIZE];

        location[0] = '\0';
        resolved[0] = '\0';

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

        if (!ResolveRedirectUrl(url, location, resolved, sizeof(resolved)))
        {
            printf("[HTTP] cannot resolve redirect URL\n");
            return false;
        }

        printf("[HTTP] redirect -> %s\n", resolved);

        return DownloadFileInternal(resolved, outputPath, redirectDepth + 1);
    }

    if (statusCode != 200)
    {
        printf("[HTTP] bad status code\n");
        DrainHttpBody(&context);
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

    while (true)
    {
        u32 downloaded = 0;

        Result rc = httpcDownloadData(
            &context,
            buffer,
            DOWNLOAD_BUFFER_SIZE,
            &downloaded
        );

        if (downloaded > 0)
        {
            size_t written = fwrite(buffer, 1, downloaded, file);

            if (written != downloaded)
            {
                printf("[HTTP] fwrite failed\n");
                fclose(file);
                DeleteFileIfExists(tmpPath);
                CloseHttp(&context);
                return false;
            }
        }

        if (rc == 0)
        {
            break;
        }

        if (rc != (Result)HTTPC_RESULTCODE_DOWNLOADPENDING)
        {
            printf("[HTTP] download failed: 0x%08lX\n", (unsigned long)rc);
            fclose(file);
            DeleteFileIfExists(tmpPath);
            CloseHttp(&context);
            return false;
        }

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

// ------------------------------------------------------------
// Version compare
// ------------------------------------------------------------

static void ClearUpdateInfo(UpdateInfo* info)
{
    if (!info)
    {
        return;
    }

    memset(info, 0, sizeof(UpdateInfo));
    info->hasUpdate = false;
}

static const char* SkipVersionPrefix(const char* version)
{
    if (!version)
    {
        return "";
    }

    if (version[0] == 'v' || version[0] == 'V')
    {
        return version + 1;
    }

    return version;
}

static int ReadNumber(const char** p)
{
    int value = 0;

    while (**p && isdigit((unsigned char)**p))
    {
        value = value * 10 + (**p - '0');
        (*p)++;
    }

    return value;
}

static int CompareMainVersion(const char* current, const char* latest)
{
    const char* c = SkipVersionPrefix(current);
    const char* l = SkipVersionPrefix(latest);

    for (int part = 0; part < 3; part++)
    {
        while (*c && !isdigit((unsigned char)*c))
        {
            if (*c == '-')
            {
                break;
            }

            c++;
        }

        while (*l && !isdigit((unsigned char)*l))
        {
            if (*l == '-')
            {
                break;
            }

            l++;
        }

        int cn = ReadNumber(&c);
        int ln = ReadNumber(&l);

        if (ln > cn)
        {
            return 1;
        }

        if (ln < cn)
        {
            return -1;
        }

        if (*c == '.')
        {
            c++;
        }

        if (*l == '.')
        {
            l++;
        }
    }

    return 0;
}

static const char* FindPrerelease(const char* version)
{
    const char* p = strchr(SkipVersionPrefix(version), '-');

    if (!p)
    {
        return NULL;
    }

    return p + 1;
}

static int ComparePrerelease(const char* current, const char* latest)
{
    const char* c = FindPrerelease(current);
    const char* l = FindPrerelease(latest);

    // Stable > beta/prerelease.
    if (!c && l)
    {
        return -1;
    }

    if (c && !l)
    {
        return 1;
    }

    if (!c && !l)
    {
        return 0;
    }

    while (*c || *l)
    {
        while (*c == '.' || *c == '-')
        {
            c++;
        }

        while (*l == '.' || *l == '-')
        {
            l++;
        }

        bool cNum = isdigit((unsigned char)*c);
        bool lNum = isdigit((unsigned char)*l);

        if (cNum && lNum)
        {
            int cn = ReadNumber(&c);
            int ln = ReadNumber(&l);

            if (ln > cn)
            {
                return 1;
            }

            if (ln < cn)
            {
                return -1;
            }

            continue;
        }

        char cc = *c;
        char lc = *l;

        if (lc > cc)
        {
            return 1;
        }

        if (lc < cc)
        {
            return -1;
        }

        if (*c)
        {
            c++;
        }

        if (*l)
        {
            l++;
        }
    }

    return 0;
}

static int CompareVersions(const char* current, const char* latest)
{
    int mainCompare = CompareMainVersion(current, latest);

    if (mainCompare != 0)
    {
        return mainCompare;
    }

    return ComparePrerelease(current, latest);
}

// ------------------------------------------------------------
// Public update API
// ------------------------------------------------------------

bool CheckForUpdate(
    const char* currentVersion,
    const char* latestReleaseUrl,
    UpdateInfo* outInfo)
{
    if (!currentVersion || !latestReleaseUrl || !outInfo)
    {
        return false;
    }

    ClearUpdateInfo(outInfo);

    printf("[UPDATE] checking latest GitHub release...\n");

    if (!GetLatestReleaseTag(
            latestReleaseUrl,
            outInfo->version,
            sizeof(outInfo->version)))
    {
        printf("[UPDATE] cannot get latest release tag\n");
        return false;
    }

    snprintf(
        outInfo->normalizedVersion,
        sizeof(outInfo->normalizedVersion),
        "%s",
        outInfo->version
    );

    BuildLatestDownloadUrl(
        latestReleaseUrl,
        "ReSharp3DS.3dsx",
        outInfo->url3dsx,
        sizeof(outInfo->url3dsx)
    );

    BuildLatestDownloadUrl(
        latestReleaseUrl,
        "ReSharp3DS.cia",
        outInfo->urlcia,
        sizeof(outInfo->urlcia)
    );

    BuildLatestDownloadUrl(
        latestReleaseUrl,
        "ReSharp3DS.elf",
        outInfo->urlelf,
        sizeof(outInfo->urlelf)
    );

    int compare = CompareVersions(currentVersion, outInfo->version);

    outInfo->hasUpdate = compare > 0;

    printf(
        "[UPDATE] current=%s latest=%s update=%d\n",
        currentVersion,
        outInfo->version,
        outInfo->hasUpdate ? 1 : 0
    );

    return true;
}