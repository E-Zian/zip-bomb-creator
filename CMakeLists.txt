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
        src/zip_file_construction/Crc32.cpp
        src/deflation/HuffmanTable.cpp
        src/factory/PayloadFactory.cpp
        src/deflation/Lz77.cpp
        src/deflation/HuffmanTree.cpp
        src/zip_file_construction/Zip64.cpp

)
set(includes
        include/zip_file_construction/LocalFileHeader.h
        include/Helper.h
        include/zip_file_construction/Eocd.h
        include/zip_file_construction/CentralDirectory.h
        include/zip_file_construction/ZipFile.h
        include/deflation/HuffmanTree.h
        include/deflation/BitWriter.h
        src/deflation/BitWriter.cpp
        include/deflation/HuffmanTable.h
        include/deflation/Lz77.h
        include/factory/PayloadFactory.h
        include/zip_file_construction/Crc32.h
        include/zip_file_construction/Zip64.h

)

add_executable(zbc ${sources} ${includes})

target_include_directories(zbc PUBLIC
        include
)