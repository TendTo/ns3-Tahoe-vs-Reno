# Return early if no sources in the subdirectory
set(main_src p2p-project)
set(header_files simulation/tcp-tahoe simulation/node simulation/configuration simulation/tracer)
set(source_files ${main_src} ${header_files})
set(target_prefix scratch_P2P_)

get_filename_component(main_src ${main_src} ABSOLUTE)
get_filename_component(scratch_absolute_directory ${main_src} DIRECTORY)
string(REPLACE "${PROJECT_SOURCE_DIR}" "${CMAKE_OUTPUT_DIRECTORY}"
               scratch_directory ${scratch_absolute_directory}
)

build_exec(
        EXECNAME p2p-project
        EXECNAME_PREFIX ${target_prefix}
        SOURCE_FILES ${source_files}
        HEADER_FILES ${header_files}
        LIBRARIES_TO_LINK "${ns3-libs}" "${ns3-contrib-libs}"
        EXECUTABLE_DIRECTORY_PATH ${scratch_directory}/
)