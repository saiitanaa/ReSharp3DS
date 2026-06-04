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
                Console.WriteLine("=== ReSharp3DS Diagnostic ===");
                Console.WriteLine("Console: OK");
                Console.WriteLine("Runtime tick: running");
                Console.WriteLine("Inputs: press buttons");
                Console.WriteLine("");
            }

            frame++;

            if ((frame % 120) == 0)
            {
                Console.Write("Alive frame ");
                Console.WriteLine(frame);
            }

            if (Input.IsAPressed())
            {
                Console.WriteLine("A held");
            }

            if (Input.IsBPressed())
            {
                Console.WriteLine("B held");
            }

            Runtime.Yield();
        }
    }
}