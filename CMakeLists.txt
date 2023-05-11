cmake_minimum_required(VERSION 3.24)
project(FirstPerson)

set(CMAKE_CXX_STANDARD 17)

include_directories(include /opt/homebrew/include/SDL2)
add_compile_options(-Wall)
add_library(console-game-engine
        include/SimpleGameEngine.h
        include/SimpleGameEngine.cpp)
target_link_libraries(console-game-engine -lSDL2)
target_link_libraries(console-game-engine -L/opt/homebrew/lib/)
add_executable(FirstPerson src/main.cpp)
target_link_libraries(FirstPerson console-game-engine)