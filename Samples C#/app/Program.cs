namespace ReSharp3DS
{
    public class Program
    {
        static bool initialized = false;

        static bool oldA = false;
        static bool oldB = false;
        static bool oldX = false;
        static bool oldY = false;
        static bool oldL = false;
        static bool oldR = false;
        static bool oldUp = false;
        static bool oldDown = false;
        static bool oldLeft = false;
        static bool oldRight = false;
        static bool oldStart = false;
        static bool oldSelect = false;

        public static void Main()
        {
            if (!initialized)
            {
                initialized = true;

                Console.Clear();
                Console.WriteLine("=== Input test ===");
                Console.WriteLine("Press buttons.");
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool x = Input.IsXPressed();
            bool y = Input.IsYPressed();
            bool l = Input.IsLPressed();
            bool r = Input.IsRPressed();
            bool up = Input.IsUpPressed();
            bool down = Input.IsDownPressed();
            bool left = Input.IsLeftPressed();
            bool right = Input.IsRightPressed();
            bool start = Input.IsStartPressed();
            bool select = Input.IsSelectPressed();

            if (a && !oldA) Console.WriteLine("A pressed");
            if (b && !oldB) Console.WriteLine("B pressed");
            if (x && !oldX) Console.WriteLine("X pressed");
            if (y && !oldY) Console.WriteLine("Y pressed");
            if (l && !oldL) Console.WriteLine("L pressed");
            if (r && !oldR) Console.WriteLine("R pressed");
            if (up && !oldUp) Console.WriteLine("UP pressed");
            if (down && !oldDown) Console.WriteLine("DOWN pressed");
            if (left && !oldLeft) Console.WriteLine("LEFT pressed");
            if (right && !oldRight) Console.WriteLine("RIGHT pressed");
            if (start && !oldStart) Console.WriteLine("START pressed");
            if (select && !oldSelect) Console.WriteLine("SELECT pressed");

            oldA = a;
            oldB = b;
            oldX = x;
            oldY = y;
            oldL = l;
            oldR = r;
            oldUp = up;
            oldDown = down;
            oldLeft = left;
            oldRight = right;
            oldStart = start;
            oldSelect = select;

            Runtime.Yield();
        }
    }
}