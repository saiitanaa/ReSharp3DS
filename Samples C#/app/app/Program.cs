namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static int frame = 0;
        static bool oldStart = false;
        static bool oldSelect = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("ReSharp3DS Samples App - runtime...");
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

            frame++;

            Runtime.Yield();
        }
    }
}