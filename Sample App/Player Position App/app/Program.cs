namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static int playerX = 10;
        static int playerY = 5;

        static bool oldUp = false;
        static bool oldDown = false;
        static bool oldLeft = false;
        static bool oldRight = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("Player position test");
                PrintPosition();
            }

            bool up = Input.IsUpPressed();
            bool down = Input.IsDownPressed();
            bool left = Input.IsLeftPressed();
            bool right = Input.IsRightPressed();

            bool moved = false;

            if (up && !oldUp)
            {
                playerY--;
                moved = true;
            }

            if (down && !oldDown)
            {
                playerY++;
                moved = true;
            }

            if (left && !oldLeft)
            {
                playerX--;
                moved = true;
            }

            if (right && !oldRight)
            {
                playerX++;
                moved = true;
            }

            if (moved)
            {
                PrintPosition();
            }

            oldUp = up;
            oldDown = down;
            oldLeft = left;
            oldRight = right;

            Runtime.Yield();
        }

        static void PrintPosition()
        {
            Console.Write("X: ");
            Console.WriteLine(playerX);

            Console.Write("Y: ");
            Console.WriteLine(playerY);
        }
    }
}