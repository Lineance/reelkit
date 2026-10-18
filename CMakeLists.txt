cmake_minimum_required(VERSION 3.20)
project(editor_core_python VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Python 3.10 REQUIRED COMPONENTS Interpreter Development.Module)
execute_process(
    COMMAND "${Python_EXECUTABLE}" -m pybind11 --cmakedir
    OUTPUT_VARIABLE PYBIND11_CMAKE_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE PYBIND11_CMAKE_DIR_RESULT
)
if(PYBIND11_CMAKE_DIR_RESULT EQUAL 0)
    list(PREPEND CMAKE_PREFIX_PATH "${PYBIND11_CMAKE_DIR}")
endif()
find_package(pybind11 CONFIG REQUIRED)
find_package(EditorCore CONFIG REQUIRED)

pybind11_add_module(_native MODULE src/bindings.cpp)
target_link_libraries(_native PRIVATE editor_core::editor_core)
install(TARGETS _native LIBRARY DESTINATION editor_core)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/python/editor_core")
configure_file(python/editor_core/__init__.py
               "${CMAKE_CURRENT_BINARY_DIR}/python/editor_core/__init__.py" COPYONLY)
set_target_properties(_native PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/python/editor_core"
)

include(CTest)
if(BUILD_TESTING)
    find_program(FFMPEG_EXECUTABLE ffmpeg REQUIRED)
    find_program(FFPROBE_EXECUTABLE ffprobe REQUIRED)
    add_test(NAME python_render_pipeline
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_pipeline.py
    )
    set_tests_properties(python_render_pipeline PROPERTIES
        ENVIRONMENT "PYTHONPATH=${CMAKE_CURRENT_BINARY_DIR}/python;EDITOR_CORE_TEST_FFMPEG=${FFMPEG_EXECUTABLE};EDITOR_CORE_TEST_FFPROBE=${FFPROBE_EXECUTABLE}"
        TIMEOUT 120
        RUN_SERIAL TRUE
    )
endif()
