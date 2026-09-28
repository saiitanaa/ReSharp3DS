# ReSharp3DS Documentation

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS">GitHub Repository</a>
</p>

---

## Latest Documentation Update

This documentation now covers the current launcher manifest behavior, runtime network helper, SDK helper types, metadata display, and app folder layout.


## Getting Started

**ReSharp3DS** is a Nintendo 3DS homebrew runtime and SDK that allows developers to create simple applications in **C#** using **.NET nanoFramework**.

The runtime loads `.pe` applications from the SD card and exposes native 3DS features such as:

* Console output
* Button input
* Bottom-screen graphics
* Beep / frequency audio
* Custom WAV playback
* Music looping
* Volume control
* File reading and writing
* Directory creation and deletion

---

## SD Card Structure

Recommended current structure:

```txt
sdmc:/3ds/ReSharp3DS.3dsx
sdmc:/ReSharp3DS/bin/mscorlib.pe
sdmc:/ReSharp3DS/apps/TestApp/manifest.json
sdmc:/ReSharp3DS/apps/TestApp/app.pe
sdmc:/ReSharp3DS/apps/TestApp/assets/sprite.bmp
sdmc:/ReSharp3DS/apps/TestApp/assets/sound.wav
```

The launcher starts in:

```txt
sdmc:/ReSharp3DS/
```

It can display folders and `.pe` applications. `mscorlib.pe` is ignored by the launcher.

You can open folders, go back to the parent directory, and launch `.pe` files from subfolders.

## App Manifest

Current launcher builds support an optional `manifest.json` next to a `.pe` file.

Example:

```json
{
  "name": "App de test",
  "author": "Saysaa",
  "version": "0.1.0",
  "entry": "app.pe",
  "description": "Application de test pour ReSharp3DS"
}
```

Expected SD structure:

```txt
sdmc:/ReSharp3DS/apps/TestApp/
  manifest.json
  app.pe
```

The launcher reads the `entry` field and applies the manifest metadata to that `.pe` file.
For example, `app.pe` can be displayed as `App de test`.

When the app is selected, the launcher can also display:

```txt
Author: Saysaa  Version: 0.1.0
```


## Runtime Execution Model

ReSharp3DS uses a **tick-based** runtime model.

`Program.Main()` is called repeatedly by the runtime, so you should use `static` variables to keep state between frames.

Recommended structure:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Hello ReSharp3DS!");
            }

            Runtime.Yield();
        }
    }
}
```

Avoid blocking infinite loops without `Runtime.Yield()`.

This is not recommended:

```csharp
while (true)
{
}
```

This is better:

```csharp
public static void Main()
{
    // update logic here

    Runtime.Yield();
}
```

---

## Available APIs

## Console API

The Console API writes text to the **top screen**.

| Function                     | Description                          |
| ---------------------------- | ------------------------------------ |
| `Console.Clear();`           | Clears the console screen.           |
| `Console.Write("text");`     | Writes text without a newline.       |
| `Console.WriteLine("text");` | Writes text with a newline.          |
| `Console.Write(123);`        | Writes an integer without a newline. |
| `Console.WriteLine(123);`    | Writes an integer with a newline.    |

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Console API Demo");
                Console.Write("Score: ");
                Console.WriteLine(100);
            }

            Runtime.Yield();
        }
    }
}
```

---

## Runtime API

