# ReSharp3DS - Documentation

[--- Github Repo ---](https://github.com/saysaa/ReSharp3DS)

## Getting Started

### List of functions available

* `Console.Write("Hello, world!");` — Writes text to the console without a newline.

* `Console.WriteLine("Hello, World!");` — Writes text to the console with a newline.

* `Runtime.Yield();` — Yields control to the runtime.

* `Input.IsStartPressed()` & `Input.IsSelectPressed()` — Checks if the START or SELECT buttons are pressed.

* `Input.IsAButtonPressed()` & `Input.IsBButtonPressed()` — Checks if the A or B buttons are pressed.

* `Input.IsXButtonPressed()` & `Input.IsYButtonPressed()` — Checks if the X or Y buttons are pressed.

* `Input.IsUpPressed()`, `Input.IsDownPressed()`, `Input.IsLeftPressed()`, `Input.IsRightPressed()` — Checks if the D-pad buttons are pressed.

* `Input.IsLButtonPressed()` & `Input.IsRButtonPressed()` — Checks if the L or R shoulder buttons are pressed.

* `while (condition) { }` — A loop that continues while the condition is true.

## Example code

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
