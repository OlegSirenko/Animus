cmake_minimum_required(VERSION 3.5)

project(UAV_Gst_v1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network Multimedia)


find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTLIBS REQUIRED
        gobject-2.0
        glib-2.0
        gstreamer-webrtc-1.0
        gstreamer-sdp-1.0
        gstreamer-pbutils-1.0
        libsoup-2.4
        json-glib-1.0
        gstreamer-rtsp-server-1.0
        gstreamer-check-1.0
        gstreamer-app-1.0)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        joypad.cpp
        joypad.h
        worker.cpp
        worker.h
#        planeview.cpp
#        planeview.h
#        planeview.ui
        images.qrc
        README.md
)

include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include
                    /usr/include/glib-2.0
                    /usr/include/x86_64-linux-gnu
                    /usr/include/gstreamer-1.0)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(UAV_Gst_v1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET UAV_Gst_v1 APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(UAV_Gst_v1 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(UAV_Gst_v1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

link_directories(${GSTLIBS_LIBRARY_DIRS})

target_link_libraries(UAV_Gst_v1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(UAV_Gst_v1 PRIVATE ${GSTLIBS_LIBRARIES})
set_target_properties(UAV_Gst_v1 PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS UAV_Gst_v1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(UAV_Gst_v1)
endif()
