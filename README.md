# ReSharp3DS Documentation

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS">GitHub Repository</a>
</p>

---

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

Basic structure:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe
  my_app.pe
```

Example with folders, audio files, and save files:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe

  demos/
    audio_demo.pe
    audio.wav
    music.wav

  games/
    my_game.pe
    music.wav
    sfx/
      jump.wav
      select.wav
    saves/
      save.txt
```

The launcher automatically starts in:

```txt
sdmc:/ReSharp3DS/
```

The launcher can display folders and `.pe` applications.

`mscorlib.pe` is ignored by the launcher.

You can open folders, go back to the parent directory, and launch `.pe` files from subfolders.

---

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

The Graphics API draws on the **bottom screen**.

Colors are RGB integers:

```csharp
0x000000 // black
0xFFFFFF // white
0xFF0000 // red
0x00FF00 // green
0x0000FF // blue
```

| Function                                         | Description                            |
| ------------------------------------------------ | -------------------------------------- |
| `Graphics.Clear(color);`                         | Clears the bottom screen with a color. |
| `Graphics.DrawPixel(x, y, color);`               | Draws one pixel.                       |
| `Graphics.FillRect(x, y, width, height, color);` | Draws a filled rectangle.              |
| `Graphics.DrawRect(x, y, width, height, color);` | Draws a rectangle outline.             |
| `Graphics.DrawText(x, y, "text", color);`        | Draws text on the bottom screen.       |
| `Graphics.Present();`                            | Presents the current frame.            |

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static int x = 0;

        public static void Main()
        {
            Graphics.Clear(0x000000);

            Graphics.DrawText(10, 10, "Graphics Demo", 0xFFFFFF);
            Graphics.FillRect(x, 80, 40, 40, 0xFF0000);
            Graphics.DrawRect(5, 5, 150, 30, 0x00FF00);
            Graphics.DrawPixel(200, 120, 0xFFFFFF);

            Graphics.Present();

            x++;

            if (x > 320)
            {
                x = 0;
            }

            Runtime.Yield();
        }
    }
}
```

---

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

## Launcher Controls

| Button       | Action                                         |
| ------------ | ---------------------------------------------- |
| `D-Pad Up`   | Move selection up.                             |
| `D-Pad Down` | Move selection down.                           |
| `A`          | Open selected folder or launch selected `.pe`. |
| `X`          | Refresh current folder.                        |
| `START`      | Exit launcher.                                 |

The launcher supports folders.

Example:

```txt
sdmc:/ReSharp3DS/
  mscorlib.pe

  demos/
    audio_demo.pe

  games/
    game.pe
```

You can enter `demos/` or `games/` from the launcher and start `.pe` files inside them.

---

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
