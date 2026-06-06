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
                Console.WriteLine("Audio Test");
                Console.WriteLine("A = play audio.wav");
                Console.WriteLine("B = beep 440Hz");
                Console.WriteLine("X = beep 880Hz");
                Console.WriteLine("Y = stop");

                Audio.Init();
                Audio.SetVolume(80);
            }

            bool a = Input.IsAPressed();
            bool b = Input.IsBPressed();
            bool x = Input.IsXPressed();
            bool y = Input.IsYPressed();

            if (a && !oldA)
            {
                Console.WriteLine("Play audio.wav");
                Audio.PlayWav("audio.wav");
            }

            if (b && !oldB)
            {
                Console.WriteLine("Beep 440Hz");
                Audio.Beep(440, 200);
            }

            if (x && !oldX)
            {
                Console.WriteLine("Beep 880Hz");
                Audio.Beep(880, 200);
            }

            if (y && !oldY)
            {
                Console.WriteLine("Stop audio");
                Audio.Stop();
                Audio.StopMusic();
            }

            oldA = a;
            oldB = b;
            oldX = x;
            oldY = y;

            Runtime.Yield();
        }
    }
}