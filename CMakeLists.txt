cmake_minimum_required(VERSION 3.5.0)
project(Gato-Juego VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Game/Source")
set(GAME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Game")
set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Game/Source/External/SDL/include")
set(SDL2_MIXER_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Game/Source/External/SDL_mixer/include")
set(SDL2_IMAGE_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Game/Source/External/SDL_image/include")
set(PUGIXML_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Game/Source/External/PugiXml/src")

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


#find_package(SDL2 REQUIRED)
#find_package(SDL2_mixer REQUIRED)
#find_package(SDL2_image REQUIRED)
#find_package(pugixml REQUIRED)

add_executable(Gato-Juego ${SOURCES})

target_include_directories(Gato-Juego PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_MIXER_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${PUGIXML_INCLUDE_DIRS})

#target_compile_options(${PROJECT_NAME} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>)
#target_compile_options(${PROJECT_NAME} PRIVATE $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>)

#target_link_libraries(citm ${SDL2_LIBRARIES} ${SDL2_MIXER_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
target_link_libraries(Gato-Juego ${CMAKE_CURRENT_SOURCE_DIR}/Game/Source/External/SDL/libx86/SDL2.lib ${CMAKE_CURRENT_SOURCE_DIR}/Output/SDL2.dll ${CMAKE_CURRENT_SOURCE_DIR}/Game/Source/External/SDL_mixer/libx86/SDL2.lib ${CMAKE_CURRENT_SOURCE_DIR}/Game/Source/External/SDL_image/libx86/SDL2.lib)

#target_compile_definitions(Gato-Juego PRIVATE uint32=unsigned\ __int32 uint64=unsigned\ __int64)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
