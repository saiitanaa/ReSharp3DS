# Documentation of ReSharp3DS


<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>ReSharp3DS - Documentation</title>
        <link rel="icon" type="image/x-icon" href="icon.png">
    <style>
        body {
            background-color: #121010;
            font-family: Arial, sans-serif;
            padding: 20px;
        } h1 {
            color: #50C878; 
            text-align: center;
            font-family: 'Montserrat-Bold', 'Arial Black', sans-serif;
            font-size: 35px;
        } h2 {
            color: teal;
            text-align: center;
            font-size: 25px;
        } h3 {
            color: purple;
            text-align: left;
            font-family: 'Montserrat', sans-serif;
            font-weight: bold;
            font-size: 20px;
        } p, ul {
            color: #e0e0e0;
            text-align: left;
            font-size: 15px;
        } code {
            background-color: #252323;
            color: #f8f8f2;
            padding: 2px 6px;
            border-radius: 4px;
            font-family: 'Courier New', Courier, monospace;
        } pre code {
            display: block;
            padding: 15px;
            overflow-x: auto;
        } a {
            color: #eeb10a;
            display: block;
            text-align: center;
            text-decoration: none;
            font-weight: bold;
        } a:hover {
            text-decoration: underline;
        }
    </style>
</head> <body> 
    <h1>ReSharp3DS - Documentation</h1>
    <a href="https://github.com/saysaa/ReSharp3DS" target="_blank">- - - Github Repo - - -</a>
    <h2>Getting Started</h2>

    <h3>List of functions available:</h3>
    <ul>
        <li><code>Console.Write("Hello, world!");</code> — Writes text to the console without a newline.</li>
        <p></p>
        <li><code>Console.WriteLine("Hello, World!");</code> — Writes text to the console with a newline.</li>
        <p></p>
        <li><code>Runtime.Yield();</code> — Yields control to the runtime.</li>
        <p></p>
        <li><code>Input.IsStartPressed()</code> & <code>Input.IsSelectPressed()</code> — Checks if the START or SELECT buttons are pressed.</li>
        <p></p>
        <li><code>Input.IsAButtonPressed()</code> & <code>Input.IsBButtonPressed()</code> — Checks if the A or B buttons are pressed.</li>
        <p></p>
        <li><code>Input.IsXButtonPressed()</code> & <code>Input.IsYButtonPressed()</code> — Checks if the X or Y buttons are pressed.</li>
        <p></p>
        <li><code>Input.IsUpPressed()</code>, <code>Input.IsDownPressed()</code>, <code>Input.IsLeftPressed()</code>, <code>Input.IsRightPressed()</code> — Checks if the D-pad buttons are pressed.</li>
        <p></p>
        <li><code>Input.IsLButtonPressed()</code> & <code>Input.IsRButtonPressed()</code> — Checks if the L or R shoulder buttons are pressed.</li>
        <p></p>
        <li><code>while (condition) { }</code> — A loop that continues while the condition is true.</li>
        <p></p>
    </ul>

    <h2>Example code:</h2>

    <pre><code>
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
    </code></pre>

</body>
</html>