| Function           | Description                                                      |
| ------------------ | ---------------------------------------------------------------- |
| `Runtime.Yield();` | Yields control back to the runtime and waits for the next frame. |

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static int frame = 0;

        public static void Main()
        {
            frame++;

            if ((frame % 60) == 0)
            {
                Console.Write("Frame: ");
                Console.WriteLine(frame);
            }

            Runtime.Yield();
        }
    }
}
```

---

## Input API

The Input API checks if a Nintendo 3DS button is currently pressed.

| Function                  | Description                                  |
| ------------------------- | -------------------------------------------- |
| `Input.IsStartPressed()`  | Checks if the START button is pressed.       |
| `Input.IsSelectPressed()` | Checks if the SELECT button is pressed.      |
| `Input.IsAPressed()`      | Checks if the A button is pressed.           |
| `Input.IsBPressed()`      | Checks if the B button is pressed.           |
| `Input.IsXPressed()`      | Checks if the X button is pressed.           |
| `Input.IsYPressed()`      | Checks if the Y button is pressed.           |
| `Input.IsLPressed()`      | Checks if the L shoulder button is pressed.  |
| `Input.IsRPressed()`      | Checks if the R shoulder button is pressed.  |
| `Input.IsUpPressed()`     | Checks if the D-Pad UP button is pressed.    |
| `Input.IsDownPressed()`   | Checks if the D-Pad DOWN button is pressed.  |
| `Input.IsLeftPressed()`   | Checks if the D-Pad LEFT button is pressed.  |
| `Input.IsRightPressed()`  | Checks if the D-Pad RIGHT button is pressed. |

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static bool oldA = false;
        static bool oldB = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Input API Demo");
                Console.WriteLine("Press A or B");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();

            if (a && !oldA)
            {
                Console.WriteLine("A pressed!");
            }

            if (b && !oldB)
            {
                Console.WriteLine("B pressed!");
            }

            oldA = a;
            oldB = b;

            Runtime.Yield();
        }
    }
}
```

---

## Full Input Example

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static int frame = 0;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;
                Console.Clear();
                Console.WriteLine("Input State Demo");
            }

            frame++;

            if ((frame % 15) == 0)
            {
                Console.Clear();

                Console.WriteLine("Input State Demo");
                Console.WriteLine("----------------");
                Console.WriteLine("");

                if (Input.IsStartPressed())
                {
                    Console.WriteLine("START is pressed");
                }

                if (Input.IsSelectPressed())
                {
                    Console.WriteLine("SELECT is pressed");
                }

                if (Input.IsAPressed())
                {
                    Console.WriteLine("A is pressed");
                }

                if (Input.IsBPressed())
                {
                    Console.WriteLine("B is pressed");
                }

                if (Input.IsXPressed())
                {
                    Console.WriteLine("X is pressed");
                }

                if (Input.IsYPressed())
                {
                    Console.WriteLine("Y is pressed");
                }

                if (Input.IsUpPressed())
                {
                    Console.WriteLine("D-Pad UP is pressed");
                }

                if (Input.IsDownPressed())
                {
                    Console.WriteLine("D-Pad DOWN is pressed");
                }

                if (Input.IsLeftPressed())
                {
                    Console.WriteLine("D-Pad LEFT is pressed");
                }

                if (Input.IsRightPressed())
                {
                    Console.WriteLine("D-Pad RIGHT is pressed");
                }

                if (Input.IsLPressed())
                {
                    Console.WriteLine("L is pressed");
                }

                if (Input.IsRPressed())
                {
                    Console.WriteLine("R is pressed");
                }

                Console.WriteLine("");
                Console.WriteLine("Hold any button to test it.");
            }

            Runtime.Yield();
        }
    }
}
```

---

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

## Graphics Function Details

### `Graphics.Clear`

```csharp
Graphics.Clear(0x000000);
```

Clears the bottom screen.

### `Graphics.DrawPixel`

```csharp
Graphics.DrawPixel(100, 100, 0xFFFFFF);
```

Draws a single pixel.

### `Graphics.FillRect`

```csharp
Graphics.FillRect(20, 40, 80, 30, 0xFF0000);
```

Draws a filled rectangle.

### `Graphics.DrawRect`

```csharp
Graphics.DrawRect(20, 40, 80, 30, 0xFFFFFF);
```

Draws only the border of a rectangle.

### `Graphics.DrawText`

```csharp
Graphics.DrawText(10, 10, "Hello", 0xFFFFFF);
```

Draws text using the runtime bitmap font.

### `Graphics.Present`

```csharp
Graphics.Present();
```

Flushes and displays the frame.

---

## Audio API

The Audio API supports:

* Beep / generated tones
* Custom WAV playback
* Background music looping
* Volume control
* Music stop

| Function                             | Description                                 |
| ------------------------------------ | ------------------------------------------- |
| `Audio.Init();`                      | Initializes the audio system.               |
| `Audio.Beep(frequency, durationMs);` | Plays a generated tone.                     |
| `Audio.PlayWav(path);`               | Plays a custom WAV file once.               |
| `Audio.SetVolume(volume);`           | Sets global audio volume from `0` to `100`. |
| `Audio.Loop(path);`                  | Plays a WAV file in loop mode as music.     |
| `Audio.StopMusic();`                 | Stops the looping music channel.            |
| `Audio.Stop();`                      | Stops audio playback.                       |

## IMPORTANT

If you decide to add audio to your homebrew, be sure to dump `dspfirm.cdc`.

### How to do it?

* Open the Rosalina menu: `L + D-Pad Down + Select`
* Go to `Miscellaneous Options`
* Select `Dump DSP Firmware`

<img width="400" height="300" alt="IMG_0749" src="https://github.com/user-attachments/assets/d3555f6a-cb75-4c66-89fd-0f41428fb6c3" />
<img width="400" height="300" alt="IMG_0750" src="https://github.com/user-attachments/assets/d13a533d-1db2-4ec3-b5d0-cf20c2270b25" />

---

## Audio.Init

```csharp
Audio.Init();
```

Initializes the audio system.

Recommended usage:

```csharp
if (!initialized)
{
    initialized = true;
    Audio.Init();
}
```

---

## Audio.Beep

```csharp
Audio.Beep(440, 200);
```

Plays a generated tone.

Arguments:

| Argument     | Description                               |
| ------------ | ----------------------------------------- |
| `frequency`  | Frequency in Hz. Example: `440`.          |
| `durationMs` | Duration in milliseconds. Example: `200`. |

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static bool oldA = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Press A for beep");

                Audio.Init();
            }

            bool a = Input.IsAPressed();

            if (a && !oldA)
            {
                Audio.Beep(440, 200);
            }

            oldA = a;

            Runtime.Yield();
        }
    }
}
```

