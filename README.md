[<img src="https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ReSharp3DS&section=header&reversal=false&textBg=false&descAlign=64" />
](https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ReSharp3DS%20SDK)

ReSharp3DS is an project that runs C# code on the Nintendo 3DS using nanoCLR / nanoFramework.

The project uses a C++ 3DS homebrew application to load C# assemblies compiled as `.pe` files, then executes them through nanoCLR.

## Screenshots

<img width="400" height="300" alt="IMG_0847" src="https://github.com/user-attachments/assets/43bd502f-863b-4c4b-b772-6c1b70d53de2" />
<img width="400" height="300" alt="IMG_0986" src="https://github.com/user-attachments/assets/2100b5c3-13ba-4d08-b1ed-7b011a59c38d" />


## Links

<p align="center">
  <a href="https://github.com/saiitanaa/ReSharp3DS/tree/docs">ReSharp3DS Documentation</a>
</p>

<p align="center">
  <a href="https://github.com/saiitanaa/ReSharp3DS/tree/sdk">ReSharp3DS - SDK</a>
</p>

<p align="center">
  <a href="https://github.com/saiitanaa/ReSharp3DS-Studio">ReSharp3DS Studio</a>
</p>

<p align="center">
  <a href="https://discord.gg/RZPgeDpCNQ">Discord server</a>
</p>

<p align="center">
  <a href="https://db.universal-team.net/3ds/resharp3ds">Universal-Updater</a>
</p>

## Progress & Roadmap

<details>
<summary><b>Done (Click to expand)</b></summary>

* [x] Initialize nanoCLR on Nintendo 3DS
* [x] Load `mscorlib.pe` and `app.pe` from SD card
* [x] Execute C# `Program.Main()`
* [x] Call native C++ functions from C# using `InternalCall`
* [x] Basic Console API (`Clear`, `Write`, `WriteLine`)
* [x] Input support (`Start`, `Select`)
* [x] Full Button Mapping (A, B, X, Y, D-Pad, L, R)
* [x] Runtime management (`Runtime.Yield()`, static state preservation)
* [x] Validate on Citra & Real Hardware
* [x] Fix screen flickering by avoiding full redraw every tick
* [x] Graphics & Audio API
* [x] Filesystem support
* [x] Automatic native method binding instead of index-based mapping
* [x] Runtime launcher with folder explorer
* [x] Launch `.pe` files from folders
* [x] Automatic `mscorlib.pe` restore
* [x] Time API
* [x] Random API
* [x] Touch API
* [x] CirclePad API
* [x] Screen constants
* [x] App API
* [x] SystemInfo API
* [x] Save API
* [x] `Graphics.DrawBitmap` BMP support
* [x] `Audio.SetSfxVolume`
* [x] `Audio.SetMusicVolume`
* [x] `Audio.IsPlaying`
* [x] `Audio.IsMusicPlaying`

</details>

<details>
<summary><b>To be implemented (Click to expand)</b></summary>

* [ ] Expanded Console API (bool, float, better formatting)
* [ ] Better error reporting for C# exceptions
* [ ] Better SystemInfo values on real hardware

</details>

---

## File Structure

Recommended SD card layout for current runtime versions:

```txt
SD:/
├── 3ds/
│   └── ReSharp3DS.3dsx
└── ReSharp3DS/
    ├── bin/
    │   └── mscorlib.pe
    ├── app.pe
    ├── apps/
    │   └── TestApp/
    │       ├── manifest.json
    │       ├── app.pe
    │       └── assets/
    │           ├── sprite.bmp
    │           └── sound.wav
    └── logs/
        ├── crash.txt
        ├── clr-panic.txt
        └── <AppName>.log
```

The runtime dependency is expected at:

```txt
sdmc:/ReSharp3DS/bin/mscorlib.pe
```

The launcher scans:

```txt
sdmc:/ReSharp3DS/
```

and displays folders and `.pe` applications. `mscorlib.pe` is ignored by the launcher.

## Launcher controls

Current runtime versions include a file browser with folder, `.pe`, manifest, and touch support.

```txt
D-Pad Up / Down      Move selection
A                    Open folder or launch selected .pe app
X                    Refresh current folder
START                Exit launcher
Touch row            Select/open folder or launch selected app
Touch right edge     Quick scroll
```

If a `.pe` file is declared in `manifest.json`, the launcher can display the manifest `name`, `author`, and `version` instead of the raw `.pe` filename.

## Runtime Network / Dependency Restore

Current runtime versions no longer use the old runtime auto-update flow in this module.

The remaining network helper is focused on runtime file restore and downloads, especially restoring the nanoFramework core library when it is missing.

Available helper responsibilities:

```txt
FileExists
DirectoryExists
EnsureDirectory
DownloadFile
```

The runtime dependency can be restored from:

```txt
https://host.saysaa.fr/3ds/ReSharp3DS/dependencies/mscorlib.pe
```

