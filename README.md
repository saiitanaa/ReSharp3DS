# ReSharp3DS Documentation

<p align="center">
  <a href="https://github.com/saysaa/ReSharp3DS">GitHub Repository</a>
</p>

---

## Getting Started

This documentation lists the currently available functions and shows a basic example program.

## Available functions

| Function                              | Description                                   |
| ------------------------------------- | --------------------------------------------- |
| `Console.Write("Hello, world!");`     | Writes text to the console without a newline. |
| `Console.WriteLine("Hello, World!");` | Writes text to the console with a newline.    |
| `Runtime.Yield();`                    | Yields control to the runtime.                |
| `Input.IsStartPressed()`              | Checks if the START button is pressed.        |
| `Input.IsSelectPressed()`             | Checks if the SELECT button is pressed.       |
| `Input.IsAButtonPressed()`            | Checks if the A button is pressed.            |
| `Input.IsBButtonPressed()`            | Checks if the B button is pressed.            |
| `Input.IsXButtonPressed()`            | Checks if the X button is pressed.            |
| `Input.IsYButtonPressed()`            | Checks if the Y button is pressed.            |
| `Input.IsUpPressed()`                 | Checks if the D-pad UP button is pressed.     |
| `Input.IsDownPressed()`               | Checks if the D-pad DOWN button is pressed.   |
| `Input.IsLeftPressed()`               | Checks if the D-pad LEFT button is pressed.   |
| `Input.IsRightPressed()`              | Checks if the D-pad RIGHT button is pressed.  |
| `Input.IsLButtonPressed()`            | Checks if the L shoulder button is pressed.   |
| `Input.IsRButtonPressed()`            | Checks if the R shoulder button is pressed.   |
| `while (condition) { }`               | Runs a loop while the condition is true.      |

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

## Notes

> This README is written in Markdown so it displays directly on GitHub without needing a separate HTML page.

