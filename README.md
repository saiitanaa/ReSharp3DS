![ReSharp3DS - SDK](https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ReSharp3DS%20-%20SDK)

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS/tree/docs">ReSharp3DS Documentation</a>
</p>

---

## ReSharp3DS SDK is the managed C# SDK used to create applications for the ReSharp3DS runtime.

This branch contains the C# API layer, examples and project setup files required to build an `app.pe` file that can be loaded by the ReSharp3DS Nintendo 3DS homebrew runtime.

This branch is only for writing C# applications.

The native 3DS homebrew runtime is maintained separately in the main homebrew branch.

## What is this branch?

This branch contains the ReSharp3DS C# SDK.

It is used to create C# applications that run through the ReSharp3DS runtime on Nintendo 3DS.

The SDK provides a small managed API for:

```csharp
Console.Clear();

Console.Write("text");
Console.WriteLine("text");

Console.Write(123);
Console.WriteLine(123);

Input.IsAPressed();
Input.IsBPressed();
Input.IsXPressed();
Input.IsYPressed();

Input.IsLPressed();
Input.IsRPressed();

Input.IsUpPressed();
Input.IsDownPressed();
Input.IsLeftPressed();
Input.IsRightPressed();

Input.IsStartPressed();
Input.IsSelectPressed();

Runtime.Yield();
```

Graphics support is experimental and may change.

## Repository branches

ReSharp3DS is split into multiple branches:

```txt
main / homebrew branch
```

Contains the native Nintendo 3DS homebrew runtime.

This is the part that loads:

```txt
sdmc:/ReSharp3DS/mscorlib.pe
sdmc:/ReSharp3DS/app.pe
```

and runs the C# application.

```txt
sdk branch
```

Contains the C# SDK files, examples and project setup.

This is the part used by developers to write their C# application and build `app.pe`.

## Requirements

To create a ReSharp3DS C# project, you need:

* Windows
* Visual Studio
* .NET nanoFramework Extension for Visual Studio
* nanoFramework CoreLibrary compatible with ReSharp3DS
* ReSharp3DS SDK files
* The native ReSharp3DS homebrew runtime from the main branch

Recommended nanoFramework package version:

```txt
nanoFramework.CoreLibrary 1.9.0
```

If your current runtime was built against a more specific package, such as:

```txt
nanoFramework.CoreLibrary 1.9.0-preview.11
```

use that exact version instead.

The nanoFramework version used by the C# app must match the `mscorlib.pe` used by the 3DS runtime.

## Installing the nanoFramework extension

Open Visual Studio.

Go to:

```txt
Extensions > Manage Extensions
```

Search for:

```txt
nanoFramework
```

Install:

```txt
.NET nanoFramework Extension
```

Restart Visual Studio after installation.

After restarting Visual Studio, you should be able to create and build nanoFramework projects.

## Creating a ReSharp3DS project

Create a new nanoFramework C# application in Visual Studio.

The final output must be:

```txt
app.pe
```

This file is copied to the SD card and loaded by the ReSharp3DS runtime.

Expected SD card layout:

```txt
sdmc:/ReSharp3DS/app.pe
sdmc:/ReSharp3DS/mscorlib.pe
```

The native runtime must already exist on the SD card as a `.3dsx` homebrew application.

## Project configuration

Your C# project should reference the nanoFramework CoreLibrary version used by ReSharp3DS.

