cmake_minimum_required(VERSION 3.20)

project(zip_bomb_creator)

set(CMAKE_CXX_STANDARD 20)

set(sources
        src/main.cpp
        src/zip_file_construction/LocalFileHeader.cpp
        src/Helper.cpp
        src/zip_file_construction/CentralDirectory.cpp
        src/zip_file_construction/Eocd.cpp
        src/zip_file_construction/ZipFile.cpp

)
set(includes
        include/zip_file_construction/LocalFileHeader.h
        include/Helper.h
        include/zip_file_construction/Eocd.h
        include/zip_file_construction/CentralDirectory.h
        include/zip_file_construction/ZipFile.h
        include/deflation/HuffmanTree.h
        src/deflation/HuffmanTree.cpp
        include/deflation/BitWriter.h
        src/deflation/BitWriter.cpp

)

add_executable(zbc ${sources} ${includes})

target_include_directories(zbc PUBLIC
        include
)