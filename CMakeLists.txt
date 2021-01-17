cmake_minimum_required(VERSION 3.16)
project(game-of-life)

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(generated-files STATIC world.pb.cc world.pb.h)
target_link_libraries(generated-files ${CONAN_LIBS})

add_library(cellular-automaton STATIC
    Player.cpp Player.h
    Camera.cpp Camera.h
    Map.h
    MapController.h
    MapRenderer.h
    MapUtilities.h
)
target_link_libraries(cellular-automaton ${CONAN_LIBS} generated-files)

add_executable(game-of-life-client client.cpp)
target_link_libraries(game-of-life-client ${CONAN_LIBS} cellular-automaton generated-files)

add_executable(game-of-life-server server.cpp)
target_link_libraries(game-of-life-server ${CONAN_LIBS} generated-files)
