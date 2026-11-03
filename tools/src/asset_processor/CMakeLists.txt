file(GLOB_RECURSE sources *.cpp)

add_executable(asset_processor ${sources})
target_include_directories(asset_processor PRIVATE .)
# Where the other tools it runs are installed, relative to the repository root.
file(RELATIVE_PATH tools_bin_dir "${PROJECT_SOURCE_DIR}/.." "${CMAKE_INSTALL_PREFIX}/bin")
target_compile_definitions(asset_processor PRIVATE TOOLS_BIN_DIR="${tools_bin_dir}")
target_link_libraries(asset_processor PRIVATE project_settings)
target_link_libraries(asset_processor PRIVATE fmt::fmt nlohmann_json::nlohmann_json filesystem util)

install(TARGETS asset_processor RUNTIME DESTINATION bin)
