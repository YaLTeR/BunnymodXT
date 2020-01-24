set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
enable_language(Rust)
include(CMakeCargo)
cargo_build(NAME bxt-rs)
