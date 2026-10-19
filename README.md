# KVR_Sound_Chemical_MinstrelFolkVST_Source_Compiled

<img width="520" height="300" alt="image" src="https://github.com/user-attachments/assets/1c153873-d935-4351-ba63-7b84df7072c2" />

Windows VST3 build of the GPT-generated **Minstrel Folk** source that [Sound Chemical](https://www.kvraudio.com/forum/viewtopic.php?p=9313048#p9313048) posted on KVR Audio (16 Oct 2026).

The original author asked someone to compile the files into a Windows VSTi and said ownership was not a concern. This repo is that compile, plus the source as posted.

## What it is

A small MIDI instrument: additive “pluck” sines plus a soft harmonic layer, three knobs (String/Choir, Warmth, Room). It does not contain recordings of The New Christy Minstrels.

## Release

The GitHub release `KVR_Sound_Chemical_MinstrelFolkVST_Source_Compiled` has a Windows VST3 zip.

Copy `Minstrel Folk.vst3` to `C:\Program Files\Common Files\VST3` and rescan plugins.

## License

Built with [JUCE](https://juce.com/) 8 under the AGPLv3. The plugin binary and this packaging are AGPLv3. Original sketch: Sound Chemical, KVR thread above.

## Compile notes

The posted source did not compile as-is. Two lines were changed so MSVC would build it:

- Removed unused nested `FolkVoice` / `FolkSound` / `makeLayout` declarations from the header so the real classes in the .cpp could be used.
- Removed the write to undeclared `choirEnv`.

The sound code is otherwise the original sketch.

## Build

Needs CMake, Visual Studio 2022, and a JUCE 8 tree.

```
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DJUCE_SOURCE_DIR="path/to/JUCE"
cmake --build build --config Release --target MinstrelFolk_VST3
```
