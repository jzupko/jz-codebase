jz-codebase
===========
Archive of C++ code developed while working on my System for Automated Interactive
Lighting (SAIL) [dissertation](https://etda.libraries.psu.edu/catalog/9361)
work (2009).

Note that the codebase which was actually used as part of the interview
portion of my dissertation is the [sailcs-codebase](https://github.com/jzupko/sailcs-codebase).
That codebase is written in C#.

This C++ `jz-codebase` was built in tandum and is less complete in terms
of the implementation of SAIL itself (training and real-time lighting).

Licenses
--------
- original code is licensed under the
  [MIT License](http://www.opensource.org/licenses/mit-license.html) and
  is Copyright (c) 2009-2012 Joseph A. Zupko.
- Environment and character art in [`media/1930`](https://github.com/jzupko/jz-codebase/tree/main/media/1930)
  is used with permission and is Copyright (c) 2009 [David Milam](http://www.sfu.ca/~dma35/cv.html).

Third-party Licenses
--------------------
- files in [`jz/fmod`](https://github.com/jzupko/jz-codebase/tree/main/jz/fmod)
  are part of `FMOD 3.75` and covered by the proprietary [FMOD End User License Agreement](https://github.com/jzupko/jz-codebase/blob/main/jz/fmod/README.TXT#L36).
- files in [`jz/freetype2`](https://github.com/jzupko/jz-codebase/tree/main/jz/freetype2)
  are part of the `FreeType project` and are covered by the terms
  of the `FreeType project license`.
- files in [`jz/glew`](https://github.com/jzupko/jz-codebase/tree/main/jz/glew)
  are part of the `OpenGL Extension Wrangler Library` and are covered by the
  terms of the `3-Clause BSD License`.
- files in [`jz/lua`](https://github.com/jzupko/jz-codebase/tree/main/jz/lua)
  are part of `Lua 5.1.4` and are covered by the terms
  of the `MIT License`.
- files in [`jz/tut_framework`](https://github.com/jzupko/jz-codebase/tree/main/jz/tut_framework)
  are part of the `Template Unit Tests Framework for C++` and are covered by
  the terms of the `Simplified BSD License`.
- files in [`jz/zlib`](https://github.com/jzupko/jz-codebase/tree/main/jz/zlib)
  are part of the `'zlib' general purpose compression library version 1.2.3`
  and are covered by the terms of the `zlib License`.

Overview
--------
This codebase includes my personal projects as well as a C++ implementation of my
System for Automated Interactive Lighting (SAIL) [dissertation](https://etda.libraries.psu.edu/catalog/9361)
work.

This codebase is not well documented or thoroughly tested and should be considered
a snapshot of WIP when I was finishing up my dissertation. It is only known to
compile with `Visual Studio 2008 Express Edition` under `Windows XP 32-bit`.
The codebase should be considered 32-bit and Windows brittle.

Modules
-------
- [`jz/jz_ai`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_ai) - fuzzy logic and learning (neural network and simulated annealing).
- [`jz/jz_app_3D`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_app_3D) - sample implementation of `jz_engine_3D`, also a sample
              implementation of jz_sail.
- [`jz/jz_core`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_core) - core library, provides platform independent math and utility
            functions.
- [`jz/jz_engine_3D`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_engine_3D) - 3D engine, primitives based around scene nodes.
- [`jz/jz_filesystem`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_filesystem) - pack file filesystem, unfinished and unused.
- [`jz/jz_graphics`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_graphics) - low-level access to graphics device functionality.
- [`jz/jz_graphics_directx9`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_graphics_directx9) - DirectX 9 specific implementation of `jz_graphics`.
- [`jz/jz_graphics_opengl`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_graphics_opengl) - OpenGL specific implementation of `jz_graphics`,
                       unfinished and unused.
- [`jz/jz_pathfinding`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_pathfinding) - dynamic 2D pathfinding, unfinished and unused.
- [`jz/jz_physics`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_physics) - 2D and 3D collision.
- [`jz/jz_pipeline`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_pipeline) - content processor written in C#, converts [COLLADA](https://collada.org/mediawiki/index.php/Main_Page)
                files into content usable by `jz_engine_3D`.
- [`jz/jz_sail`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_sail) - System for Automated Interactive Lighting (SAIL), C++
            implementation of the solver described in my dissertation.
- [`jz/jz_sound`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_sound) - streaming audio and SFX playback, unfinished and unused.
- [`jz/jz_system`](https://github.com/jzupko/jz-codebase/tree/main/jz/jz_system) - system library, provides I/O, timing, threading, and other
              platform dependent functions.