---

## Notes API

The SDK can expose note frequency constants.

| Constant   | Frequency |
| ---------- | --------- |
| `Notes.C4` | `262`     |
| `Notes.D4` | `294`     |
| `Notes.E4` | `330`     |
| `Notes.F4` | `349`     |
| `Notes.G4` | `392`     |
| `Notes.A4` | `440`     |
| `Notes.B4` | `494`     |
| `Notes.C5` | `523`     |
| `Notes.D5` | `587`     |
| `Notes.E5` | `659`     |
| `Notes.F5` | `698`     |
| `Notes.G5` | `784`     |
| `Notes.A5` | `880`     |
| `Notes.B5` | `988`     |

Example:

```csharp
Audio.Beep(Notes.A4, 200);
Audio.Beep(Notes.C5, 200);
```

---

## Audio.PlayWav

```csharp
Audio.PlayWav("audio.wav");
```

Plays a WAV file once.

**Recommended WAV format:**

```txt
PCM 16-bit
Mono or stereo
44100 Hz or 22050 Hz
```

Relative path example:

```csharp
Audio.PlayWav("audio.wav");
```

If your app is here:

```txt
sdmc:/ReSharp3DS/my_app.pe
```

then ReSharp3DS will look for:

```txt
sdmc:/ReSharp3DS/audio.wav
```

Subfolder example:

```csharp
Audio.PlayWav("sfx/jump.wav");
```

Absolute path example:

```csharp
Audio.PlayWav("sdmc:/MyGame/sfx/jump.wav");
```

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static bool oldA = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("WAV Demo");
                Console.WriteLine("Press A");

                Audio.Init();
            }

            bool a = Input.IsAPressed();

            if (a && !oldA)
            {
                Console.WriteLine("Playing audio.wav");
                Audio.PlayWav("audio.wav");
            }

            oldA = a;

            Runtime.Yield();
        }
    }
}
```

Expected SD structure:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe
  wav_demo.pe
  audio.wav
```

---

## Audio.SetVolume

```csharp
Audio.SetVolume(80);
```

Sets the audio volume.

Range:

```txt
0   silent
100 full volume
```

Example:

```csharp
Audio.SetVolume(100);
Audio.SetVolume(80);
Audio.SetVolume(40);
Audio.SetVolume(0);
```

