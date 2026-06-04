namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static int score = 0;
        static int hp = 100;

        static bool oldA = false;
        static bool oldB = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("=== Stats Test ===");
                PrintStats();
                Console.WriteLine("A = score +10");
                Console.WriteLine("B = damage");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();

            if (a && !oldA)
            {
                score = score + 10;
                PrintStats();
            }

            if (b && !oldB)
            {
                hp = hp - 5;

                if (hp < 0)
                {
                    hp = 0;
                }

                PrintStats();
            }

            oldA = a;
            oldB = b;

            Runtime.Yield();
        }

        static void PrintStats()
        {
            Console.Write("Score: ");
            Console.WriteLine(score);

            Console.Write("HP: ");
            Console.WriteLine(hp);
        }
    }
}