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
```csharp
namespace ReSharp3DS
{
    public class Program
    {
        public static void Main()
        {
            Console.Clear();

            Console.WriteLine("ReSharp3DS API Demo");
            Console.WriteLine("-------------------");
            Console.WriteLine("This app shows all available API functions.");
            Console.WriteLine("");
            Console.WriteLine("Press START to quit.");
            Console.WriteLine("");

            while (!Input.IsStartPressed())
            {
                Console.Clear();

                Console.WriteLine("ReSharp3DS API Demo");
                Console.WriteLine("-------------------");
                Console.WriteLine("");

                Console.Write("Console.Write: ");
                Console.WriteLine("working");

                Console.WriteLine("");
                Console.WriteLine("Input state:");
                Console.WriteLine("");

                if (Input.IsSelectPressed())
                {
                    Console.WriteLine("SELECT is pressed");
                }

                if (Input.IsAButtonPressed())
                {
                    Console.WriteLine("A is pressed");
                }

                if (Input.IsBButtonPressed())
                {
                    Console.WriteLine("B is pressed");
                }

                if (Input.IsXButtonPressed())
                {
                    Console.WriteLine("X is pressed");
                }

                if (Input.IsYButtonPressed())
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

                if (Input.IsLButtonPressed())
                {
                    Console.WriteLine("L button is pressed");
                }

                if (Input.IsRButtonPressed())
                {
                    Console.WriteLine("R button is pressed");
                }

                Console.WriteLine("");
                Console.WriteLine("Hold any button to test it.");
                Console.WriteLine("Press START to quit.");

                Runtime.Yield();
            }

            Console.Clear();
            Console.WriteLine("Bye.");
        }
    }
}
```

```

## Notes

> This README is written in Markdown so it displays directly on GitHub without needing a separate HTML page.

