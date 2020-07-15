project ( CrossWindow )
cmake_minimum_required ( VERSION 3.1 )

set ( CMAKE_CXX_STANDARD 11 )
set ( CMAKE_CXX_STANDARD_REQUIRED ON )

find_package ( GLM REQUIRED )
list ( APPEND CROSS_WINDOW_INCLUDE_DIRS ${GLM_INCLUDE_DIRS} )

string ( TOUPPER ${CMAKE_SYSTEM_NAME} UPPERCASE_SYSTEM_NAME )
list ( APPEND CROSS_WINDOW_DEFINITIONS CROSS_WINDOW_PLATFORM_${UPPERCASE_SYSTEM_NAME} )
set ( CROSS_WINDOW_PLATFORM_${UPPERCASE_SYSTEM_NAME} TRUE )

find_package ( SDL2 QUIET )
if ( SDL2_FOUND )
    option ( CROSS_WINDOW_SDL2_SUPPORT "Add SDL2 backend support" TRUE )
endif()

if (( NOT TARGET Logger ) AND ( EXISTS ${CMAKE_CURRENT_LIST_DIR}/Logger ))
    add_subdirectory ( Logger )
    list ( APPEND CROSS_WINDOW_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} )
endif()
# if the logger project is present, use it
if ( TARGET Logger )
    list ( APPEND CROSS_WINDOW_LIBRARIES Logger )
    set ( CROSS_WINDOW_LOGGER_SUPPORT TRUE )
    message ( STATUS "CrossWindow - Building with Logger support")
endif()

file ( GLOB CROSS_WINDOW_SOURCES "*.cpp" "*.h" AstyleConfig.astylerc )
if ( CROSS_WINDOW_SDL2_SUPPORT )
    message ( STATUS "CrossWindow - Building with SDL2 support")
    file ( GLOB CROSS_WINDOW_SDL2_SOURCES "SDL2/*" )
    list ( APPEND CROSS_WINDOW_SOURCES ${CROSS_WINDOW_SDL2_SOURCES} )
    list ( APPEND CROSS_WINDOW_LIBRARIES ${SDL2_LIBRARIES} )
    list ( APPEND CROSS_WINDOW_INCLUDE_DIRS ${SDL2_INCLUDE_DIRS} )
    list ( APPEND CROSS_WINDOW_DEFINITIONS "-DCROSS_WINDOW_SDL2_SUPPORT" )
    source_group ( SDL2 FILES ${CROSS_WINDOW_SDL2_SOURCES} )
endif()

configure_file ( CrossWindowConfig.h.in ${CMAKE_CURRENT_LIST_DIR}/CrossWindowConfig.h )

if (( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" ) AND ( MINGW ))
    message ( STATUS "Building under MinGW. Copying DLL's to output folder")
    foreach ( LIBRARY gcc_s_seh-1 winpthread-1 stdc++-6 )
        find_library ( MINGW_DEPENDENCY_LIB
            NAMES
                ${LIBRARY}
            )
        file ( COPY ${MINGW_DEPENDENCY_LIB} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} )
    endforeach()
endif()

add_library ( CrossWindow ${CROSS_WINDOW_SOURCES} )
target_link_libraries ( CrossWindow ${CROSS_WINDOW_LIBRARIES} )
target_include_directories ( CrossWindow PUBLIC ${CROSS_WINDOW_INCLUDE_DIRS} )
target_compile_definitions ( CrossWindow PRIVATE ${CROSS_WINDOW_DEFINITIONS} )
target_warning_options ( CrossWindow HIGH AS_ERRORS )

add_executable ( CrossWindowTest Test/Test.cpp )
target_link_libraries ( CrossWindowTest CrossWindow )
target_include_directories ( CrossWindowTest PUBLIC ${CROSS_WINDOW_INCLUDE_DIRS} )
