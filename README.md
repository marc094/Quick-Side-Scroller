# QSS - Quick Side Scroller

Started as a simplistic SDL side scroller for educational purposes and has turned into a 2D gravity sandbox:
thousands of bodies attract each other (Barnes-Hut quadtree) and merge when they collide.

![screenshot](/Docs/screenshot.png)

QSS is maintained at https://github.com/d0n3val/Quick-Side-Scroller

## Controls

| Input | Action |
|---|---|
| Space | Pause / resume |
| Q | Advance one step while paused |
| Numpad + / - | Increase / decrease time scale (E resets to 0.1) |
| Mouse wheel | Zoom |
| Middle mouse drag, WASD | Pan |
| Left mouse drag | Select a body to follow (shows velocity vector and trail) |
| Right mouse | Respawn a removed body at the cursor |
| B | Follow the heaviest body |
| F | Reset zoom |
| G (held) | Pull everything towards the screen centre |
| R | Reset |
| Esc | Quit |

## Building

Requires SDL2 development files (e.g. `apt install libsdl2-dev`).

```
cmake -S . -B build
cmake --build build
./build/qss
```

On Windows the Visual Studio solution (`Quick Side Scroller.sln`) also works.

Gravity uses a Barnes-Hut quadtree (`QuadTree.h`, accuracy set by `BH_THETA` in `Defs.h`) and OpenMP when available.
`--exact` switches to the O(n^2) reference and `--check-error` prints the tree's force error against it.

Options for headless smoke tests: `qss --run --frames N` starts unpaused, exits after N frames and prints
active body count, total mass and total momentum (the latter two must stay constant). Use
`SDL_VIDEODRIVER=dummy` to run without a display.

## Credits

Ricard Pillosu

## License

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
