namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;
        static bool oldA = false;
        static bool oldB = false;
        static bool oldX = false;
        static bool oldY = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("File API Test");
                Console.WriteLine("A = create save");
                Console.WriteLine("B = read save");
                Console.WriteLine("X = delete save");
                Console.WriteLine("Y = create saves dir");

                if (File.Exists("save.txt"))
                {
                    Console.WriteLine("save.txt exists");
                }
                else
                {
                    Console.WriteLine("save.txt missing");
                }
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool x = Input.IsXPressed();
            bool y = Input.IsYPressed();

            if (a && !oldA)
            {
                File.WriteAllText("save.txt", "Hello from ReSharp3DS!");
                Console.WriteLine("save.txt written");
            }

            if (b && !oldB)
            {
                if (File.Exists("save.txt"))
                {
                    string text = File.ReadAllText("save.txt");
                    Console.WriteLine(text);
                }
                else
                {
                    Console.WriteLine("No save.txt");
                }
            }

            if (x && !oldX)
            {
                File.Delete("save.txt");
                Console.WriteLine("save.txt deleted");
            }

            if (y && !oldY)
            {
                Directory.Create("saves");
                Console.WriteLine("saves dir created");
            }

            oldA = a;
            oldB = b;
            oldX = x;
            oldY = y;

            Runtime.Yield();
        }
    }
}