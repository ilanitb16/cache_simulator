cmake_minimum_required(VERSION 3.0)
project(cache_simulation)

# Add the executable target
add_executable(cache_simulation cache.c main.c)

# Optionally, specify any additional compiler flags or settings
# For example:
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
