cmake_minimum_required(VERSION 3.5.0)
project(Gato-Juego VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Game/Source")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Output_Linux")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++2a  -l Box2D")

file(GLOB SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h")
list(APPEND SOURCES "${SOURCE_DIR}/External/Optick/include/optick.h")
file(GLOB_RECURSE PLAYER_SOURCES "${SOURCE_DIR}/States/*.cpp" "${SOURCE_DIR}/States/*.h")
list(APPEND SOURCES ${PLAYER_SOURCES})

include_directories(${SOURCE_DIR})

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(pugixml REQUIRED)

add_executable(Gato-Juego ${SOURCES})

include_directories(Gato-Juego ${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${PUGIXML_INCLUDE_DIRS})

#target_compile_options(${PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>)
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)
add_compile_options(-Winconsistent-missing-override)
target_link_libraries(Gato-Juego PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf pugixml::pugixml ${CMAKE_SOURCE_DIR}/Output_Linux/lib/libOptickCore.so)
#target_link_libraries(Gato-Juego /usr/lib/libSDL2main.a /usr/lib/libSDL2-2.0.so /usr/lib/libSDL2_mixer.so /usr/lib/libSDL2_image.so pugixml::pugixml)

#target_compile_definitions(Gato-Juego PRIVATE uint32=unsigned\ __int32 uint64=unsigned\ __int64)

configure_file("Output/config.xml" "${CMAKE_CURRENT_SOURCE_DIR}/Output_Linux/config.xml" COPYONLY)
configure_file("Output/save_game.xml" "${CMAKE_CURRENT_SOURCE_DIR}/Output_Linux/save_game.xml" COPYONLY)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Output/Assets ${EXECUTABLE_OUTPUT_PATH}/Assets
)
add_dependencies(Gato-Juego copy_assets)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
