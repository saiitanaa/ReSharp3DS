namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static bool oldStart = false;
        static bool oldSelect = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Input test");
                Console.WriteLine("Press START or SELECT.");
            }

            bool start = Input.IsStartPressed();
            bool select = Input.IsSelectPressed();

            if (start && !oldStart)
            {
                Console.WriteLine("START pressed");
            }

            if (select && !oldSelect)
            {
                Console.WriteLine("SELECT pressed");
            }

            oldStart = start;
            oldSelect = select;

            Runtime.Yield();
        }
    }
}