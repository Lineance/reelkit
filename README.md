# editor-core-python

`editor-core-python` is a Python package over the C++ `editor-core` MLT++ facade. It is one callable capability library for Agent-generated Python pipelines, not the Agent, the whole publishing pipeline, or the monetization workflow.

The Python API exposes Runtime, Profile, Timeline, Track, FrameRange, RenderJob, RenderSpec, and MLT XML load/save. Its helpers call the C++ API; they do not maintain a second timeline model or serialize Python commands as a separate format. The Agent can choose this package alongside separate research, media-generation, and publishing tools.

## Build prerequisite

Build and install the C++ SDK first (currently tested with Ubuntu 24.04, MLT/MLT++ 7.22):

```bash
cmake -S ../editor-core -B ../editor-core/build -G Ninja
cmake --build ../editor-core/build
cmake --install ../editor-core/build --prefix "$HOME/.local"
```

Install the Python build dependencies and build/install this package:

```bash
python3 -m venv .venv
. .venv/bin/activate
python -m pip install --upgrade pip
python -m pip install scikit-build-core pybind11
CMAKE_PREFIX_PATH="$HOME/.local" python -m pip install .
```

The system must also have the MLT runtime modules and FFmpeg-backed MLT `avformat` support. This package does not bundle MLT or optional effect packs.

## Minimal render

```python
from editor_core import render_clip_with_title

result = render_clip_with_title(
    source="input.mp4",
    output="output.mp4",
    frame_count=300,
    title="Hello from Python",
)
print(result.output, result.frames, result.duration_seconds)
```

The lower-level Timeline/Track classes are also exported for scripts that need to build MLT graphs directly. Generic MLT XML can be loaded and rendered through `MltDocument`.

## Development test

```bash
cmake -S . -B build -G Ninja \
  -DCMAKE_PREFIX_PATH="$HOME/.local"
cmake --build build
ctest --test-dir build --output-on-failure
```

The end-to-end smoke test creates a synthetic source clip, constructs the timeline through the Python package, renders MP4 through the C++/MLT core, writes MLT XML, reloads it, and renders again.

Initial scope deliberately excludes Agent frameworks, monetization/publishing orchestration, Project JSON schema, and a large Triton-style deferred compiler DSL. The package exposes an ergonomic callable API for an Agent-generated pipeline; syntax can grow from actual use.
