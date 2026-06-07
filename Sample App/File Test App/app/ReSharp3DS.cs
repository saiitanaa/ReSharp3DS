using System.Runtime.CompilerServices;

namespace ReSharp3DS
{
    internal static class Native3DS
    {
        // Console: native methods
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

        // Input: native methods
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

        // Runtime: native method
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Yield();

        // Audio: native methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioInit();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioBeep(int frequency, int durationMs);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioStop();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioPlayWav(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioSetVolume(int volume);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioLoop(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AudioStopMusic();

        // File: native methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int FileExists(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void FileWriteAllText(string path, string text);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string FileReadAllText(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void FileDelete(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int DirectoryExists(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void DirectoryCreate(string path);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void DirectoryDelete(string path);
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

    public static class Notes // For audio
    {
        public const int C4 = 262;
        public const int D4 = 294;
        public const int E4 = 330;
        public const int F4 = 349;
        public const int G4 = 392;
        public const int A4 = 440;
        public const int B4 = 494;

        public const int C5 = 523;
        public const int D5 = 587;
        public const int E5 = 659;
        public const int F5 = 698;
        public const int G5 = 784;
        public const int A5 = 880;
        public const int B5 = 988;
    }

    public static class Audio
    {
        public static void Init()
        {
            Native3DS.AudioInit();
        }

        public static void Beep(int frequency, int durationMs)
        {
            Native3DS.AudioBeep(frequency, durationMs);
        }

        public static void Stop()
        {
            Native3DS.AudioStop();
        }

        public static void PlayWav(string path)
        {
            Native3DS.AudioPlayWav(path);
        }

        public static void SetVolume(int volume)
        {
            Native3DS.AudioSetVolume(volume);
        }

        public static void Loop(string path)
        {
            Native3DS.AudioLoop(path);
        }

        public static void StopMusic()
        {
            Native3DS.AudioStopMusic();
        }
    }

    public static class File
    {
        public static bool Exists(string path)
        {
            return Native3DS.FileExists(path) != 0;
        }

        public static void WriteAllText(string path, string text)
        {
            Native3DS.FileWriteAllText(path, text);
        }

        public static string ReadAllText(string path)
        {
            return Native3DS.FileReadAllText(path);
        }

        public static void Delete(string path)
        {
            Native3DS.FileDelete(path);
        }
    }

    public static class Directory
    {
        public static bool Exists(string path)
        {
            return Native3DS.DirectoryExists(path) != 0;
        }

        public static void Create(string path)
        {
            Native3DS.DirectoryCreate(path);
        }

        public static void Delete(string path)
        {
            Native3DS.DirectoryDelete(path);
        }
    }
}
