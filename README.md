# ReSharp3DS
ReSharp3DS is an experimental project that allows C# code to run on the Nintendo 3DS using nanoCLR / nanoFramework.

The goal is to provide a base for developing 3DS homebrew applications with C# logic, while keeping a native C++ layer for accessing console-specific features such as display output, buttons, file system access, and libctru functions.

The project currently loads:

sdmc:/mscorlib.pe
sdmc:/app.pe

It then initializes the nanoCLR runtime, resolves the assemblies, prepares execution, and runs the C# entry point Program.Main().

What is it for?

ReSharp3DS is meant for experimenting with managed C# code execution on the Nintendo 3DS.

It can be used as a base to:

create 3DS homebrew logic in C#;
test nanoCLR on a non-standard platform;
build a simple C# API for the 3DS;
call native C++ code from C#;
learn how an embedded managed runtime works.

The long-term goal is to make C# development feel close to a classic console-style C# application.

Example:

public class Program
{
    public static void Main()
    {
        Native3DS.Print();
    }
}

or, with a future higher-level API:

public class Program
{
    public static void Main()
    {
        Console3DS.WriteLine("Hello from C# on 3DS");

        while (true)
        {
            Runtime3DS.Yield();
        }
    }
}
Current project status

The project is still experimental.

Currently working:

nanoCLR runtime initialization on 3DS;
loading mscorlib.pe from the SD card;
loading app.pe from the SD card;
assembly resolution;
execution of Program.Main();
native C# to C++ calls through InternalCall;
text output from native C++ to the 3DS screen.

Still being worked on:

a proper high-level C# API;
Console.WriteLine(string) or equivalent;
passing parameters from C# to C++;
proper input handling;
libctru bindings;
automatic .pe generation;
a clean reusable SDK structure.
Requirements

To build the 3DS homebrew project, you need:

devkitPro;
devkitARM;
libctru;
make;
a Nintendo 3DS capable of running homebrew;
Luma3DS / Homebrew Launcher or an equivalent setup.

On Windows, devkitPro usually installs the required tools under:

C:\devkitPro

or:

/opt/devkitpro

depending on your environment.

Expected SD card structure

The following files must be placed at the root of the SD card:

SD:/
├── mscorlib.pe
└── app.pe

mscorlib.pe contains the nanoFramework base library.

app.pe contains your compiled C# program.

The homebrew loads them using:

sdmc:/mscorlib.pe
sdmc:/app.pe

If app.pe is missing, the program will print an error such as:

[ERR] fopen sdmc:/app.pe
[FATAL] app load failed
C# example

Minimal example:

using System.Runtime.CompilerServices;

public class Program
{
    public static void Main()
    {
        Native3DS.Print();
    }
}

public static class Native3DS
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Print();
}

This calls a native C++ method.

On the 3DS runtime side, that native method can print something like:

Hello from C# -> C++ on 3DS!
Building the 3DS homebrew

From the project folder, run:

make clean
make

If the build succeeds, a .3dsx file should be generated.

Copy the homebrew to the proper SD card folder, for example:

SD:/3ds/ReSharp3DS/ReSharp3DS.3dsx

Then copy the required assemblies to the SD card root:

SD:/mscorlib.pe
SD:/app.pe
Building the C# application

The C# code must be compiled into a nanoFramework-compatible .pe assembly.

The project has been tested with nanoFramework.CoreLibrary.

After building the C# project, take the generated .pe file and copy or rename it as:

app.pe

Then place it at the root of the SD card:

SD:/app.pe

On Windows, it is recommended to enable file extension display to avoid accidentally naming the file:

app.pe.pe

instead of:

app.pe
Running on the 3DS
Copy the .3dsx file to the homebrew folder on the SD card.
Copy mscorlib.pe to the root of the SD card.
Copy app.pe to the root of the SD card.
Launch the homebrew from the Homebrew Launcher.
The runtime initializes nanoCLR, loads the assemblies, and runs Program.Main().

Expected output for a successful native call test:

[LOAD] sdmc:/mscorlib.pe
[OK] linked sdmc:/mscorlib.pe
[LOAD] sdmc:/app.pe
[OK] linked sdmc:/app.pe
[ResolveAll] 0x00000000
[Prepare] 0x00000000
[CLR] Execute
[NATIVE] Native3DS.Print called
Hello from C# -> C++ on 3DS!
[CLR] returned 0x00000000
Important notes

This project is experimental.

It is not yet a complete SDK for writing full 3DS homebrew applications entirely in C# without limitations.

C# logic can currently run, but access to 3DS-specific functionality must go through native C++ methods.

Full support for features such as:

Console.WriteLine("text");
Input.IsKeyDown(...);
File.ReadAllText(...);

still requires dedicated C# to C++ bindings.

Long-term goal

The final goal is to provide a simple development layer that allows 3DS homebrew applications to be written in C# with a clean API, for example:

public class Program
{
    public static void Main()
    {
        Console3DS.WriteLine("Hello 3DS");

        while (!Input3DS.IsStartPressed())
        {
            Runtime3DS.Yield();
        }
    }
}
Disclaimer

This project is not affiliated with Nintendo, Microsoft, or the .NET Foundation.

It is an experimental homebrew and runtime porting project intended for learning, research, and development purposes.
