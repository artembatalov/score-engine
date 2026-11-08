# score-engine

A C++20 audio renderer that turns a plain-text score into a 44.1 kHz, 16-bit mono WAV. Define instruments, arrange notes into reusable patterns, and render the result from the command line.

## Demo

[Listen to Blue Hour (MP3)](examples/blue-hour.mp3?raw=true) · [Read the score](examples/blue-hour.txt)

A 19-second piece with a sine bass, tremolo chords, and a triangle lead with echo. The engine renders the WAV from the linked score. The MP3 is a compressed copy for listening.

## Build and render

Use CMake 3.24+ and a C++20 compiler. CMake downloads GoogleTest if it cannot find a local installation.

From the repository root:

```sh
cmake -S . -B build
cmake --build build --target score-engine
./build/bin/score-engine examples/blue-hour.txt blue-hour.wav
```

## Score format

A score sets the tempo in BPM and starts playback from the `main` pattern. In the example below, `resolution 4` gives four ticks per beat. Each note lists its start tick, instrument, pitch, duration in ticks, and velocity as a percentage. The `@` prefix plays a named pattern at the given tick.

```text
bpm 112

instrument lead triangle
  attack=360 release=1900
  effect echo delay=0.402 decay=0.26
end

pattern motif resolution 4
  0 lead E5 3 29
  4 lead A5 2 21
  7 lead C6 3 26
end

pattern main resolution 4
  0 @motif
  16 @motif
end
```

The renderer supports WAV samples and sine, square, and triangle oscillators. Instruments have attack and release envelopes in samples and can use gain, echo, and tremolo effects.