Volume control example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static bool oldUp = false;
        static bool oldDown = false;
        static int volume = 80;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Volume Demo");
                Console.WriteLine("UP/DOWN = volume");

                Audio.Init();
                Audio.SetVolume(volume);
                Audio.Loop("music.wav");
            }

            bool up = Input.IsUpPressed();
            bool down = Input.IsDownPressed();

            if (up && !oldUp)
            {
                volume += 10;

                if (volume > 100)
                {
                    volume = 100;
                }

                Audio.SetVolume(volume);

                Console.Write("Volume: ");
                Console.WriteLine(volume);
            }

            if (down && !oldDown)
            {
                volume -= 10;

                if (volume < 0)
                {
                    volume = 0;
                }

                Audio.SetVolume(volume);

                Console.Write("Volume: ");
                Console.WriteLine(volume);
            }

            oldUp = up;
            oldDown = down;

            Runtime.Yield();
        }
    }
}
```

---

## Audio.Loop

```csharp
Audio.Loop("music.wav");
```

Loops a WAV file as background music.

Recommended WAV format:

```txt
PCM 16-bit
Mono or stereo
44100 Hz or 22050 Hz
```

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Music Loop Demo");

                Audio.Init();
                Audio.SetVolume(80);
                Audio.Loop("music.wav");
            }

            Runtime.Yield();
        }
    }
}
```

Expected SD structure:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe
  music_demo.pe
  music.wav
```

---

## Audio.StopMusic

```csharp
Audio.StopMusic();
```

Stops the looping music channel.

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static bool oldY = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Y = stop music");

                Audio.Init();
                Audio.Loop("music.wav");
            }

            bool y = Input.IsYPressed();

            if (y && !oldY)
            {
                Console.WriteLine("Music stopped");
                Audio.StopMusic();
            }

            oldY = y;

            Runtime.Yield();
        }
    }
}
```

---

## Audio.Stop

```csharp
Audio.Stop();
```

Stops audio playback.

Example:

```csharp
if (Input.IsBPressed())
{
    Audio.Stop();
}
```

---

## Full Audio Example

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static bool oldA = false;
        static bool oldB = false;
        static bool oldX = false;
        static bool oldY = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Audio Demo");
                Console.WriteLine("A = play WAV");
                Console.WriteLine("B = beep");
                Console.WriteLine("X = lower volume");
                Console.WriteLine("Y = stop music");

                Audio.Init();
                Audio.SetVolume(80);
                Audio.Loop("music.wav");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool x = Input.IsXPressed();
            bool y = Input.IsYPressed();

            if (a && !oldA)
            {
                Audio.PlayWav("audio.wav");
            }

            if (b && !oldB)
            {
                Audio.Beep(Notes.A4, 150);
            }

            if (x && !oldX)
            {
                Audio.SetVolume(40);
            }

            if (y && !oldY)
            {
                Audio.StopMusic();
            }

            oldA = a;
            oldB = b;
            oldX = x;
            oldY = y;

            Runtime.Yield();
        }
    }
}
```

---

## File and Directory API

The File and Directory APIs allow applications to create save files, read configuration files, delete files, and manage folders.

Paths can be relative to the launched `.pe` file or absolute.

Example:

```csharp
File.WriteAllText("save.txt", "Hello!");
```

If your app is here:

```txt
sdmc:/ReSharp3DS/games/my_game.pe
```

then ReSharp3DS will write:

```txt
sdmc:/ReSharp3DS/games/save.txt
```

Subfolder example:

```csharp
Directory.Create("saves");
File.WriteAllText("saves/save.txt", "PlayerLevel=5");
```

Absolute path example:

```csharp
File.WriteAllText("sdmc:/ReSharp3DS/global_save.txt", "Global data");
```

---

## File API

| Function                         | Description                                          |
| -------------------------------- | ---------------------------------------------------- |
| `File.Exists(path);`             | Checks if a file exists.                             |
| `File.WriteAllText(path, text);` | Writes text to a file. Creates or replaces the file. |
| `File.ReadAllText(path);`        | Reads a text file and returns its content.           |
| `File.Delete(path);`             | Deletes a file.                                      |

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static bool oldA = false;
        static bool oldB = false;
        static bool oldX = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("File API Demo");
                Console.WriteLine("A = write save");
                Console.WriteLine("B = read save");
                Console.WriteLine("X = delete save");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool x = Input.IsXPressed();

            if (a && !oldA)
            {
                File.WriteAllText("save.txt", "Hello from ReSharp3DS!");
                Console.WriteLine("save.txt written");
            }

            if (b && !oldB)
            {
                if (File.Exists("save.txt"))
                {
                    string text = File.ReadAllText("save.txt");

                    Console.Write("Loaded: ");
                    Console.WriteLine(text);
                }
                else
                {
                    Console.WriteLine("save.txt not found");
                }
            }

            if (x && !oldX)
            {
                File.Delete("save.txt");
                Console.WriteLine("save.txt deleted");
            }

            oldA = a;
            oldB = b;
            oldX = x;

            Runtime.Yield();
        }
    }
}
```

