using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Linq;
using System.Net.Http;
using System.Threading.Tasks;

const string CoreLibraryPackage = "nanoFramework.CoreLibrary";
const string CoreLibraryVersion = "1.9.0-preview.11";

const string MetadataProcessorPackage = "nanoFramework.Tools.MetadataProcessor.CLI";
const string MetadataProcessorVersion = "3.0.100";

string? projectRoot = null;
string outputName = "app.pe";

for (int i = 0; i < args.Length; i++)
{
    if (args[i] == "--project" && i + 1 < args.Length)
    {
        projectRoot = Path.GetFullPath(args[++i]);
    }
    else if (args[i] == "--output" && i + 1 < args.Length)
    {
        outputName = args[++i];
    }
}

if (string.IsNullOrWhiteSpace(projectRoot))
{
    Console.Write("Project folder path: ");
    string? input = Console.ReadLine();

    if (string.IsNullOrWhiteSpace(input))
    {
        Console.Error.WriteLine("No path provided.");
        return 2;
    }

    input = input.Trim().Trim('"');
    projectRoot = Path.GetFullPath(input);
}

if (!Directory.Exists(projectRoot))
{
    Console.Error.WriteLine($"Project folder not found: {projectRoot}");
    return 3;
}

if (!outputName.EndsWith(".pe", StringComparison.OrdinalIgnoreCase))
{
    outputName += ".pe";
}

string assemblyName = Path.GetFileNameWithoutExtension(outputName);

string objDir = Path.Combine(projectRoot, "obj", "resharp3ds-il");
string distDir = Path.Combine(projectRoot, "dist");

Directory.CreateDirectory(objDir);
Directory.CreateDirectory(distDir);

string dllPath = Path.Combine(objDir, assemblyName + ".dll");
string pePath = Path.Combine(distDir, outputName);
string rspPath = Path.Combine(objDir, "compile.rsp");

string packagesDir = Path.Combine(
    Environment.GetFolderPath(Environment.SpecialFolder.UserProfile),
    ".resharp3ds-studio",
    "packages"
);

Console.WriteLine("ReSharp3DS Compiler");
Console.WriteLine($"Project: {projectRoot}");
Console.WriteLine($"Output: {pePath}");
Console.WriteLine($"CoreLibrary: {CoreLibraryPackage} {CoreLibraryVersion}");
Console.WriteLine($"MetadataProcessor: {MetadataProcessorPackage} {MetadataProcessorVersion}");
Console.WriteLine();

string coreLibraryDir = await EnsurePackageAsync(CoreLibraryPackage, CoreLibraryVersion);
string metadataProcessorDir = await EnsurePackageAsync(MetadataProcessorPackage, MetadataProcessorVersion);

string mscorlibDll = Path.Combine(coreLibraryDir, "lib", "mscorlib.dll");

if (!File.Exists(mscorlibDll))
{
    Console.Error.WriteLine("mscorlib.dll not found.");
    Console.Error.WriteLine(mscorlibDll);
    return 10;
}

string? metadataProcessorExe = Directory
    .EnumerateFiles(metadataProcessorDir, "nanoFramework.Tools.MetadataProcessor.exe", SearchOption.AllDirectories)
    .FirstOrDefault();

if (metadataProcessorExe == null)
{
    Console.Error.WriteLine("MetadataProcessor executable not found.");
    Console.Error.WriteLine(metadataProcessorDir);
    return 11;
}

string? cscDll = FindRoslynCscDll();

if (cscDll == null)
{
    Console.Error.WriteLine("Roslyn csc.dll not found.");
    Console.Error.WriteLine("Install the .NET SDK.");
    return 12;
}

Console.WriteLine($"Roslyn: {cscDll}");
Console.WriteLine($"mscorlib.dll: {mscorlibDll}");
Console.WriteLine($"MetadataProcessor.exe: {metadataProcessorExe}");
Console.WriteLine();

string[] sources = Directory
    .EnumerateFiles(projectRoot, "*.cs", SearchOption.AllDirectories)
    .Where(IsSourceFile)
    .OrderBy(source => Path.GetFileName(source).Equals("ReSharp3DS.cs", StringComparison.OrdinalIgnoreCase) ? 0 : 1)
    .ThenBy(source => source)
    .ToArray();

if (sources.Length == 0)
{
    Console.Error.WriteLine("No C# source files found.");
    return 13;
}

