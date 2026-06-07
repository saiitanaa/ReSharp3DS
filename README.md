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

## Launcher Controls

| Button       | Action                                              |
| ------------ | --------------------------------------------------- |
| `D-Pad Up`   | Move selection up.                                  |
| `D-Pad Down` | Move selection down.                                |
| `A`          | Open selected folder or launch selected `.pe` file. |
| `X`          | Refresh current folder.                             |
| `START`      | Exit launcher.                                      |

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

## Known Limitations

* WAV support is currently focused on PCM 16-bit files.
* Very large audio files may use too much memory.
* `File.ReadAllText()` is intended for small text files.
* `Directory.Delete()` only works on empty folders.
* Audio requires DSP setup on real 3DS hardware.
* Graphics are currently simple framebuffer-style drawing.
* The SDK is still experimental and APIs may change before a stable release.

