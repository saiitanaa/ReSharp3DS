using System.Runtime.CompilerServices;

namespace ReSharp3DS
{
    internal static class Native3DS
    {
        // Console
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void Clear();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void Write(string text);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void WriteLine(string text);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void WriteInt(int value);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void WriteLineInt(int value);

        // Input held
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsStartPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsSelectPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsAPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsBPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsXPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsYPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsLPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsRPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsUpPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsDownPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsLeftPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsRightPressed();

        // Input pressed once
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsStartPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsSelectPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsAPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsBPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsXPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsYPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsLPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsRPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsUpPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsDownPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsLeftPressedOnce();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int IsRightPressedOnce();

        // Touch / Circle Pad
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int TouchIsPressed();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int TouchX();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int TouchY();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int CirclePadX();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int CirclePadY();

        // Runtime
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void Yield();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void RuntimeExit();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void RuntimeRestart();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern string RuntimeGetVersion();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int RuntimeGetFps();

        // Time / Random
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int TimeMilliseconds();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int TimeSeconds();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void RandomSeed(int seed);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int RandomNext(int min, int max);

        // Graphics
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsClear(int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawPixel(int x, int y, int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsFillRect(int x, int y, int width, int height, int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawRect(int x, int y, int width, int height, int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawLine(int x0, int y0, int x1, int y1, int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawCircle(int x, int y, int radius, int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsFillCircle(int x, int y, int radius, int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawText(int x, int y, string text, int color);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawBitmap(string path, int x, int y);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawSprite(string path, int x, int y);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsDrawSpriteScaled(string path, int x, int y, int width, int height);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void GraphicsPresent();

        // Audio
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioInit();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioBeep(int frequency, int durationMs);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioStop();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioPlayWav(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioPlaySfx(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioSetVolume(int volume);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioSetSfxVolume(int volume);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioSetMusicVolume(int volume);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int AudioIsPlaying();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int AudioIsMusicPlaying();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioLoop(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioStopMusic();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioPauseMusic();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioResumeMusic();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void AudioSetMusicLoop(int loop);

        // File / Directory
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int FileExists(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void FileWriteAllText(string path, string text);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern string FileReadAllText(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void FileDelete(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int FileGetSize(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int DirectoryExists(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void DirectoryCreate(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern void DirectoryDelete(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern string DirectoryListFiles(string path);
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern string DirectoryListFolders(string path);

        // App / System
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern string AppGetPath();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern string AppGetDirectory();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern string AppGetName();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int SystemIsNew3DS();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int SystemGetBatteryLevel();
        [MethodImpl(MethodImplOptions.InternalCall)] public static extern int SystemGetFreeMemory();
    }

    public static class Console
    {
        public static void Clear() { Native3DS.Clear(); }
        public static void Write(string text) { Native3DS.Write(text); }
        public static void WriteLine(string text) { Native3DS.WriteLine(text); }
        public static void Write(int value) { Native3DS.WriteInt(value); }
        public static void WriteLine(int value) { Native3DS.WriteLineInt(value); }
    }

    public static class Debug
    {
        public static void Log(string text) { Console.WriteLine(text); }
        public static void LogInt(int value) { Console.WriteLine(value); }
    }

    public static class Input
    {
        public static bool IsStartPressed() { return Native3DS.IsStartPressed() != 0; }
        public static bool IsSelectPressed() { return Native3DS.IsSelectPressed() != 0; }
        public static bool IsAPressed() { return Native3DS.IsAPressed() != 0; }
        public static bool IsBPressed() { return Native3DS.IsBPressed() != 0; }
        public static bool IsXPressed() { return Native3DS.IsXPressed() != 0; }
        public static bool IsYPressed() { return Native3DS.IsYPressed() != 0; }
        public static bool IsLPressed() { return Native3DS.IsLPressed() != 0; }
        public static bool IsRPressed() { return Native3DS.IsRPressed() != 0; }
        public static bool IsUpPressed() { return Native3DS.IsUpPressed() != 0; }
        public static bool IsDownPressed() { return Native3DS.IsDownPressed() != 0; }
        public static bool IsLeftPressed() { return Native3DS.IsLeftPressed() != 0; }
        public static bool IsRightPressed() { return Native3DS.IsRightPressed() != 0; }

        public static bool IsStartPressedOnce() { return Native3DS.IsStartPressedOnce() != 0; }
        public static bool IsSelectPressedOnce() { return Native3DS.IsSelectPressedOnce() != 0; }
        public static bool IsAPressedOnce() { return Native3DS.IsAPressedOnce() != 0; }
        public static bool IsBPressedOnce() { return Native3DS.IsBPressedOnce() != 0; }
        public static bool IsXPressedOnce() { return Native3DS.IsXPressedOnce() != 0; }
        public static bool IsYPressedOnce() { return Native3DS.IsYPressedOnce() != 0; }
        public static bool IsLPressedOnce() { return Native3DS.IsLPressedOnce() != 0; }
        public static bool IsRPressedOnce() { return Native3DS.IsRPressedOnce() != 0; }
        public static bool IsUpPressedOnce() { return Native3DS.IsUpPressedOnce() != 0; }
        public static bool IsDownPressedOnce() { return Native3DS.IsDownPressedOnce() != 0; }
        public static bool IsLeftPressedOnce() { return Native3DS.IsLeftPressedOnce() != 0; }
        public static bool IsRightPressedOnce() { return Native3DS.IsRightPressedOnce() != 0; }

        public static int CirclePadX() { return Native3DS.CirclePadX(); }
        public static int CirclePadY() { return Native3DS.CirclePadY(); }
    }

    public static class Touch
    {
        public static bool IsPressed() { return Native3DS.TouchIsPressed() != 0; }
        public static int X() { return Native3DS.TouchX(); }
        public static int Y() { return Native3DS.TouchY(); }
    }

    public static class Runtime
    {
        public static void Yield() { Native3DS.Yield(); }
        public static void Exit() { Native3DS.RuntimeExit(); }
        public static void Restart() { Native3DS.RuntimeRestart(); }
        public static string GetVersion() { return Native3DS.RuntimeGetVersion(); }
        public static int GetFps() { return Native3DS.RuntimeGetFps(); }
    }

    public static class Time
    {
        public static int Milliseconds() { return Native3DS.TimeMilliseconds(); }
        public static int Seconds() { return Native3DS.TimeSeconds(); }
    }

    public static class Random
    {
        public static void Seed(int seed) { Native3DS.RandomSeed(seed); }
        public static int Next(int min, int max) { return Native3DS.RandomNext(min, max); }
    }

    public static class Math3DS
    {
        public static int Clamp(int value, int min, int max)
        {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        public static int Abs(int value)
        {
            return value < 0 ? -value : value;
        }

        public static int Lerp(int a, int b, int percent)
        {
            return a + ((b - a) * percent) / 100;
        }
    }

    public static class Screen
    {
        public const int TopWidth = 400;
        public const int TopHeight = 240;
        public const int BottomWidth = 320;
        public const int BottomHeight = 240;
    }

    public static class Color
    {
        public const int Black = 0x000000;
        public const int White = 0xFFFFFF;
        public const int Red = 0xFF0000;
        public const int Green = 0x00FF00;
        public const int Blue = 0x0000FF;
        public const int Yellow = 0xFFFF00;
        public const int Cyan = 0x00FFFF;
        public const int Magenta = 0xFF00FF;
        public const int Gray = 0x808080;
        public const int Orange = 0xFF8000;
        public const int Purple = 0x8000FF;

        public static int RGB(int r, int g, int b)
        {
            r = Math3DS.Clamp(r, 0, 255);
            g = Math3DS.Clamp(g, 0, 255);
            b = Math3DS.Clamp(b, 0, 255);
            return (r << 16) | (g << 8) | b;
        }
    }

    public static class Graphics
    {
        public static void Clear(int color) { Native3DS.GraphicsClear(color); }
        public static void DrawPixel(int x, int y, int color) { Native3DS.GraphicsDrawPixel(x, y, color); }
        public static void FillRect(int x, int y, int width, int height, int color) { Native3DS.GraphicsFillRect(x, y, width, height, color); }
        public static void DrawRect(int x, int y, int width, int height, int color) { Native3DS.GraphicsDrawRect(x, y, width, height, color); }
        public static void DrawLine(int x0, int y0, int x1, int y1, int color) { Native3DS.GraphicsDrawLine(x0, y0, x1, y1, color); }
        public static void DrawCircle(int x, int y, int radius, int color) { Native3DS.GraphicsDrawCircle(x, y, radius, color); }
        public static void FillCircle(int x, int y, int radius, int color) { Native3DS.GraphicsFillCircle(x, y, radius, color); }
        public static void DrawText(int x, int y, string text, int color) { Native3DS.GraphicsDrawText(x, y, text, color); }
        public static void DrawBitmap(string path, int x, int y) { Native3DS.GraphicsDrawBitmap(path, x, y); }
        public static void DrawSprite(string path, int x, int y) { Native3DS.GraphicsDrawSprite(path, x, y); }
        public static void DrawSprite(string path, int x, int y, int width, int height) { Native3DS.GraphicsDrawSpriteScaled(path, x, y, width, height); }
        public static void Present() { Native3DS.GraphicsPresent(); }
    }

    public static class UI
    {
        public static void DrawButton(int x, int y, int width, int height, string text)
        {
            Graphics.FillRect(x, y, width, height, Color.Gray);
            Graphics.DrawRect(x, y, width, height, Color.White);
            Graphics.DrawText(x + 6, y + 6, text, Color.White);
        }

        public static void DrawProgressBar(int x, int y, int width, int height, int value, int max)
        {
            if (max <= 0) max = 1;
            value = Math3DS.Clamp(value, 0, max);
            int fill = (width * value) / max;

            Graphics.DrawRect(x, y, width, height, Color.White);
            Graphics.FillRect(x + 1, y + 1, fill - 2, height - 2, Color.Green);
        }

        public static void MessageBox(string text)
        {
            Graphics.FillRect(20, 70, 280, 90, Color.Black);
            Graphics.DrawRect(20, 70, 280, 90, Color.White);
            Graphics.DrawText(34, 92, text, Color.White);
            Graphics.DrawText(34, 126, "Press A", Color.Gray);
            Graphics.Present();

            while (!Input.IsAPressedOnce())
            {
                Runtime.Yield();
            }
        }
    }

    public static class Notes
    {
        public const int C4 = 262; public const int D4 = 294; public const int E4 = 330; public const int F4 = 349; public const int G4 = 392; public const int A4 = 440; public const int B4 = 494;
        public const int C5 = 523; public const int D5 = 587; public const int E5 = 659; public const int F5 = 698; public const int G5 = 784; public const int A5 = 880; public const int B5 = 988;
    }

    public static class Audio
    {
        public static void Init() { Native3DS.AudioInit(); }
        public static void Beep(int frequency, int durationMs) { Native3DS.AudioBeep(frequency, durationMs); }
        public static void Stop() { Native3DS.AudioStop(); }
        public static void PlayWav(string path) { Native3DS.AudioPlayWav(path); }
        public static void PlaySfx(string path) { Native3DS.AudioPlaySfx(path); }
        public static void SetVolume(int volume) { Native3DS.AudioSetVolume(volume); }
        public static void SetSfxVolume(int volume) { Native3DS.AudioSetSfxVolume(volume); }
        public static void SetMusicVolume(int volume) { Native3DS.AudioSetMusicVolume(volume); }
        public static bool IsPlaying() { return Native3DS.AudioIsPlaying() != 0; }
        public static bool IsMusicPlaying() { return Native3DS.AudioIsMusicPlaying() != 0; }
        public static void Loop(string path) { Native3DS.AudioLoop(path); }
        public static void StopMusic() { Native3DS.AudioStopMusic(); }
        public static void PauseMusic() { Native3DS.AudioPauseMusic(); }
        public static void ResumeMusic() { Native3DS.AudioResumeMusic(); }
        public static void SetMusicLoop(bool loop) { Native3DS.AudioSetMusicLoop(loop ? 1 : 0); }
    }

    public static class File
    {
        public static bool Exists(string path) { return Native3DS.FileExists(path) != 0; }
        public static void WriteAllText(string path, string text) { Native3DS.FileWriteAllText(path, text); }
        public static string ReadAllText(string path) { return Native3DS.FileReadAllText(path); }
        public static void Delete(string path) { Native3DS.FileDelete(path); }
        public static int GetSize(string path) { return Native3DS.FileGetSize(path); }
    }

    public static class Directory
    {
        public static bool Exists(string path) { return Native3DS.DirectoryExists(path) != 0; }
        public static void Create(string path) { Native3DS.DirectoryCreate(path); }
        public static void Delete(string path) { Native3DS.DirectoryDelete(path); }
        public static string ListFiles(string path) { return Native3DS.DirectoryListFiles(path); }
        public static string ListFolders(string path) { return Native3DS.DirectoryListFolders(path); }
    }

    public static class Save
    {
        static string PathFor(string key)
        {
            return "saves/" + key + ".txt";
        }

        public static void SetString(string key, string value)
        {
            if (!Directory.Exists("saves"))
            {
                Directory.Create("saves");
            }

            File.WriteAllText(PathFor(key), value);
        }

        public static string GetString(string key, string defaultValue)
        {
            string path = PathFor(key);

            if (!File.Exists(path))
            {
                return defaultValue;
            }

            return File.ReadAllText(path);
        }

        public static void SetInt(string key, int value)
        {
            SetString(key, value.ToString());
        }

        public static int GetInt(string key, int defaultValue)
        {
            string text = GetString(key, "");

            if (text == null || text.Length == 0)
            {
                return defaultValue;
            }

            int sign = 1;
            int index = 0;
            int value = 0;

            if (text[0] == '-')
            {
                sign = -1;
                index = 1;
            }

            for (; index < text.Length; index++)
            {
                char c = text[index];

                if (c < '0' || c > '9')
                {
                    return defaultValue;
                }

                value = value * 10 + (c - '0');
            }

            return value * sign;
        }
    }

    public static class App
    {
        public static string GetPath() { return Native3DS.AppGetPath(); }
        public static string GetDirectory() { return Native3DS.AppGetDirectory(); }
        public static string GetName() { return Native3DS.AppGetName(); }
        public static bool Exists(string path) { return File.Exists(path); }
        public static string ReadText(string path) { return File.ReadAllText(path); }
        public static void WriteText(string path, string text) { File.WriteAllText(path, text); }
    }

    public static class SystemInfo
    {
        public static bool IsNew3DS() { return Native3DS.SystemIsNew3DS() != 0; }
        public static int GetBatteryLevel() { return Native3DS.SystemGetBatteryLevel(); }
        public static int GetFreeMemory() { return Native3DS.SystemGetFreeMemory(); }
    }
}
