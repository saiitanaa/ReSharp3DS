# ReSharp3DS Builder

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS">GitHub Main</a>
</p>

**ReSharp3DS Builder replaces ReSharp3DS Studio for compiling your homebrew!**

**Take advantage of an easy CLI environment to compile your homebrew into .pe files**

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
