[<img src="https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ReSharp3DS&section=header&reversal=false&textBg=false&descAlign=64" />
](https://capsule-render.vercel.app/api?type=waving&height=300&color=gradient&text=ReSharp3DS%20SDK)

ReSharp3DS is an experimental project that runs C# code on the Nintendo 3DS using nanoCLR / nanoFramework.

The project uses a C++ 3DS homebrew application to load C# assemblies compiled as `.pe` files, then executes them through nanoCLR.

### Screenshots 
<img width="400" height="300" alt="IMG_0730" src="https://github.com/user-attachments/assets/bff1cacb-165b-45e2-9b38-f4d5698f7843" />
<img width="400" height="300" alt="IMG_0731" src="https://github.com/user-attachments/assets/68339bd4-f017-4502-bd06-c524884d4492" />

## Links

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS/tree/docs">ReSharp3DS Documentation</a>
</p>

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS/tree/sdk">ReSharp3DS - SDK</a>
</p>

<p align="center">
  <a href="https://discord.gg/ENBwURmUj8">Discord server</a>
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

</details>

<details>
<summary><b>To be implemented (Click to expand)</b></summary>

* [ ] Expanded Console API (bool, float, better formatting)
* [ ] Better error reporting for C# exceptions
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

#### 2. Building the C# application
Compile your C# code into a `.pe` assembly using the nanoFramework toolchain. Rename the output file to `app.pe`.

#### 3. Deployment
1. Copy `ReSharp3DS.3dsx` to `SD:/3ds/`.
2. Copy `mscorlib.pe` and your `app.pe` to `SD:/ReSharp3DS/` (create the folder at the root of the SD if it doesn't exist).
3. Launch the app from the **Homebrew Launcher**.

---

### Troubleshooting

If a file is missing or incorrectly placed, the program will display an error:
`[FATAL] app load failed`

Make sure the `ReSharp3DS` folder is at the **root** of the SD card, not inside the `/3ds/` folder.

