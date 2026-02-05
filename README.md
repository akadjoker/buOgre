# buOgre

A small Ogre3D + buLanguage sandbox for experimenting with rendering, bindings, and gameplay scripts.

## Requirements

- CMake 3.20+
- C++17 toolchain
- Ogre3D (with RTShaderSystem, Overlay, Terrain)
- SDL2
- OpenGL

## Build

```bash
cmake -S . -B build
cmake --build build -j
```

The executable is placed in `bin/` as `main`.

## Run

You can pass a script path, or let it fall back to the default script search paths.

```bash
./bin/main bin/scripts/sinbad_terrain.bu
```

Script search paths at runtime:
- `./bin`
- `./scripts`
- `./bin/scripts`
- `.`

## Scripts

Example scripts live in `bin/scripts/`.

- `bin/scripts/sinbad_terrain.bu`
- `bin/scripts/terrain_dacing.bu`

## Video

```text
https://www.youtube.com/watch?v=MK84TQXPrr4
```

## Screenshots

![Screenshot 1](images/image1.png)
![Screenshot 2](images/image2.png)
![Screenshot 3](images/image3.png)

## Notes

- Assets are expected under `assets/` and `media/` as referenced by the scripts.
- This project exposes a set of Ogre bindings to buLanguage for rapid prototyping.
