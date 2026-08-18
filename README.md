# ReSharp3DS Builder

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS">GitHub Main</a>
</p>

**ReSharp3DS Builder replaces ReSharp3DS Studio for compiling your homebrew!**

**Take advantage of an easy CLI environment to compile your homebrew into .pe files**

## Download

Pre-built binaries are available on the [Releases page](https://github.com/saiitanaa/ReSharp3DS/releases/tag/builder-v1.0.0).

| Platform | Architecture | Download |
|---|---|---|
| macOS | Apple Silicon (M1/M2/M3) | [osx-arm64.zip](https://github.com/saiitanaa/ReSharp3DS/releases/download/builder-v1.0.0/osx-arm64.zip) |
| macOS | Intel | [osx-x64.zip](https://github.com/saiitanaa/ReSharp3DS/releases/download/builder-v1.0.0/osx-x64.zip) |
| Linux | x64 | [linux-x64.zip](https://github.com/saiitanaa/ReSharp3DS/releases/download/builder-v1.0.0/linux-x64.zip) |
| Linux | ARM64 | [linux-arm64.zip](https://github.com/saiitanaa/ReSharp3DS/releases/download/builder-v1.0.0/linux-arm64.zip) |
| Windows | x64 | [win-x64.zip](https://github.com/saiitanaa/ReSharp3DS/releases/download/builder-v1.0.0/win-x64.zip) |
| Windows | ARM64 | [win-arm64.zip](https://github.com/saiitanaa/ReSharp3DS/releases/download/builder-v1.0.0/win-arm64.zip) |

Not sure which one to pick? macOS with M1/M2/M3 → `osx-arm64`. Older Intel Mac → `osx-x64`. Most Windows/Linux PCs → `x64`.

## Running the compiled binary

Extract the downloaded `.zip`, then:

### macOS
```bash
chmod +x ReSharp3DS.Compiler
./ReSharp3DS.Compiler --project /path/to/YourApp
```
> macOS may block the binary as "unidentified developer". Either right-click the file → **Open**,
> or run: `xattr -d com.apple.quarantine ReSharp3DS.Compiler`

### Linux
```bash
chmod +x ReSharp3DS.Compiler
./ReSharp3DS.Compiler --project /path/to/YourApp
```

### Windows
```powershell
.\ReSharp3DS.Compiler.exe --project C:\YourApp
```
> SmartScreen may warn about an unrecognized app — click **More info** → **Run anyway**.

**Requirements:**
- **macOS / Linux:** `mono` must be installed (`brew install mono` / `sudo apt install mono-complete` / `sudo dnf install mono-complete`), used to run the MetadataProcessor step.
- **Windows:** no extra dependency — MetadataProcessor runs natively.
- The .NET runtime is bundled in the binary (self-contained build), so the .NET SDK is **not** required to run it.
- 
## Run

### macOS
```bash
chmod +x ReSharp3DS.Compiler
./ReSharp3DS.Compiler --project /path/to/YourApp
```
> macOS may block the binary as "unidentified developer". Either right-click → Open,
> or run: `xattr -d com.apple.quarantine ReSharp3DS.Compiler`

### Linux
```bash
chmod +x ReSharp3DS.Compiler
./ReSharp3DS.Compiler --project /path/to/YourApp
```

### Windows
```powershell
.\ReSharp3DS.Compiler.exe --project C:\YourApp
```
> SmartScreen may warn about an unrecognized app — click "More info" → "Run anyway".

**Note:** macOS and Linux require `mono` installed (`brew install mono` / `apt install mono-complete`).

## Usage

### Run from source
```bash
dotnet run -- --project /path/to/YourApp --output YourApp.pe
```

### Build a standalone executable

| Platform | Command |
|---|---|
| macOS (Apple Silicon) | `dotnet publish -c Release -r osx-arm64 --self-contained true -p:PublishSingleFile=true -o ./dist` |
| macOS (Intel) | `dotnet publish -c Release -r osx-x64 --self-contained true -p:PublishSingleFile=true -o ./dist` |
| Linux | `dotnet publish -c Release -r linux-x64 --self-contained true -p:PublishSingleFile=true -o ./dist` |
| Windows | `dotnet publish -c Release -r win-x64 --self-contained true -p:PublishSingleFile=true -o ./dist` |

macOS and Linux additionally require `mono` (`brew install mono` / `apt install mono-complete`) to run the .NET Framework-based MetadataProcessor.