Expected SD structure:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe
  file_demo.pe
```

After pressing A:

```txt
sdmc:/ReSharp3DS/
  save.txt
```

---

## Directory API

| Function                  | Description                   |
| ------------------------- | ----------------------------- |
| `Directory.Exists(path);` | Checks if a directory exists. |
| `Directory.Create(path);` | Creates a directory.          |
| `Directory.Delete(path);` | Deletes an empty directory.   |

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static bool oldA = false;
        static bool oldB = false;
        static bool oldX = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Directory API Demo");
                Console.WriteLine("A = create saves folder");
                Console.WriteLine("B = write save file");
                Console.WriteLine("X = delete saves folder");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool x = Input.IsXPressed();

            if (a && !oldA)
            {
                Directory.Create("saves");
                Console.WriteLine("saves folder created");
            }

            if (b && !oldB)
            {
                if (!Directory.Exists("saves"))
                {
                    Directory.Create("saves");
                }

                File.WriteAllText("saves/save.txt", "Save data");
                Console.WriteLine("save created in saves folder");
            }

            if (x && !oldX)
            {
                File.Delete("saves/save.txt");
                Directory.Delete("saves");
                Console.WriteLine("saves folder deleted");
            }

            oldA = a;
            oldB = b;
            oldX = x;

            Runtime.Yield();
        }
    }
}
```

Expected SD structure after creating a save:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe
  directory_demo.pe
  saves/
    save.txt
```

`Directory.Delete(path)` only works if the directory is empty.

---

## Full File System Example

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static bool oldA = false;
        static bool oldB = false;
        static bool oldX = false;
        static bool oldY = false;

        static int counter = 0;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("File System Demo");
                Console.WriteLine("A = save counter");
                Console.WriteLine("B = load counter");
                Console.WriteLine("X = delete save");
                Console.WriteLine("Y = create folder");

                if (!Directory.Exists("saves"))
                {
                    Console.WriteLine("saves folder missing");
                }
                else
                {
                    Console.WriteLine("saves folder found");
                }
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool x = Input.IsXPressed();
            bool y = Input.IsYPressed();

            if (a && !oldA)
            {
                counter++;

                Directory.Create("saves");
                File.WriteAllText("saves/counter.txt", "Counter saved!");

                Console.Write("Saved counter: ");
                Console.WriteLine(counter);
            }

            if (b && !oldB)
            {
                if (File.Exists("saves/counter.txt"))
                {
                    string text = File.ReadAllText("saves/counter.txt");

                    Console.Write("Loaded: ");
                    Console.WriteLine(text);
                }
                else
                {
                    Console.WriteLine("No save found");
                }
            }

            if (x && !oldX)
            {
                File.Delete("saves/counter.txt");
                Console.WriteLine("Save deleted");
            }

            if (y && !oldY)
            {
                Directory.Create("saves");
                Console.WriteLine("Folder created");
            }

            oldA = a;
            oldB = b;
            oldX = x;
            oldY = y;

            Runtime.Yield();
        }
    }
}
```

---

