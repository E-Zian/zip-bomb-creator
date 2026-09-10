cmake_minimum_required(VERSION 3.20)

project(zip_bomb_creator)

set(CMAKE_CXX_STANDARD 20)

set(sources
        src/main.cpp
        src/localFileHeader.cpp
        src/Helper.cpp
        src/CentralDirectory.cpp
        src/Eocd.cpp
        src/ZipFile.cpp

)
set(includes
        include/LocalFileHeader.h
        include/Helper.h
        include/Eocd.h
        include/CentralDirectory.h
        include/ZipFile.h

)

add_executable(zbc ${sources} ${includes})

target_include_directories(zbc PUBLIC
        include
)