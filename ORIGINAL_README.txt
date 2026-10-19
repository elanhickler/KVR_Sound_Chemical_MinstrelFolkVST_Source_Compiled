MINSTREL FOLK — Windows VST3 source project
===========================================

What it is
----------
A small MIDI-playable folk-pop instrument inspired by the bright, close-harmony,
early-1960s American folk-group sound associated with The New Christy Minstrels.
It uses synthesized string-like plucks and a soft choir-like harmonic layer.
It does NOT contain recordings or samples of the group, and it is not an exact
emulation of their individual singers or arrangements.

Important
---------
This download is the plugin SOURCE PROJECT, not a precompiled .vst3 binary.
A Windows build needs JUCE and a C++ build toolchain. The current environment
did not have JUCE installed, so a ready-to-load binary could not be compiled here.

Build on Windows
----------------
1. Install Visual Studio 2022 (Desktop development with C++).
2. Download JUCE from https://juce.com/download/ and extract it, for example to C:\\JUCE.
3. Open "x64 Native Tools Command Prompt for VS 2022".
4. Change directory to this folder.
5. Run:
   cmake -S . -B build -DJUCE_DIR="C:/JUCE"
   cmake --build build --config Release

Depending on the JUCE installation, JUCE_DIR may need to point to the folder
containing JUCEConfig.cmake (often C:/JUCE/lib/cmake/JUCE).

The VST3 output should be under:
   build/MinstrelFolk_artefacts/Release/VST3/
Copy the resulting "Minstrel Folk.vst3" bundle to:
   C:\\Program Files\\Common Files\\VST3
Then rescan plugins in your DAW.

Sound/design notes
------------------
- Play chords rather than only single notes.
- Try a bright, close-position triad or a 6/9 folk chord.
- String / Choir controls the harmonic blend.
- Warmth softens the attack and adds thickness.
- Room adds a short vintage-room echo.

This is a first prototype, not a polished commercial instrument. The synthesis is
algorithmic and will sound more like a stylized folk synth than a realistic
multi-miked acoustic ensemble. A higher-fidelity version would use licensed
guitar/mandolin/banjo samples and purpose-built multi-part vocal samples.