Example `.csproj` structure:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="Current" DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup Label="Globals">
    <NanoFrameworkProjectSystemPath>$(MSBuildExtensionsPath)\nanoFramework\v1.0\</NanoFrameworkProjectSystemPath>
  </PropertyGroup>

  <Import Project="$(NanoFrameworkProjectSystemPath)NFProjectSystem.Default.props" Condition="Exists('$(NanoFrameworkProjectSystemPath)NFProjectSystem.Default.props')" />

  <PropertyGroup>
    <Configuration Condition=" '$(Configuration)' == '' ">Debug</Configuration>
    <Platform Condition=" '$(Platform)' == '' ">AnyCPU</Platform>
    <ProjectTypeGuids>{11A8DD76-328B-46DF-9F39-F559912D0360};{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}</ProjectTypeGuids>
    <OutputType>Exe</OutputType>
    <AppDesignerFolder>Properties</AppDesignerFolder>
    <FileAlignment>512</FileAlignment>
    <RootNamespace>app</RootNamespace>
    <AssemblyName>app</AssemblyName>
    <TargetFrameworkVersion>v1.0</TargetFrameworkVersion>
  </PropertyGroup>

  <Import Project="$(NanoFrameworkProjectSystemPath)NFProjectSystem.props" Condition="Exists('$(NanoFrameworkProjectSystemPath)NFProjectSystem.props')" />

  <ItemGroup>
    <Compile Include="ReSharp3DS.cs" />
    <Compile Include="Program.cs" />
    <Compile Include="Properties\AssemblyInfo.cs" />
  </ItemGroup>

  <ItemGroup>
    <Reference Include="mscorlib">
      <HintPath>..\packages\nanoFramework.CoreLibrary.1.9.0-preview.11\lib\mscorlib.dll</HintPath>
    </Reference>
  </ItemGroup>

  <ItemGroup>
    <None Include="packages.config" />
  </ItemGroup>

  <Import Project="$(NanoFrameworkProjectSystemPath)NFProjectSystem.CSharp.targets" Condition="Exists('$(NanoFrameworkProjectSystemPath)NFProjectSystem.CSharp.targets')" />
</Project>
```

Important:

```txt
ReSharp3DS.cs must be compiled before Program.cs.
```

The current runtime uses native method indexes. Changing the method order inside `Native3DS` can break native calls.

## Required SDK file

Every ReSharp3DS app needs the SDK API file:

```txt
ReSharp3DS.cs
```

This file exposes the public API used by developers.

It also contains the internal native methods connected to the C++ runtime through `InternalCall`.

Example structure:

```csharp
using System.Runtime.CompilerServices;

namespace ReSharp3DS
{
    internal static class Native3DS
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Clear();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void WriteLine(string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void WriteInt(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void WriteLineInt(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsStartPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsSelectPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsAPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsBPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsXPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsYPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsLPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsRPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsUpPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsDownPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsLeftPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsRightPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Yield();
    }

    public static class Console
    {
        public static void Clear()
        {
            Native3DS.Clear();
        }

        public static void Write(string text)
        {
            Native3DS.Write(text);
        }

        public static void WriteLine(string text)
        {
            Native3DS.WriteLine(text);
        }

        public static void Write(int value)
        {
            Native3DS.WriteInt(value);
        }

        public static void WriteLine(int value)
        {
            Native3DS.WriteLineInt(value);
        }
    }

    public static class Input
    {
        public static bool IsStartPressed()
        {
            return Native3DS.IsStartPressed() != 0;
        }

        public static bool IsSelectPressed()
        {
            return Native3DS.IsSelectPressed() != 0;
        }

        public static bool IsAPressed()
        {
            return Native3DS.IsAPressed() != 0;
        }

        public static bool IsBPressed()
        {
            return Native3DS.IsBPressed() != 0;
        }

        public static bool IsXPressed()
        {
            return Native3DS.IsXPressed() != 0;
        }

        public static bool IsYPressed()
        {
            return Native3DS.IsYPressed() != 0;
        }

        public static bool IsLPressed()
        {
            return Native3DS.IsLPressed() != 0;
        }

        public static bool IsRPressed()
        {
            return Native3DS.IsRPressed() != 0;
        }

        public static bool IsUpPressed()
        {
            return Native3DS.IsUpPressed() != 0;
        }

        public static bool IsDownPressed()
        {
            return Native3DS.IsDownPressed() != 0;
        }

        public static bool IsLeftPressed()
        {
            return Native3DS.IsLeftPressed() != 0;
        }

        public static bool IsRightPressed()
        {
            return Native3DS.IsRightPressed() != 0;
        }
    }

    public static class Runtime
    {
        public static void Yield()
        {
            Native3DS.Yield();
        }
    }
}
```

## Writing your first app

Create a `Program.cs` file:

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
                Console.WriteLine("Hello from ReSharp3DS SDK!");
                Console.WriteLine("Press A or START.");
            }

            if (Input.IsAPressed())
            {
                Console.WriteLine("A pressed");
            }

            if (Input.IsStartPressed())
            {
                Console.WriteLine("START pressed");
            }

            Runtime.Yield();
        }
    }
}
```

## Runtime model

ReSharp3DS currently uses a tick-based model.

That means `Program.Main()` is called repeatedly by the native runtime.

Use `static` fields to keep state between ticks.

Recommended pattern:

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
                Console.WriteLine("App started");
            }

