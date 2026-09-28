## ReSharp3DS API

Current runtime and SDK versions include APIs for console output, input, touch, runtime control, timing, graphics, audio, files, directories, saves, app information, and system information.

Available APIs include:

```txt
Console API
Debug API
Input API
Runtime API
Time API
Random API
Touch API
CirclePad API
Screen constants
App API
SystemInfo API
Graphics API
Audio API
File API
Directory API
Save API
```

Newer SDK helpers include:

```txt
Button
ScreenTarget
Vector2
Rect
Timer
GameApp
Input.IsHeld(Button)
Input.IsPressed(Button)
Input.CirclePad()
Touch.Position()
Runtime.ReturnToLauncher()
Graphics.SetTarget(ScreenTarget.Top/Bottom)
Graphics.DrawSpriteTransparent(...)
Save.Exists(...)
Save.Delete(...)
Save.SetBool(...)
Save.GetBool(...)
```

## Time API

```csharp
int ms = Time.Milliseconds();
int seconds = Time.Seconds();
```

## Random API

```csharp
Random.Seed(1234);
int value = Random.Next(0, 100);
```

## Touch API

```csharp
bool pressed = Touch.IsPressed();
int x = Touch.X();
int y = Touch.Y();
```

## CirclePad API

```csharp
int x = Input.CirclePadX();
int y = Input.CirclePadY();
```

## Screen constants

```csharp
Screen.TopWidth
Screen.TopHeight
Screen.BottomWidth
Screen.BottomHeight
```

## App API

```csharp
string path = App.GetPath();
string dir = App.GetDirectory();
string name = App.GetName();
```

## SystemInfo API

```csharp
bool isNew3DS = SystemInfo.IsNew3DS();
int battery = SystemInfo.GetBatteryLevel();
int memory = SystemInfo.GetFreeMemory();
```

Some values may return fallback values depending on hardware support.

Current fallback behavior:

```txt
GetBatteryLevel() returns -1 if unavailable
GetFreeMemory() returns 0 if unavailable
```

## Graphics API

Available methods include:

```csharp
Graphics.Clear(int color);
Graphics.DrawPixel(int x, int y, int color);
Graphics.FillRect(int x, int y, int width, int height, int color);
Graphics.DrawRect(int x, int y, int width, int height, int color);
Graphics.DrawLine(int x1, int y1, int x2, int y2, int color);
Graphics.DrawCircle(int x, int y, int radius, int color);
Graphics.FillCircle(int x, int y, int radius, int color);
Graphics.DrawText(int x, int y, string text, int color);
Graphics.DrawBitmap(string path, int x, int y);
Graphics.DrawSprite(string path, int x, int y);
Graphics.DrawSprite(string path, int x, int y, int width, int height);
Graphics.DrawSpriteTransparent(string path, int x, int y, int transparentColor);
Graphics.SetTarget(ScreenTarget.Top);
Graphics.SetTarget(ScreenTarget.Bottom);
Graphics.Present();
```

`Graphics.DrawBitmap`, `Graphics.DrawSprite`, and `Graphics.DrawSpriteTransparent` support simple BMP files.

Recommended format:

```txt
BMP
24-bit or 32-bit
uncompressed
```

## Audio API

Available methods include:

```csharp
Audio.Init();
Audio.Beep(int frequency, int durationMs);
Audio.Stop();

Audio.PlayWav(string path);
Audio.Loop(string path);
Audio.StopMusic();

Audio.SetVolume(int volume);
Audio.SetSfxVolume(int volume);
Audio.SetMusicVolume(int volume);

Audio.IsPlaying();
Audio.IsMusicPlaying();
```

WAV files should be:

```txt
PCM WAV
16-bit
44100 Hz or 22050 Hz
mono or stereo
```

For real hardware, DSP audio must be available on the SD card.

## Save API

```csharp
Save.SetInt("score", 1200);
int score = Save.GetInt("score", 0);

Save.SetString("name", "Player");
string name = Save.GetString("name", "Default");

Save.SetBool("unlocked", true);
bool unlocked = Save.GetBool("unlocked", false);

bool exists = Save.Exists("score");
Save.Delete("score");
```

## File and Directory API

```csharp
File.Exists(string path);
File.WriteAllText(string path, string text);
File.ReadAllText(string path);
File.Delete(string path);

Directory.Exists(string path);
Directory.Create(string path);
Directory.Delete(string path);
```

Paths can be relative to the running `.pe` application folder.
