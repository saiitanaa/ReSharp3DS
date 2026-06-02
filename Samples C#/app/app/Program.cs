using System.Runtime.CompilerServices;

public class Program
{
    public static void Main()
    {
        Native3DS.Print();
    }
}

public static class Native3DS
{
    [MethodImpl(MethodImplOptions.InternalCall)]
    public static extern void Print();
}