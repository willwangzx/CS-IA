cmake_minimum_required(VERSION 3.21)

project(LibraryManagementSystem VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(BUILD_CONSOLE_APP "Build the console application" ON)
option(BUILD_GUI_APP "Build the SFML GUI application when dependencies are available" ON)
option(ENABLE_WARNINGS "Enable compiler warnings" ON)
option(BUILD_TESTING "Build test executables" ON)
set(LIBRARY_GUI_DEFAULT_FONT "" CACHE STRING "Optional default font path for the GUI application")

set(LIBRARY_COMMON_SOURCES
    src/Book.cpp
    src/LibraryManagementSystem.cpp
)

set(LIBRARY_COMMON_HEADERS
    src/Book.h
    src/LibraryManagementSystem.h
    src/RedBlackTree.h
)

add_library(library_core STATIC ${LIBRARY_COMMON_SOURCES} ${LIBRARY_COMMON_HEADERS})
add_library(LibraryManagementSystem::core ALIAS library_core)

target_include_directories(library_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(library_core PUBLIC cxx_std_17)

if(ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(library_core PRIVATE /W4 /permissive-)
    else()
        target_compile_options(library_core PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if(BUILD_CONSOLE_APP)
    add_executable(library_system src/main.cpp)
    target_link_libraries(library_system PRIVATE library_core)
    set_target_properties(library_system PROPERTIES OUTPUT_NAME library_system)

    install(TARGETS library_system RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

set(GUI_TARGET_BUILT FALSE)
if(BUILD_GUI_APP)
    find_package(SFML 3 QUIET COMPONENTS Graphics Window System)

    if(SFML_FOUND AND TARGET SFML::Graphics)
        add_executable(library_system_gui src/main_gui.cpp src/LibraryGUI.cpp)
        target_link_libraries(library_system_gui PRIVATE library_core SFML::Graphics)
        target_compile_definitions(library_system_gui PRIVATE LIBRARY_GUI_DEFAULT_FONT="$<IF:$<BOOL:${LIBRARY_GUI_DEFAULT_FONT}>,${LIBRARY_GUI_DEFAULT_FONT},>")
        set_target_properties(library_system_gui PROPERTIES OUTPUT_NAME library_system_gui)
        set(GUI_TARGET_BUILT TRUE)

        if(ENABLE_WARNINGS)
            if(MSVC)
                target_compile_options(library_system_gui PRIVATE /W4 /permissive-)
            else()
                target_compile_options(library_system_gui PRIVATE -Wall -Wextra -Wpedantic)
            endif()
        endif()

        if(WIN32)
            add_custom_command(TARGET library_system_gui POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_RUNTIME_DLLS:library_system_gui>
                    $<TARGET_FILE_DIR:library_system_gui>
                COMMAND_EXPAND_LISTS
            )
        endif()

        install(TARGETS library_system_gui RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    else()
        message(WARNING "SFML 3 was not found. The GUI target will not be generated. Install SFML 3, set SFML_DIR/CMAKE_PREFIX_PATH, or configure with -DBUILD_GUI_APP=OFF.")
    endif()
endif()

if(BUILD_TESTING)
    enable_testing()

    add_executable(library_unit_tests tests/library_unit_tests.cpp)
    target_link_libraries(library_unit_tests PRIVATE library_core)
    add_test(NAME library_unit_tests COMMAND library_unit_tests)

    add_executable(rbtree_stress_test tests/rbtree_stress_test.cpp)
    target_link_libraries(rbtree_stress_test PRIVATE library_core)

    add_executable(lms_bruteforce_test tests/lms_bruteforce_test.cpp)
    target_link_libraries(lms_bruteforce_test PRIVATE library_core)
endif()

install(TARGETS library_core
    EXPORT LibraryManagementSystemTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(FILES ${LIBRARY_COMMON_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/library-management-system)

install(EXPORT LibraryManagementSystemTargets
    FILE LibraryManagementSystemTargets.cmake
    NAMESPACE LibraryManagementSystem::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LibraryManagementSystem
)

set(CPACK_PACKAGE_NAME "LibraryManagementSystem")
set(CPACK_PACKAGE_VENDOR "OpenAI")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Console and optional SFML GUI library management system")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ")
endif()
include(CPack)
