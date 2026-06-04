namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static int selected = 0;

        static bool oldUp = false;
        static bool oldDown = false;
        static bool oldA = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                DrawMenu();
            }

            bool up = Input.IsUpPressed();
            bool down = Input.IsDownPressed();
            bool a = Input.IsAPressed();

            if (up && !oldUp)
            {
                selected--;

                if (selected < 0)
                {
                    selected = 2;
                }

                DrawMenu();
            }

            if (down && !oldDown)
            {
                selected++;

                if (selected > 2)
                {
                    selected = 0;
                }

                DrawMenu();
            }

            if (a && !oldA)
            {
                Console.Write("Selected option: ");
                Console.WriteLine(selected);
            }

            oldUp = up;
            oldDown = down;
            oldA = a;

            Runtime.Yield();
        }

        static void DrawMenu()
        {
            Console.Clear();

            Console.WriteLine("=== Menu ===");

            if (selected == 0) Console.WriteLine("> Start");
            else Console.WriteLine("  Start");

            if (selected == 1) Console.WriteLine("> Options");
            else Console.WriteLine("  Options");

            if (selected == 2) Console.WriteLine("> Exit");
            else Console.WriteLine("  Exit");

            Console.WriteLine("");
            Console.WriteLine("Use UP/DOWN, press A.");
        }
    }
}