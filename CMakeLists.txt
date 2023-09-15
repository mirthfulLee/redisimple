cmake_minimum_required(VERSION 3.0)
project(redisimple VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR})

# chosen poller implementation
add_definitions(-DEPOLL_POLLER 1)

link_directories(redisimple)

file(GLOB SRC "redisimple/*.cc")

add_executable(${PROJECT_NAME} ${SRC})

# target_link_libraries(${PROJECT_NAME} redisimple)