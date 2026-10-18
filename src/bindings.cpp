#include <editor_core/editor_core.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl/filesystem.h>

namespace py = pybind11;

PYBIND11_MODULE(_native, module)
{
    module.doc() = "Python bindings for the editor-core MLT++ facade";
    py::register_exception<editor_core::Error>(module, "EditorCoreError");

    py::enum_<editor_core::ErrorCode>(module, "ErrorCode")
        .value("invalid_argument", editor_core::ErrorCode::InvalidArgument)
        .value("runtime_initialization", editor_core::ErrorCode::RuntimeInitialization)
        .value("missing_service", editor_core::ErrorCode::MissingService)
        .value("invalid_graph", editor_core::ErrorCode::InvalidGraph)
        .value("serialization_failure", editor_core::ErrorCode::SerializationFailure)
        .value("render_failure", editor_core::ErrorCode::RenderFailure)
        .value("unsupported_output", editor_core::ErrorCode::UnsupportedOutput);

    py::class_<editor_core::RuntimeOptions>(module, "RuntimeOptions")
        .def(py::init<>())
        .def_readwrite("module_directory", &editor_core::RuntimeOptions::moduleDirectory);

    py::class_<editor_core::Runtime>(module, "Runtime")
        .def(py::init<editor_core::RuntimeOptions>(), py::arg("options") = editor_core::RuntimeOptions{})
        .def("has_producer", &editor_core::Runtime::hasProducer, py::arg("service"))
        .def("has_filter", &editor_core::Runtime::hasFilter, py::arg("service"))
        .def("has_transition", &editor_core::Runtime::hasTransition, py::arg("service"))
        .def("has_consumer", &editor_core::Runtime::hasConsumer, py::arg("service"))
        .def("producers", &editor_core::Runtime::producers)
        .def("filters", &editor_core::Runtime::filters)
        .def("transitions", &editor_core::Runtime::transitions)
        .def("consumers", &editor_core::Runtime::consumers);

    py::class_<editor_core::RationalRate>(module, "RationalRate")
        .def(py::init<int, int>(), py::arg("numerator") = 30, py::arg("denominator") = 1)
        .def_readwrite("numerator", &editor_core::RationalRate::numerator)
        .def_readwrite("denominator", &editor_core::RationalRate::denominator)
        .def_property_readonly("value", &editor_core::RationalRate::asDouble);

    py::class_<editor_core::ProfileSpec>(module, "ProfileSpec")
        .def(py::init<>())
        .def_readwrite("width", &editor_core::ProfileSpec::width)
        .def_readwrite("height", &editor_core::ProfileSpec::height)
        .def_readwrite("frame_rate", &editor_core::ProfileSpec::frameRate)
        .def_readwrite("sample_aspect_numerator", &editor_core::ProfileSpec::sampleAspectNumerator)
        .def_readwrite("sample_aspect_denominator", &editor_core::ProfileSpec::sampleAspectDenominator)
        .def_readwrite("display_aspect_numerator", &editor_core::ProfileSpec::displayAspectNumerator)
        .def_readwrite("display_aspect_denominator", &editor_core::ProfileSpec::displayAspectDenominator)
        .def_readwrite("progressive", &editor_core::ProfileSpec::progressive)
        .def_readwrite("colorspace", &editor_core::ProfileSpec::colorspace);

    py::class_<editor_core::Profile>(module, "Profile")
        .def(py::init<const editor_core::Runtime &, editor_core::ProfileSpec>(),
             py::arg("runtime"), py::arg("settings") = editor_core::ProfileSpec{})
        .def_property_readonly("settings", &editor_core::Profile::settings,
                               py::return_value_policy::reference_internal)
        .def_property_readonly("fps", &editor_core::Profile::fps);

    py::enum_<editor_core::TrackKind>(module, "TrackKind")
        .value("video", editor_core::TrackKind::Video)
        .value("audio", editor_core::TrackKind::Audio)
        .value("overlay", editor_core::TrackKind::Overlay)
        .value("mixed", editor_core::TrackKind::Mixed);

    py::class_<editor_core::FrameRange>(module, "FrameRange")
        .def(py::init<editor_core::FrameCount, editor_core::FrameCount>(),
             py::arg("start"), py::arg("end"),
             "Create a zero-based half-open source range [start, end).")
        .def_readonly("start", &editor_core::FrameRange::start)
        .def_readonly("end", &editor_core::FrameRange::end)
        .def_property_readonly("duration", &editor_core::FrameRange::duration);

    py::class_<editor_core::TextSpec>(module, "TextSpec")
        .def(py::init<>())
        .def_readwrite("text", &editor_core::TextSpec::text)
        .def_readwrite("family", &editor_core::TextSpec::family)
        .def_readwrite("foreground", &editor_core::TextSpec::foreground)
        .def_readwrite("background", &editor_core::TextSpec::background)
        .def_readwrite("outline_color", &editor_core::TextSpec::outlineColor)
        .def_readwrite("size", &editor_core::TextSpec::size)
        .def_readwrite("weight", &editor_core::TextSpec::weight)
        .def_readwrite("outline", &editor_core::TextSpec::outline)
        .def_readwrite("padding", &editor_core::TextSpec::padding)
        .def_readwrite("opacity", &editor_core::TextSpec::opacity);

    py::class_<editor_core::RenderSpec>(module, "RenderSpec")
        .def(py::init<>())
        .def_readwrite("target", &editor_core::RenderSpec::target)
        .def_readwrite("muxer", &editor_core::RenderSpec::muxer)
        .def_readwrite("video_codec", &editor_core::RenderSpec::videoCodec)
        .def_readwrite("audio_codec", &editor_core::RenderSpec::audioCodec)
        .def_readwrite("pixel_format", &editor_core::RenderSpec::pixelFormat)
        .def_readwrite("mlt_audio_format", &editor_core::RenderSpec::mltAudioFormat)
        .def_readwrite("preset", &editor_core::RenderSpec::preset)
        .def_readwrite("quality", &editor_core::RenderSpec::quality)
        .def_readwrite("enable_video", &editor_core::RenderSpec::enableVideo)
        .def_readwrite("enable_audio", &editor_core::RenderSpec::enableAudio)
        .def_readwrite("consumer_properties", &editor_core::RenderSpec::consumerProperties)
        .def_static("mp4", &editor_core::RenderSpec::mp4, py::arg("target"))
        .def_static("m4a", &editor_core::RenderSpec::m4a, py::arg("target"))
        .def_static("float_wav", &editor_core::RenderSpec::floatWav, py::arg("target"));

    py::class_<editor_core::RenderProgress>(module, "RenderProgress")
        .def_readonly("completed_frames", &editor_core::RenderProgress::completedFrames)
        .def_readonly("total_frames", &editor_core::RenderProgress::totalFrames)
        .def_readonly("fraction", &editor_core::RenderProgress::fraction);

    py::class_<editor_core::RenderResult>(module, "RenderResult")
        .def_readonly("output", &editor_core::RenderResult::output)
        .def_readonly("frames", &editor_core::RenderResult::frames)
        .def_readonly("frame_rate", &editor_core::RenderResult::frameRate)
        .def_readonly("duration_seconds", &editor_core::RenderResult::durationSeconds)
        .def_readonly("cancelled", &editor_core::RenderResult::cancelled)
        .def_readonly("warnings", &editor_core::RenderResult::warnings);

    py::class_<editor_core::RenderJob>(module, "RenderJob")
        .def("start", &editor_core::RenderJob::start)
        .def("cancel", &editor_core::RenderJob::cancel)
        .def("is_running", &editor_core::RenderJob::isRunning)
        .def("progress", &editor_core::RenderJob::progress)
        .def("wait", [](editor_core::RenderJob &job) {
            py::gil_scoped_release release;
            return job.wait();
        });

    py::class_<editor_core::Track>(module, "Track")
        .def_property_readonly("name", &editor_core::Track::name)
        .def_property_readonly("index", &editor_core::Track::index)
        .def_property_readonly("duration_frames", &editor_core::Track::durationFrames)
        .def("append_file", &editor_core::Track::appendFile,
             py::arg("path"), py::arg("source_range"))
        .def("blank", &editor_core::Track::blank, py::arg("frames"))
        .def("dissolve", &editor_core::Track::dissolve,
             py::arg("first_clip"), py::arg("duration_frames"))
        .def("add_text", [](editor_core::Track &track, const editor_core::TextSpec &text) {
            track.addText(text);
        }, py::arg("text"));

    py::class_<editor_core::Timeline>(module, "Timeline")
        .def(py::init<const editor_core::Profile &>(), py::arg("profile"))
        .def("add_track", &editor_core::Timeline::addTrack,
             py::arg("kind") = editor_core::TrackKind::Video, py::arg("name") = std::string{})
        .def("duration_frames", &editor_core::Timeline::durationFrames)
        .def("save_mlt_xml", &editor_core::Timeline::saveMltXml, py::arg("path"))
        .def("start_render", &editor_core::Timeline::startRender, py::arg("spec"))
        .def("render", [](const editor_core::Timeline &timeline,
                           const editor_core::RenderSpec &spec) {
            py::gil_scoped_release release;
            return timeline.render(spec);
        }, py::arg("spec"));

    py::class_<editor_core::MltDocument>(module, "MltDocument")
        .def_static("load", &editor_core::MltDocument::load,
                    py::arg("profile"), py::arg("path"))
        .def_property_readonly("duration_frames", &editor_core::MltDocument::durationFrames)
        .def("save_mlt_xml", &editor_core::MltDocument::saveMltXml, py::arg("path"))
        .def("start_render", &editor_core::MltDocument::startRender, py::arg("spec"))
        .def("render", [](const editor_core::MltDocument &document,
                           const editor_core::RenderSpec &spec) {
            py::gil_scoped_release release;
            return document.render(spec);
        }, py::arg("spec"));
}
