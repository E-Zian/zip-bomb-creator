cmake_minimum_required(VERSION 3.20)

project(zip_bomb_creator)

set(CMAKE_CXX_STANDARD 20)

set(sources
        src/main.cpp
        src/localFileHeader.cpp


)
set(includes
        include/LocalFileHeader.h
        src/Helper.cpp
        include/Helper.h
        src/EOCD.cpp
        include/Eocd.h
        src/CentralDirectory.cpp
        include/CentralDirectory.h

)

add_executable(zbc ${sources} ${includes})

target_include_directories(zbc PUBLIC
        include
)