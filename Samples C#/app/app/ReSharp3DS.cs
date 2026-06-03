using System.Runtime.CompilerServices;

namespace ReSharp3DS
{
    internal static class Native3DS
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Clear();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Write(string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void WriteLine(string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void WriteInt(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void WriteLineInt(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsStartPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsSelectPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Yield();
    }

    public static class Console
    {
        public static void Clear()
        {
            Native3DS.Clear();
        }

        public static void Write(string text)
        {
            Native3DS.Write(text);
        }

        public static void WriteLine(string text)
        {
            Native3DS.WriteLine(text);
        }

        public static void Write(int value)
        {
            Native3DS.WriteInt(value);
        }

        public static void WriteLine(int value)
        {
            Native3DS.WriteLineInt(value);
        }
    }

    public static class Input
    {
        public static bool IsStartPressed()
        {
            return Native3DS.IsStartPressed() != 0;
        }

        public static bool IsSelectPressed()
        {
            return Native3DS.IsSelectPressed() != 0;
        }
    }



    public static class Runtime
    {
        public static void Yield()
        {
            Native3DS.Yield();
        }
    }
}