## Full API Demo

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static int x = 0;
        static int volume = 80;

        static bool oldA = false;
        static bool oldB = false;
        static bool oldX = false;
        static bool oldY = false;
        static bool oldUp = false;
        static bool oldDown = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("ReSharp3DS Full API Demo");
                Console.WriteLine("A = WAV");
                Console.WriteLine("B = Beep");
                Console.WriteLine("X = Stop music");
                Console.WriteLine("Y = Start music");
                Console.WriteLine("UP/DOWN = Volume");

                Audio.Init();
                Audio.SetVolume(volume);
                Audio.Loop("music.wav");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool xButton = Input.IsXPressed();
            bool yButton = Input.IsYPressed();
            bool up = Input.IsUpPressed();
            bool down = Input.IsDownPressed();

            if (a && !oldA)
            {
                Console.WriteLine("Play audio.wav");
                Audio.PlayWav("audio.wav");
            }

            if (b && !oldB)
            {
                Console.WriteLine("Beep");
                Audio.Beep(Notes.A4, 150);
            }

            if (xButton && !oldX)
            {
                Console.WriteLine("Stop music");
                Audio.StopMusic();
            }

            if (yButton && !oldY)
            {
                Console.WriteLine("Loop music.wav");
                Audio.Loop("music.wav");
            }

            if (up && !oldUp)
            {
                volume += 10;

                if (volume > 100)
                {
                    volume = 100;
                }

                Audio.SetVolume(volume);
                Console.Write("Volume: ");
                Console.WriteLine(volume);
            }

            if (down && !oldDown)
            {
                volume -= 10;

                if (volume < 0)
                {
                    volume = 0;
                }

                Audio.SetVolume(volume);
                Console.Write("Volume: ");
                Console.WriteLine(volume);
            }

            Graphics.Clear(0x000000);
            Graphics.DrawText(10, 10, "ReSharp3DS", 0xFFFFFF);
            Graphics.DrawText(10, 30, "Bottom screen graphics", 0x00FF00);
            Graphics.FillRect(x, 90, 32, 32, 0xFF0000);
            Graphics.DrawRect(5, 5, 180, 45, 0xFFFFFF);
            Graphics.Present();

            x++;

            if (x > 320)
            {
                x = 0;
            }

            oldA = a;
            oldB = b;
            oldX = xButton;
            oldY = yButton;
            oldUp = up;
            oldDown = down;

            Runtime.Yield();
        }
    }
}
```

Expected SD structure:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe
  full_demo.pe
  audio.wav
  music.wav
```

---

## SDK Helper Types

Current SDK builds also include helper types for cleaner app code:

```csharp
Button.A
Button.B
Button.Start
ScreenTarget.Top
ScreenTarget.Bottom
Vector2 position
Rect bounds
Timer timer
```

Common helpers:

```csharp
if (Input.IsHeld(Button.Left)) { }
if (Input.IsPressed(Button.A)) { }

Vector2 touch = Touch.Position();
Graphics.SetTarget(ScreenTarget.Bottom);
Runtime.ReturnToLauncher();
```


## Dynamic Native Mapping

ReSharp3DS uses dynamic native mapping.

This means the runtime maps native methods by name instead of fixed method indexes.

Developers can safely add helper methods:

```csharp
static void Update()
{
}

static void Draw()
{
}

static void PlaySound()
{
}
```

This no longer breaks native calls such as:

```csharp
Runtime.Yield();
Audio.PlayWav("audio.wav");
Input.IsAPressed();
```

---

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

## Audio Requirements

On real Nintendo 3DS hardware, NDSP audio requires:

```txt
sdmc:/3ds/dspfirm.cdc
```

You can generate it from Rosalina:

```txt
L + D-Pad Down + Select
Miscellaneous options
Dump DSP firmware
```

On Citra, make sure the audio output is correctly configured.

---

## Known Limitations

* WAV support is currently focused on PCM 16-bit files.
* Very large WAV files may use too much memory.
* `File.ReadAllText()` is intended for small text files.
* `Directory.Delete()` only works on empty folders.
* Audio requires DSP setup on real 3DS hardware.
* Graphics are currently simple framebuffer-style drawing.
* The SDK is still experimental and APIs may change before a stable release.

---

## Minimal Hello World

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Hello World!");
                Console.WriteLine("Running on ReSharp3DS");
            }

            Runtime.Yield();
        }
    }
}
```


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

