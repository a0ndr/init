cmake_minimum_required(VERSION 3.30)
project(init)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(main src/main.cpp include/defines.h include/util.h
        src/singleton.cpp include/singleton.h
        src/unit.cpp include/unit.h
        src/unit_manager.cpp include/unit_manager.h
        src/dag.cpp include/dag.h
        src/cycle_detector.cpp include/cycle_detector.h
        src/reaper.cpp include/reaper.h
)

set_target_properties(main PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(main PROPERTIES LINK_SEARCH_END_STATIC 1)
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

target_include_directories(main PRIVATE include)

find_package(Microsoft.GSL CONFIG REQUIRED)
target_link_libraries(main PRIVATE Microsoft.GSL::GSL)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(main PRIVATE fmt::fmt-header-only)

find_path(SIMPLEINI_INCLUDE_DIRS "ConvertUTF.c")
target_include_directories(main PRIVATE ${SIMPLEINI_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS program_options algorithm filesystem)
target_link_libraries(main PRIVATE Boost::program_options Boost::algorithm Boost::filesystem)

target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
