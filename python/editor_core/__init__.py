"""Python package for the editor-core MLT++ convenience API."""

from ._native import (
    EditorCoreError,
    ErrorCode,
    FrameRange,
    MltDocument,
    Profile,
    ProfileSpec,
    RationalRate,
    RenderJob,
    RenderProgress,
    RenderResult,
    RenderSpec,
    Runtime,
    RuntimeOptions,
    TextSpec,
    Timeline,
    Track,
    TrackKind,
)

__all__ = [
    "EditorCoreError",
    "ErrorCode",
    "FrameRange",
    "MltDocument",
    "Profile",
    "ProfileSpec",
    "RationalRate",
    "RenderJob",
    "RenderProgress",
    "RenderResult",
    "RenderSpec",
    "Runtime",
    "RuntimeOptions",
    "TextSpec",
    "Timeline",
    "Track",
    "TrackKind",
]


def render_clip_with_title(source, output, frame_count, title):
    """Build and render a minimal titled video through the C++/MLT core."""
    runtime = Runtime()
    settings = ProfileSpec()
    profile = Profile(runtime, settings)
    timeline = Timeline(profile)
    track = timeline.add_track(TrackKind.video, "main")
    track.append_file(source, FrameRange(0, frame_count))
    text = TextSpec()
    text.text = title
    text.size = 48
    track.add_text(text)
    spec = RenderSpec.mp4(output)
    spec.enable_audio = False
    return timeline.render(spec)


__all__.append("render_clip_with_title")
