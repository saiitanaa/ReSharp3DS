# >_ ReSharp3DS
ReSharp3DS is an experimental project that runs C# code on the Nintendo 3DS using nanoCLR / nanoFramework.

The project uses a C++ 3DS homebrew application to load C# assemblies compiled as `.pe` files, then executes them through nanoCLR.

The goal is to eventually allow 3DS homebrew development with C# logic, while keeping a native C++ layer for console-specific features.

The runtime currently loads these files from the SD card:

```txt
sdmc:/mscorlib.pe
sdmc:/app.pe
```

`mscorlib.pe` contains the nanoFramework base library.

`app.pe` contains the user C# program.

---

### What is it for?

ReSharp3DS is meant for experimenting with managed C# code execution on the Nintendo 3DS.

It can be used as a base to:

* run managed C# code on the 3DS;
* create 3DS homebrew logic in C#;
* test nanoCLR on a non-standard platform;
* build a simple C# API for the Nintendo 3DS;
* call native C++ code from C#;
* learn how an embedded managed runtime works.

The long-term goal is to make development feel close to a classic console-style C# application.

Example:

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        public static void Main()
        {
            Console.Clear();

            Console.Write("Hello ");
            Console.WriteLine("from C# on 3DS !");
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

To build the 3DS homebrew project, you need:

* devkitPro;
* devkitARM;
* libctru;
* make;
* a Nintendo 3DS capable of running homebrew;
* Luma3DS, Homebrew Launcher or an equivalent setup.

On Windows, devkitPro is usually installed under:

```txt
C:\devkitPro
```

On Linux or some MSYS2-based setups, it may be available under:

```txt
/opt/devkitpro
```

---

### SD card structure

The following files must be placed at the root of the SD card:

```txt
SD:/
├── mscorlib.pe
└── app.pe
```

The homebrew loads them using these paths:

```txt
sdmc:/mscorlib.pe
sdmc:/app.pe
```

If `app.pe` is missing or placed incorrectly, the program will print an error like:

```txt
[ERR] fopen sdmc:/app.pe
[FATAL] app load failed
```

---

### Building the 3DS homebrew

From the project folder, run:

```bat
make clean
make
```

If the build succeeds, a `.3dsx` file will be generated.

Then copy the homebrew to the SD card, for example:

```txt
SD:/3ds/ReSharp3DS/ReSharp3DS.3dsx
```

Also copy the required assemblies to the SD card root:

```txt
SD:/mscorlib.pe
SD:/app.pe
```

---

### Building the C# application

The C# code must be compiled into a nanoFramework-compatible `.pe` assembly.

The project has been tested with nanoFramework.CoreLibrary.

After building the C# project, take the generated `.pe` file and copy or rename it as:

```txt
app.pe
```

Then place it at the root of the SD card:

```txt
SD:/app.pe
```

---

### Running on the 3DS

1. Copy the `.3dsx` file to the homebrew folder on the SD card.
2. Copy `mscorlib.pe` to the root of the SD card.
3. Copy `app.pe` to the root of the SD card.
4. Launch the homebrew from the Homebrew Launcher.
5. The runtime initializes nanoCLR, loads the assemblies, and executes `Program.Main()`.

---

## Disclaimer

This project is not affiliated with Nintendo, Microsoft, the .NET Foundation, or the nanoFramework project.

It is an experimental homebrew and runtime porting project intended for learning, research, and development purposes.

## Assets 
<img width="200" height="200" alt="84c616ab-7373-4a75-8c1e-87f9ba1a9c54_removalai_preview" src="https://github.com/user-attachments/assets/b9d68a0d-2456-4bcb-a413-a0e7eee89aec" />