Console.WriteLine("Source files:");

foreach (string source in sources)
{
    Console.WriteLine($"  {Path.GetRelativePath(projectRoot, source)}");
}

Console.WriteLine();

WriteResponseFile(rspPath, sources, mscorlibDll, dllPath);

DeleteIfExists(dllPath);
DeleteIfExists(pePath);

int cscExit = RunProcess(
    "dotnet",
    $"{Quote(cscDll)} @{Quote(rspPath)}",
    projectRoot
);

if (cscExit != 0)
{
    Console.Error.WriteLine("C# compilation failed.");
    return cscExit;
}

if (!File.Exists(dllPath))
{
    Console.Error.WriteLine($"C# compiler finished but DLL was not created: {dllPath}");
    return 14;
}

Console.WriteLine();
Console.WriteLine("C# compilation completed.");
Console.WriteLine(dllPath);
Console.WriteLine();

string metadataProcessorArgs =
    $"-loadHints mscorlib {Quote(mscorlibDll)} " +
    $"-parse {Quote(dllPath)} " +
    $"-compile {Quote(pePath)} false";

int metadataProcessorExit;

if (OperatingSystem.IsWindows())
{
    metadataProcessorExit = RunProcess(metadataProcessorExe, metadataProcessorArgs, projectRoot);
}
else
{
    metadataProcessorExit = RunProcess("mono", $"{Quote(metadataProcessorExe)} {metadataProcessorArgs}", projectRoot);
}

if (metadataProcessorExit != 0)
{
    Console.Error.WriteLine("MetadataProcessor failed.");
    return metadataProcessorExit;
}

if (!File.Exists(pePath))
{
    Console.Error.WriteLine($"MetadataProcessor finished but PE was not created: {pePath}");
    return 15;
}

Console.WriteLine();
Console.WriteLine("Build completed.");
Console.WriteLine(pePath);

return 0;

bool IsSourceFile(string file)
{
    string relative = Path.GetRelativePath(projectRoot, file).Replace('\\', '/');
    string normalized = "/" + relative;

    return !normalized.Contains("/bin/", StringComparison.OrdinalIgnoreCase)
        && !normalized.Contains("/obj/", StringComparison.OrdinalIgnoreCase)
        && !normalized.Contains("/dist/", StringComparison.OrdinalIgnoreCase)
        && !normalized.Contains("/node_modules/", StringComparison.OrdinalIgnoreCase)
        && !normalized.Contains("/.git/", StringComparison.OrdinalIgnoreCase)
        && !normalized.Contains("/compiler/", StringComparison.OrdinalIgnoreCase);
}

void WriteResponseFile(string path, string[] sourceFiles, string mscorlib, string outputDll)
{
    var lines = new List<string>
    {
        "/noconfig",
        "/nostdlib+",
        "/target:exe",
        "/debug-",
        "/optimize+",
        "/langversion:latest",
        "/out:" + Quote(outputDll),
        "/reference:" + Quote(mscorlib)
    };

    foreach (string source in sourceFiles)
    {
        lines.Add(Quote(source));
    }

    File.WriteAllLines(path, lines);
}

async Task<string> EnsurePackageAsync(string packageId, string version)
{
    string packageDir = Path.Combine(packagesDir, $"{packageId}.{version}");
    string marker = Path.Combine(packageDir, ".ok");

    if (File.Exists(marker))
    {
        return packageDir;
    }

    Directory.CreateDirectory(packagesDir);

    if (Directory.Exists(packageDir))
    {
        Directory.Delete(packageDir, recursive: true);
    }

    Directory.CreateDirectory(packageDir);

    string nupkgPath = Path.Combine(packagesDir, $"{packageId}.{version}.nupkg");
    string url = $"https://www.nuget.org/api/v2/package/{packageId}/{version}";

    Console.WriteLine($"Downloading {packageId} {version}");
    Console.WriteLine(url);

    using var http = new HttpClient();
    byte[] data = await http.GetByteArrayAsync(url);

    await File.WriteAllBytesAsync(nupkgPath, data);

    ZipFile.ExtractToDirectory(nupkgPath, packageDir, overwriteFiles: true);
    await File.WriteAllTextAsync(marker, DateTime.UtcNow.ToString("O"));

    return packageDir;
}

