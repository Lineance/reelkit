import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

import editor_core


class PythonRenderPipelineTest(unittest.TestCase):
    def test_python_facade_renders_and_round_trips_mlt(self):
        ffmpeg = os.environ.get("EDITOR_CORE_TEST_FFMPEG", "ffmpeg")
        with tempfile.TemporaryDirectory(prefix="editor-core-python-") as temp:
            root = Path(temp)
            source = root / "source.mp4"
            output = root / "titled.mp4"
            project = root / "titled.mlt"
            roundtrip = root / "roundtrip.mp4"

            subprocess.run(
                [
                    ffmpeg,
                    "-hide_banner",
                    "-loglevel",
                    "error",
                    "-y",
                    "-f",
                    "lavfi",
                    "-i",
                    "color=c=navy:s=640x360:r=30:d=2",
                    "-an",
                    "-c:v",
                    "libx264",
                    "-pix_fmt",
                    "yuv420p",
                    str(source),
                ],
                check=True,
            )

            runtime = editor_core.Runtime()
            self.assertTrue(runtime.has_producer("avformat"))
            settings = editor_core.ProfileSpec()
            settings.width = 640
            settings.height = 360
            settings.frame_rate = editor_core.RationalRate(30, 1)
            profile = editor_core.Profile(runtime, settings)

            timeline = editor_core.Timeline(profile)
            track = timeline.add_track(editor_core.TrackKind.video, "main")
            track.append_file(source, editor_core.FrameRange(0, 60))
            title = editor_core.TextSpec()
            title.text = "Python -> C++ -> MLT"
            title.size = 32
            track.add_text(title)

            spec = editor_core.RenderSpec.mp4(output)
            spec.enable_audio = False
            render = timeline.start_render(spec)
            render.start()
            self.assertTrue(render.is_running())
            result = render.wait()
            self.assertFalse(result.cancelled)
            self.assertEqual(result.frames, 60)
            self.assertEqual(result.frame_rate.numerator, 30)
            self.assertTrue(output.is_file())

            timeline.save_mlt_xml(project)
            loaded = editor_core.MltDocument.load(profile, project)
            self.assertEqual(loaded.duration_frames, 60)
            roundtrip_spec = editor_core.RenderSpec.mp4(roundtrip)
            roundtrip_spec.enable_audio = False
            roundtrip_result = loaded.render(roundtrip_spec)
            self.assertEqual(roundtrip_result.frames, 60)
            self.assertTrue(roundtrip.is_file())

            probe_tool = os.environ.get("EDITOR_CORE_TEST_FFPROBE", "ffprobe")
            probe = subprocess.run(
                [
                    probe_tool,
                    "-v",
                    "error",
                    "-show_entries",
                    "stream=codec_name,width,height,r_frame_rate,nb_frames",
                    "-of",
                    "json",
                    str(output),
                ],
                check=True,
                capture_output=True,
                text=True,
            )
            self.assertIn('"codec_name": "h264"', probe.stdout)
            self.assertIn('"width": 640', probe.stdout)
            self.assertIn('"height": 360', probe.stdout)
            self.assertIn('"nb_frames": "60"', probe.stdout)


if __name__ == "__main__":
    unittest.main()
