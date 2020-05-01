cmake_minimum_required(VERSION 3.5)

project(Anisotropy LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(Qt5 COMPONENTS Core Quick Concurrent REQUIRED)
find_package(Qt5Charts)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)
add_definitions(-DZEROTH_USE_QT)

add_executable(Anisotropy
    Bio2DImageView.cpp
    Bio2DImageView.h

    BioImage.cpp
    BioImage.h

    BioImageProvider.cpp
    BioImageProvider.h

    Anisotropy.cpp
    Anisotropy.h

    BioImageManager.cpp
    BioImageManager.h

    ColorTable.cpp
    ColorTable.h

    Common.cpp
    Common.h
    TiffReader.cpp
    TiffReader.h
    TiffWriter.cpp
    TiffWriter.h
    main.cpp
    qml.qrc
    )


target_compile_definitions(Anisotropy
    PRIVATE QT_QML_DEBUG)
target_link_libraries(Anisotropy
    PRIVATE Qt5::Core Qt5::Quick Qt5::Charts Qt5::Concurrent)
