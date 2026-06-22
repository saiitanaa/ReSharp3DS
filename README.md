[<img src="https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ReSharp3DS&section=header&reversal=false&textBg=false&descAlign=64" />
](https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ReSharp3DS%20SDK)

ReSharp3DS is an experimental project that runs C# code on the Nintendo 3DS using nanoCLR / nanoFramework.

The project uses a C++ 3DS homebrew application to load C# assemblies compiled as `.pe` files, then executes them through nanoCLR.

### Screenshots

<img width="400" height="300" alt="IMG_0847" src="https://github.com/user-attachments/assets/43bd502f-863b-4c4b-b772-6c1b70d53de2" />
<img width="400" height="300" alt="IMG_0986" src="https://github.com/user-attachments/assets/2100b5c3-13ba-4d08-b1ed-7b011a59c38d" />


## Links

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS/tree/docs">ReSharp3DS Documentation</a>
</p>

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS/tree/sdk">ReSharp3DS - SDK</a>
</p>

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS-Studio">ReSharp3DS Studio</a>
</p>

<p align="center">
  <a href="https://discord.gg/ENBwURmUj8">Discord server</a>
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

For the runtime to function correctly, your SD card must be organized as follows:

```
SD:/
├── 3ds/
│   └── ReSharp3DS.3dsx          # Runtime Homebrew
└── ReSharp3DS/                  # Data folder
    ├── mscorlib.pe              # nanoFramework base library
    └── app.pe                   # C# program (user app)
```

> **Note:** The runtime specifically looks for the assemblies in `sdmc:/ReSharp3DS/`.

Current runtime versions use this recommended layout:

```txt
SD:/
├── 3ds/
│   └── ReSharp3DS.3dsx
└── ReSharp3DS/
    ├── bin/
    │   └── mscorlib.pe
    ├── app.pe
    ├── MyApp/
    │   ├── MyApp.pe
    │   └── logo.bmp
    └── AnotherApp/
        └── AnotherApp.pe
```

Current runtime versions use:

```txt
sdmc:/ReSharp3DS/bin/mscorlib.pe
```

The launcher scans:

```txt
sdmc:/ReSharp3DS/
```

and displays folders and `.pe` applications.

---

### What is it for?

ReSharp3DS is meant for experimenting with managed C# code execution on the Nintendo 3DS. It can be used as a base to build 3DS homebrew logic in C#, test nanoCLR on non-standard platforms, and call native C++ code from C#.

**Example:**

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        public static void Main()
        {
            Console.Clear();
            Console.WriteLine("Hello from C# on 3DS!");
            Console.WriteLine("Press START to quit.");

            while (!Input.IsStartPressed())
            {
                Runtime.Yield();
            }

            Console.WriteLine("Bye.");
        }
    }
}
```

---

### Requirements

* **C++ Side:** devkitPro (devkitARM, libctru, make).
* **C# Side:** nanoFramework-compatible compiler (e.g., nanoFramework.CoreLibrary).
* **Hardware:** A Nintendo 3DS with Luma3DS and Homebrew Launcher.

---

### Installation & Build

#### 1. Building the Homebrew (C++)

From the project folder, run:

```bash
make clean
make
```

This generates `ReSharp3DS.3dsx`.

Depending on your build setup, it may also generate:

```txt
ReSharp3DS.cia
ReSharp3DS.elf
```

#### 2. Building the C# application

Compile your C# code into a `.pe` assembly using the nanoFramework toolchain. Rename the output file to `app.pe`.

Current runtime versions can also launch custom `.pe` files from folders inside:

```txt
sdmc:/ReSharp3DS/
```

Example:

```txt
sdmc:/ReSharp3DS/MyApp/MyApp.pe
```

#### 3. Deployment

1. Copy `ReSharp3DS.3dsx` to `SD:/3ds/`.
2. Copy `mscorlib.pe` and your `app.pe` to `SD:/ReSharp3DS/` (create the folder at the root of the SD if it doesn't exist).
3. Launch the app from the **Homebrew Launcher**.

For current runtime versions, the recommended deployment is:

```txt
sdmc:/3ds/ReSharp3DS.3dsx
sdmc:/ReSharp3DS/bin/mscorlib.pe
sdmc:/ReSharp3DS/app.pe
```

You can also place apps in folders:

```txt
sdmc:/ReSharp3DS/MyApp/MyApp.pe
```

---

## Launcher controls

Current runtime versions include a file browser.

```txt
D-Pad Up / Down   Move selection
A                 Open folder or launch selected .pe app
X                 Refresh file list and check updates
START             Exit
```

---

## Update System

Current runtime versions use a server-hosted update system.

The runtime checks:

```txt
https://host.saysaa.fr/3ds/ReSharp3DS/latest/version.json
```

Expected minimal JSON response:

```json
{
  "version": "v2.0.3"
}
```

The runtime compares this version with its internal version.

Example:

```txt
Runtime version: v2.0.3-beta.9
Server version:  v2.0.3
Compared as:     v2.0.3
```

The `-beta.X` suffix is ignored during update comparison.

Update files are hosted here:

```txt
https://host.saysaa.fr/3ds/ReSharp3DS/latest/ReSharp3DS.3dsx
https://host.saysaa.fr/3ds/ReSharp3DS/latest/ReSharp3DS.cia
https://host.saysaa.fr/3ds/ReSharp3DS/latest/ReSharp3DS.elf
```

Runtime dependency:

```txt
https://host.saysaa.fr/3ds/ReSharp3DS/dependencies/mscorlib.pe
```

`latest.txt` is no longer required.

---

## Automatic `mscorlib.pe` restore

If `mscorlib.pe` is missing, current runtime versions try to download it automatically from:

```txt
https://host.saysaa.fr/3ds/ReSharp3DS/dependencies/mscorlib.pe
```

The expected local path is:

```txt
sdmc:/ReSharp3DS/bin/mscorlib.pe
```

If restore fails, copy it manually to that path.

---

## ReSharp3DS API

Current runtime and SDK versions include additional APIs.

Available APIs include:

```txt
Console API
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
Graphics.DrawText(int x, int y, string text, int color);
Graphics.DrawBitmap(string path, int x, int y);
Graphics.Present();
```

`Graphics.DrawBitmap` supports simple BMP files.

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

### Update check does not work

Check that this URL is reachable:

```txt
https://host.saysaa.fr/3ds/ReSharp3DS/latest/version.json
```

The JSON must contain at least:

```json
{
  "version": "v2.0.3"
}
```

### Runtime says no update even with beta builds

This is normal.

The runtime ignores `-beta.X`.

Example:

```txt
v2.0.3-beta.9 = v2.0.3
```

To trigger an update, the server version must be higher, for example:

```txt
v2.0.4
```

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