The expected local path is:

```txt
sdmc:/ReSharp3DS/bin/mscorlib.pe
```

If automatic restore fails, copy `mscorlib.pe` manually to that path.

## ReSharp3DS API

Current runtime and SDK versions include APIs for console output, input, touch, runtime control, timing, graphics, audio, files, directories, saves, app information, and system information.

Available APIs include:

```txt
Console API
Debug API
Input API
Runtime API
Time API
Random API
Touch API
CirclePad API
Screen constants
App API
SystemInfo API
Graphics API
Audio API
File API
Directory API
Save API
```

Newer SDK helpers include:

```txt
Button
ScreenTarget
Vector2
Rect
Timer
GameApp
Input.IsHeld(Button)
Input.IsPressed(Button)
Input.CirclePad()
Touch.Position()
Runtime.ReturnToLauncher()
Graphics.SetTarget(ScreenTarget.Top/Bottom)
Graphics.DrawSpriteTransparent(...)
Save.Exists(...)
Save.Delete(...)
Save.SetBool(...)
Save.GetBool(...)
```

## Time API

```csharp
int ms = Time.Milliseconds();
int seconds = Time.Seconds();
```

## Random API

```csharp
Random.Seed(1234);
int value = Random.Next(0, 100);
```

## Touch API

```csharp
bool pressed = Touch.IsPressed();
int x = Touch.X();
int y = Touch.Y();
```

## CirclePad API

```csharp
int x = Input.CirclePadX();
int y = Input.CirclePadY();
```

## Screen constants

```csharp
Screen.TopWidth
Screen.TopHeight
Screen.BottomWidth
Screen.BottomHeight
```

## App API

```csharp
string path = App.GetPath();
string dir = App.GetDirectory();
string name = App.GetName();
```

## SystemInfo API

```csharp
bool isNew3DS = SystemInfo.IsNew3DS();
int battery = SystemInfo.GetBatteryLevel();
int memory = SystemInfo.GetFreeMemory();
```

Some values may return fallback values depending on hardware support.

Current fallback behavior:

```txt
GetBatteryLevel() returns -1 if unavailable
GetFreeMemory() returns 0 if unavailable
```

## Graphics API

Available methods include:

```csharp
Graphics.Clear(int color);
Graphics.DrawPixel(int x, int y, int color);
Graphics.FillRect(int x, int y, int width, int height, int color);
Graphics.DrawRect(int x, int y, int width, int height, int color);
Graphics.DrawLine(int x1, int y1, int x2, int y2, int color);
Graphics.DrawCircle(int x, int y, int radius, int color);
Graphics.FillCircle(int x, int y, int radius, int color);
Graphics.DrawText(int x, int y, string text, int color);
Graphics.DrawBitmap(string path, int x, int y);
Graphics.DrawSprite(string path, int x, int y);
Graphics.DrawSprite(string path, int x, int y, int width, int height);
Graphics.DrawSpriteTransparent(string path, int x, int y, int transparentColor);
Graphics.SetTarget(ScreenTarget.Top);
Graphics.SetTarget(ScreenTarget.Bottom);
Graphics.Present();
```

`Graphics.DrawBitmap`, `Graphics.DrawSprite`, and `Graphics.DrawSpriteTransparent` support simple BMP files.

Recommended format:

```txt
BMP
24-bit or 32-bit
uncompressed
```

## Audio API

Available methods include:

```csharp
Audio.Init();
Audio.Beep(int frequency, int durationMs);
Audio.Stop();

Audio.PlayWav(string path);
Audio.Loop(string path);
Audio.StopMusic();

Audio.SetVolume(int volume);
Audio.SetSfxVolume(int volume);
Audio.SetMusicVolume(int volume);

Audio.IsPlaying();
Audio.IsMusicPlaying();
```

WAV files should be:

```txt
PCM WAV
16-bit
44100 Hz or 22050 Hz
mono or stereo
```

For real hardware, DSP audio must be available on the SD card.

## Save API

```csharp
Save.SetInt("score", 1200);
int score = Save.GetInt("score", 0);

Save.SetString("name", "Player");
string name = Save.GetString("name", "Default");

Save.SetBool("unlocked", true);
bool unlocked = Save.GetBool("unlocked", false);

bool exists = Save.Exists("score");
Save.Delete("score");
```

## File and Directory API

```csharp
File.Exists(string path);
File.WriteAllText(string path, string text);
File.ReadAllText(string path);
File.Delete(string path);

Directory.Exists(string path);
Directory.Create(string path);
Directory.Delete(string path);
```

Paths can be relative to the running `.pe` application folder.

---

### Troubleshooting

If a file is missing or incorrectly placed, the program will display an error:
`[FATAL] app load failed`

Make sure the `ReSharp3DS` folder is at the **root** of the SD card, not inside the `/3ds/` folder.

### `[FATAL] mscorlib load failed`

Check that:

```txt
sdmc:/ReSharp3DS/bin/mscorlib.pe
```

exists and matches the nanoFramework version used by the SDK.

### The launcher shows no apps

Check that your `.pe` files are inside:

```txt
sdmc:/ReSharp3DS/
```

or inside a subfolder.

### BMP does not draw

Check that the file is:

```txt
BMP
24-bit or 32-bit
uncompressed
```

Also check that the path is relative to the running `.pe` file.

### Audio does not work on real hardware

Make sure DSP has been dumped correctly and is available on the SD card.

---

## Changelog

## ReSharp3DS Runtime Update

### Added

- Added `manifest.json` support in the launcher.
- Added manifest-based `.pe` display names.
- Added app metadata support through manifest fields:
  - `name`
  - `author`
  - `version`
  - `entry`
  - `description`
- Added selected app metadata display in the launcher:
  - `Author: ...`
  - `Version: ...`
- Added touchscreen support in the launcher.
- Added improved launcher file browsing and app/folder sorting.
- Added `runtime_net.cpp` and `runtime_net.h` for runtime network/file helpers.
- Added helper functions for dependency restore:
  - `FileExists`
  - `DirectoryExists`
  - `EnsureDirectory`
  - `DownloadFile`
- Added basic crash log and crash screen infrastructure.
- Added runtime log paths:
  - `sdmc:/ReSharp3DS/logs/crash.txt`
  - `sdmc:/ReSharp3DS/logs/clr-panic.txt`
  - `sdmc:/ReSharp3DS/logs/<AppName>.log`
- Added centralized input snapshot logic for held buttons, pressed-once buttons, touch, and Circle Pad.
- Added graphics target support for top and bottom screen rendering.
- Added transparent sprite drawing support.

### Changed

- The launcher now uses `manifest.json` to rename the `.pe` entry declared by the manifest `entry` field.
- Replaced the old updater-style code with a smaller runtime network helper.
- Runtime network code is now focused on dependency restore, especially `mscorlib.pe`.
- Input handling now avoids repeated `hidScanInput()` calls from individual native functions.

### Removed

- Removed the old `UpdateInfo` / `CheckForUpdate` style update flow from the launcher side.
- Removed runtime auto-update behavior from this part of the project.

### Fixed

- Fixed manifest display behavior so a `.pe` file can be displayed with the manifest name.
- Fixed author/version metadata rendering placement.
- Fixed generated C++ string literal issues caused by broken `\n` escaping.
- Fixed duplicate `copy_json_string_value(...)` definition during manifest integration.
- Fixed invalid `\0` handling caused by embedded null characters in generated source.

## ReSharp3DS API Update

### Added

- Added native binding for returning to the launcher:
  - `RuntimeReturnToLauncher`
- Added native debug log binding:
  - `DebugLogFile`
- Added native graphics target binding:
  - `GraphicsSetTarget`
- Added native transparent sprite binding:
  - `GraphicsDrawSpriteTransparent`
- Added native support for top/bottom screen drawing target selection.
- Added native debug file logging support.
- Added native system info improvements for battery level, free memory, and New 3DS detection.

### Changed

- Native input calls now read from a cached input snapshot.
- Native graphics code can now target different screens.
- Native debug logging is prepared to write logs into the ReSharp3DS logs folder.

### Fixed

- Improved pressed-once input stability using frame-based state.
- Fixed native-side string literal issues generated during integration.

## ReSharp3DS SDK Update

### Added

- Added C# helper enum:
  - `Button`
- Added C# screen target enum:
  - `ScreenTarget`
- Added C# helper structs:
  - `Vector2`
  - `Rect`
- Added C# utility class:
  - `Timer`
- Added C# app base class:
  - `GameApp`
- Added input helpers:
  - `Input.IsHeld(Button button)`
  - `Input.IsPressed(Button button)`
  - `Input.CirclePad()`
- Added touch helper:
  - `Touch.Position()`
- Added runtime helper:
  - `Runtime.ReturnToLauncher()`
- Added debug helpers:
  - `Debug.Log(...)`
  - `Debug.Warn(...)`
  - `Debug.Error(...)`
  - `Debug.LogInt(...)`
- Added graphics helpers:
  - `Graphics.SetTarget(ScreenTarget.Top)`
  - `Graphics.SetTarget(ScreenTarget.Bottom)`
  - `Graphics.DrawSpriteTransparent(...)`
- Added save helpers:
  - `Save.Exists(...)`
  - `Save.Delete(...)`
  - `Save.SetBool(...)`
  - `Save.GetBool(...)`

### Changed

- Improved SDK usability for C# app developers.
- Made input code cleaner with button enums.
- Made touch handling cleaner with a `Vector2` helper.
- Made save data easier to use with boolean helpers.

### Fixed

- Fixed `UI.DrawProgressBar(...)` to avoid invalid or negative fill sizes.
- Documented that apps must not compile two copies of `ReSharp3DS.cs`.