            frame++;

            Runtime.Yield();
        }
    }
}
```

Avoid clearing or redrawing the screen every tick:

```csharp
public static void Main()
{
    Console.Clear();
    Console.WriteLine("This will flicker.");
    Runtime.Yield();
}
```

Use this instead:

```csharp
static bool initialized = false;

public static void Main()
{
    if (!initialized)
    {
        initialized = true;

        Console.Clear();
        Console.WriteLine("Draw once.");
    }

    Runtime.Yield();
}
```

## Building the app

In Visual Studio:

1. Restore NuGet packages.
2. Clean the solution.
3. Rebuild the solution.

After build, locate the generated:

```txt
app.pe
```

Copy it to:

```txt
sdmc:/ReSharp3DS/app.pe
```

The runtime also requires:

```txt
sdmc:/ReSharp3DS/mscorlib.pe
```

`mscorlib.pe` must match the nanoFramework version used by the SDK project.

## SD card layout

Recommended layout:

```txt
sdmc:/ReSharp3DS/app.pe
sdmc:/ReSharp3DS/mscorlib.pe
```

The native homebrew runtime is handled by the homebrew branch, not by this SDK branch.

## Examples

Suggested examples folder:

https://github.com/saysaa/ReSharp3DS/tree/sdk/Sample%20App


## Example: input test

```csharp
namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static bool oldA = false;
        static bool oldB = false;
        static bool oldStart = false;
        static bool oldSelect = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Input test");
                Console.WriteLine("Press A, B, START or SELECT.");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool start = Input.IsStartPressed();
            bool select = Input.IsSelectPressed();

            if (a && !oldA)
            {
                Console.WriteLine("A pressed");
            }

            if (b && !oldB)
            {
                Console.WriteLine("B pressed");
            }

            if (start && !oldStart)
            {
                Console.WriteLine("START pressed");
            }

            if (select && !oldSelect)
            {
                Console.WriteLine("SELECT pressed");
            }

            oldA = a;
            oldB = b;
            oldStart = start;
            oldSelect = select;

            Runtime.Yield();
        }
    }
}
```

## Example: counter

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
                Console.WriteLine("Counter example");
            }

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

## Troubleshooting

### The app stays on `[CLR] Execute`

Possible causes:

```txt
app.pe was not copied to sdmc:/ReSharp3DS/app.pe
mscorlib.pe is missing
mscorlib.pe does not match the expected nanoFramework version
native method mapping is broken
ReSharp3DS.cs method order changed
```

### Text flickers

Do not call `Console.Clear()` every tick.

Use an `initialized` static field.

### Inputs do not work

Make sure:

```txt
ReSharp3DS.cs is included in the project
ReSharp3DS.cs is compiled before Program.cs
app.pe was rebuilt and copied again
the native runtime version supports the input methods
```

### Build fails or Visual Studio cannot find nanoFramework

Make sure:

```txt
the .NET nanoFramework Extension is installed
Visual Studio was restarted after installing the extension
NuGet packages were restored
the project uses the expected nanoFramework CoreLibrary version
```

### Native calls do the wrong thing

The current runtime still uses index-based native mapping.

Do not reorder the methods inside `Native3DS`.

If you add new methods, the native runtime must be updated too.

## Current limitations

* The SDK is experimental.
* Native method binding is currently index-based.
* Graphics support is experimental.
* Exception reporting is limited.
* The runtime model is tick-based.
* The SDK currently targets a specific nanoFramework version.
* Newer nanoFramework versions may require runtime changes.

## Roadmap

Planned SDK improvements:

* Project template for Visual Studio
* Cleaner SDK package structure
* Better examples
* Stable graphics API
* Touch input
* Circle Pad input
* Audio API
* Filesystem API
* Better error messages
* Native binding by name instead of method index
* NuGet package for the SDK
* Documentation website

## License

Add your license here.


