cmake_minimum_required(VERSION 3.19)
project(Turing_machine LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Turing_machine
    WIN32 MACOSX_BUNDLE
    main.cpp
    turingmachine.cpp
    turingmachine.h
    turingmachine.ui
)

target_link_libraries(Turing_machine
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS Turing_machine
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Turing_machine
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