string? FindRoslynCscDll()
{
    var roots = new List<string>();

    // 1. Known paths (fast fallback)
    string? dotnetRoot = Environment.GetEnvironmentVariable("DOTNET_ROOT");
    if (!string.IsNullOrWhiteSpace(dotnetRoot))
    {
        roots.Add(Path.Combine(dotnetRoot, "sdk"));
    }

    roots.Add("/usr/share/dotnet/sdk");
    roots.Add("/usr/lib/dotnet/sdk");
    roots.Add("/snap/dotnet-sdk/current/sdk");
    roots.Add("/usr/local/share/dotnet/sdk");
    roots.Add("/opt/homebrew/opt/dotnet/libexec/sdk");
    roots.Add("/usr/local/opt/dotnet/libexec/sdk");

    string home = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
    if (!string.IsNullOrWhiteSpace(home))
    {
        roots.Add(Path.Combine(home, ".dotnet", "sdk"));
    }

    // 2. Reliable fallback: ask dotnet directly where its SDK is
    foreach (string sdkPath in GetSdkRootsFromDotnetCli())
    {
        roots.Add(sdkPath);
    }

    foreach (string root in roots.Distinct())
    {
        if (!Directory.Exists(root))
        {
            continue;
        }

        string? found = Directory
            .EnumerateFiles(root, "csc.dll", SearchOption.AllDirectories)
            .Where(path => path.Replace('\\', '/').Contains("/Roslyn/bincore/"))
            .OrderByDescending(GetSdkVersionFromPath)
            .FirstOrDefault();

        if (found != null)
        {
            return found;
        }
    }

    return null;
}

IEnumerable<string> GetSdkRootsFromDotnetCli()
{
    // "dotnet --list-sdks" prints something like:
    // 10.0.100 [/usr/local/share/dotnet/sdk]
    var startInfo = new ProcessStartInfo
    {
        FileName = "dotnet",
        Arguments = "--list-sdks",
        RedirectStandardOutput = true,
        UseShellExecute = false
    };

    using var process = Process.Start(startInfo);
    if (process == null)
    {
        yield break;
    }

    string output = process.StandardOutput.ReadToEnd();
    process.WaitForExit();

    foreach (string line in output.Split('\n', StringSplitOptions.RemoveEmptyEntries))
    {
        int open = line.IndexOf('[');
        int close = line.IndexOf(']');

        if (open >= 0 && close > open)
        {
            yield return line.Substring(open + 1, close - open - 1);
        }
    }
}

Version GetSdkVersionFromPath(string value)
{
    try
    {
        string[] parts = value.Replace('\\', '/').Split('/');
        int sdkIndex = Array.FindIndex(parts, part => part == "sdk");

        if (sdkIndex >= 0 && sdkIndex + 1 < parts.Length)
        {
            string versionText = parts[sdkIndex + 1].Split('-')[0];

            if (Version.TryParse(versionText, out Version? version))
            {
                return version;
            }
        }
    }
    catch
    {
    }

    return new Version(0, 0);
}

int RunProcess(string fileName, string arguments, string workingDirectory)
{
    Console.WriteLine($"$ {fileName} {arguments}");
    Console.WriteLine();

    var startInfo = new ProcessStartInfo
    {
        FileName = fileName,
        Arguments = arguments,
        WorkingDirectory = workingDirectory,
        RedirectStandardOutput = true,
        RedirectStandardError = true,
        UseShellExecute = false
    };

    using var process = new Process { StartInfo = startInfo };

    process.OutputDataReceived += (_, e) => { if (e.Data != null) Console.WriteLine(e.Data); };
    process.ErrorDataReceived += (_, e) => { if (e.Data != null) Console.Error.WriteLine(e.Data); };

    try
    {
        process.Start();
    }
    catch (System.ComponentModel.Win32Exception)
    {
        Console.Error.WriteLine();
        Console.Error.WriteLine($"Error: could not start '{fileName}'.");
        Console.Error.WriteLine($"Make sure it is installed and available in your PATH (e.g. brew install mono).");
        return -1;
    }

    process.BeginOutputReadLine();
    process.BeginErrorReadLine();
    process.WaitForExit();

    Console.WriteLine();
    Console.WriteLine($"Exit code: {process.ExitCode}");

    return process.ExitCode;
}

void DeleteIfExists(string path)
{
    if (File.Exists(path))
    {
        File.Delete(path);
    }
}

string Quote(string value)
{
    return "\"" + value.Replace("\"", "\\\"") + "\"";
}