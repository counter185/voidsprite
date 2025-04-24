cmake_minimum_required(VERSION 3.15...4.0)

project(
    discord_game_sdk
    VERSION 1.0
    LANGUAGES CXX)

file(GLOB_RECURSE src *.cpp)

add_library(discord_game_sdk STATIC ${src})
target_sources(discord_game_sdk PRIVATE ${src})
