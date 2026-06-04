<div align="center">
# >_ ReSharp3DS
</div>

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS/tree/docs">ReSharp3DS Documentation</a>
</p>

ReSharp3DS is an experimental project that runs C# code on the Nintendo 3DS using nanoCLR / nanoFramework.

The project uses a C++ 3DS homebrew application to load C# assemblies compiled as `.pe` files, then executes them through nanoCLR.

### Screenshots 
<img width="400" height="300" alt="IMG_0694" src="https://github.com/user-attachments/assets/8ad88d4e-ba59-45e4-a533-e22ee33996cb" />
<img width="400" height="300" alt="IMG_0703" src="https://github.com/user-attachments/assets/08046418-f449-4dc3-8240-dbe1549b0781" />


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
</details>

<details>
<summary><b>To be implemented (Click to expand)</b></summary>

* [ ] Expanded Console API (bool, float, better formatting)
* [ ] Automatic native method binding instead of index-based mapping
* [ ] Graphics & Audio APIs
* [ ] Filesystem support
* [ ] Better error reporting for C# exceptions
* [ ] Proper SDK structure & templates
* [ ] Stabilize HOME Menu suspend/resume behavior
</details>

---

## File Structure

For the runtime to function correctly, your SD card must be organized as follows:

```
SD:/
├── 3ds/
│   └── ReSharp3DS.3dsx          # The homebrew application
└── ReSharp3DS/                  # Required data folder
    ├── mscorlib.pe              # nanoFramework base library
    └── app.pe                   # Your compiled C# program (user app)
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

---

## Disclaimer

This project is not affiliated with Nintendo, Microsoft, the .NET Foundation, or the nanoFramework project. It is an experimental homebrew and runtime porting project intended for learning, research, and development purposes.

## Assets 
<img width="200" height="200" alt="ReSharp3DS Logo" src="https://github.com/user-attachments/assets/b9d68a0d-2456-4bcb-a413-a0e7eee89aec" />
