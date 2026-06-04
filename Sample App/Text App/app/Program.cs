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
                Console.WriteLine("Hello, world from C#!");
            }

            Runtime.Yield();
        }
    }
}