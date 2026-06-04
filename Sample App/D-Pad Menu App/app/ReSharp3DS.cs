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
        public static extern int IsAPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsBPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsXPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsYPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsLPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsRPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsUpPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsDownPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsLeftPressed();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int IsRightPressed();

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

        public static bool IsAPressed()
        {
            return Native3DS.IsAPressed() != 0;
        }

        public static bool IsBPressed()
        {
            return Native3DS.IsBPressed() != 0;
        }

        public static bool IsXPressed()
        {
            return Native3DS.IsXPressed() != 0;
        }

        public static bool IsYPressed()
        {
            return Native3DS.IsYPressed() != 0;
        }

        public static bool IsLPressed()
        {
            return Native3DS.IsLPressed() != 0;
        }

        public static bool IsRPressed()
        {
            return Native3DS.IsRPressed() != 0;
        }

        public static bool IsUpPressed()
        {
            return Native3DS.IsUpPressed() != 0;
        }

        public static bool IsDownPressed()
        {
            return Native3DS.IsDownPressed() != 0;
        }

        public static bool IsLeftPressed()
        {
            return Native3DS.IsLeftPressed() != 0;
        }

        public static bool IsRightPressed()
        {
            return Native3DS.IsRightPressed() != 0;
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