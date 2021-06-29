project(dukto)
cmake_minimum_required(VERSION 3.1.0)

OPTION(USE_UPDATER "Add updater for application" OFF)
OPTION(USE_SINGLE_APP "Enable only single instance" OFF)
OPTION(USE_NOTIFY_NATIVE_TRAY "For notifications use System Tray's Native" OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

if(UNIX)
    FIND_PACKAGE(PkgConfig)
    if(PKG_CONFIG_FOUND)
        PKG_CHECK_MODULES(LIBNOTIFY libnotify)
        if(LIBNOTIFY_FOUND)
            OPTION(USE_NOTIFY_LIBNOTIFY "For notifications use libnotify" OFF)
        endif(LIBNOTIFY_FOUND)
    endif(PKG_CONFIG_FOUND)
endif(UNIX)

if(USE_UPDATER)
    add_definitions("-DUPDATER")
endif()
if(USE_SINGLE_APP)
    add_definitions("-DSINGLE_APP")
endif()
if(USE_NOTIFY_NATIVE_TRAY)
    add_definitions("-DNOTIFY_NATIVE_TRAY")
endif()
if(UNIX)
    if(USE_NOTIFY_LIBNOTIFY)
        add_definitions("-DNOTIFY_LIBNOTIFY")
        include_directories(${LIBNOTIFY_INCLUDE_DIRS})
        list(APPEND DUKTO_LIBS ${LIBNOTIFY_LIBRARIES})
    endif(USE_NOTIFY_LIBNOTIFY)
endif(UNIX)

if(NOT QT_DEFAULT_MAJOR_VERSION)
    find_package(Qt6 QUIET COMPONENTS Core)
    if (NOT Qt6_FOUND)
        find_package(Qt5 COMPONENTS Core REQUIRED)
        set(QT_DEFAULT_MAJOR_VERSION 5)
    else()
        set(QT_DEFAULT_MAJOR_VERSION 6)
    endif(NOT Qt6_FOUND)
endif(NOT QT_DEFAULT_MAJOR_VERSION)

set(QT_COMPONENTS Gui Network Qml QuickWidgets)
set(QT_LIBRARIES "")
foreach(temp ${QT_COMPONENTS})
    list(APPEND QT_LIBRARIES "Qt${QT_DEFAULT_MAJOR_VERSION}::${temp}")
endforeach()

find_package(Qt${QT_DEFAULT_MAJOR_VERSION} COMPONENTS ${QT_COMPONENTS} REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions("-DQT_NO_DEBUG_OUTPUT")
endif()

add_definitions("-DUNICODE")

set(DUKTO_HDR
    buddylistitemmodel.h
    destinationbuddy.h
    duktoprotocol.h
    duktowindow.h
    guibehind.h
    ipaddressitemmodel.h
    peer.h
    platform.h
    recentlistitemmodel.h
    settings.h
    systemtray.h
    theme.h
)

set(DUKTO_SRC
    buddylistitemmodel.cpp
    destinationbuddy.cpp
    duktoprotocol.cpp
    duktowindow.cpp
    guibehind.cpp
    ipaddressitemmodel.cpp
    main.cpp
    platform.cpp
    recentlistitemmodel.cpp
    settings.cpp
    systemtray.cpp
    theme.cpp
)

set(DUKTO_RESOURCES
    qml.qrc
    qml/common/common.qrc
)
if(Qt6_FOUND OR Qt5Core_VERSION VERSION_GREATER "5.15.0" OR Qt5Core_VERSION VERSION_EQUAL "5.15.0")
    list(APPEND DUKTO_RESOURCES "qml/new/main.qrc")
else()
    list(APPEND DUKTO_RESOURCES "qml/old/main.qrc")
endif()

if(USE_UPDATER)
    list(APPEND DUKTO_SRC updateschecker.cpp)
    list(APPEND DUKTO_HDR updateschecker.h)
endif()

if(WIN32)
    list(APPEND DUKTO_SRC ecwin7.cpp)
    list(APPEND DUKTO_HDR ecwin7.h)
    list(APPEND DUKTO_RESOURCES dukto.rc)
endif()

if(USE_SINGLE_APP)
    set(QAPPLICATION_CLASS "QApplication")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/SingleApplication)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/SingleApplication)
endif()

add_executable(${PROJECT_NAME}
                ${DUKTO_HDR}
                ${DUKTO_SRC}
                ${DUKTO_RESOURCES})

if(USE_SINGLE_APP)
    add_dependencies(${PROJECT_NAME} SingleApplication)
    link_directories(${CMAKE_CURRENT_BINARY_DIR}/modules/SingleApplication)
    target_link_libraries(${PROJECT_NAME} SingleApplication)
endif()

target_link_libraries(${PROJECT_NAME} ${DUKTO_LIBS} ${QT_LIBRARIES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} Ws2_32 ole32 user32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # Visual Studio C++
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # gcc
        target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
    endif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
endif(WIN32)

If (APPLE)
    SET( MACOSX_BUNDLE_ICON_FILE dukto.icns )
    SET_SOURCE_FILES_PROPERTIES( "dukto.icns" PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
    target_sources(${PROJECT_NAME} PRIVATE ${MACOSX_BUNDLE_ICON_FILE})
    set_property(TARGET ${PROJECT_NAME} PROPERTY MACOSX_BUNDLE 1)
    find_library(CORE_SERVICES CoreServices REQUIRED)
    target_link_options(${PROJECT_NAME} PRIVATE "${CORE_SERVICES}/CoreServices.tbd")
endif()

if(UNIX AND NOT APPLE)
    install(TARGETS ${PROJECT_NAME}
            DESTINATION bin)
    install(FILES dukto.png
            DESTINATION share/pixmaps/)
    install(FILES dukto.desktop
            DESTINATION share/applications/)
endif()
