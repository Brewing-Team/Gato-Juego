cmake_minimum_required(VERSION 3.5.0)
project(Gato-Juego VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Game/Source")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Output_Linux")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive -std=c++2a  -l Box2D")

#file(GLOB SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h")
file(GLOB SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.h")
#set(SOURCES
#"${SOURCE_DIR}/Main.cpp"
#"${SOURCE_DIR}/App.cpp"
#"${SOURCE_DIR}/App.h"
#"${SOURCE_DIR}/Audio.cpp"
#"${SOURCE_DIR}/Audio.h"
#"${SOURCE_DIR}/Defs.h"
#"${SOURCE_DIR}/DynArray.h"
#"${SOURCE_DIR}/Input.cpp"
#"${SOURCE_DIR}/Input.h"
#"${SOURCE_DIR}/List.h"
#"${SOURCE_DIR}/Module.h"
#"${SOURCE_DIR}/Point.h"
#"${SOURCE_DIR}/Queue.h"
#"${SOURCE_DIR}/Render.cpp"
#"${SOURCE_DIR}/Render.h"
#"${SOURCE_DIR}/Scene.cpp"
#"${SOURCE_DIR}/Scene.h"
#"${SOURCE_DIR}/SString.h"
#"${SOURCE_DIR}/Textures.cpp"
#"${SOURCE_DIR}/Textures.h"
#"${SOURCE_DIR}/Window.cpp"
#"${SOURCE_DIR}/Window.h"
#)


find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(pugixml REQUIRED)

add_executable(Gato-Juego ${SOURCES})

include_directories(Gato-Juego ${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${PUGIXML_INCLUDE_DIRS})

#target_compile_options(${PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>)
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

target_link_libraries(Gato-Juego PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer pugixml::pugixml)
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
