cmake_minimum_required(VERSION 3.20)

project(zip_bomb_creator)

set(CMAKE_CXX_STANDARD 20)

set(sources
        src/main.cpp


)
set(includes

)

add_executable(zbc ${sources} ${includes})

target_include_directories(zip_bomb_creator PUBLIC
        include
)