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
                Console.WriteLine("Counter test");
            }

            frame++;

            if ((frame % 60) == 0)
            {
                Console.Write("Frame: ");
                Console.WriteLine(frame);
            }

            Runtime.Yield();
        }
    }
}