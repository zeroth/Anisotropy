QT += gui qml quick widgets xml concurrent network quickcontrols2
DESTDIR = $$PWD/../Release/

#DEFINES += PHTRANSLATE_STATICLIB
QMAKE_CXXFLAGS += /std:c++17
DEFINES += ZEROTH_USE_QT
HEADERS += \
    Bio2DImageView.h \
    BioImage.h \
    BioImageProvider.h \
    Anisotropy.h \
    BioImageManager.h \
    ColorTable.h \
    Common.h \
    DataModel.h \
    TiffReader.h \
    TiffWriter.h

SOURCES += \
    Bio2DImageView.cpp \
    BioImage.cpp \
    BioImageProvider.cpp \
    Anisotropy.cpp \
    BioImageManager.cpp \
    ColorTable.cpp \
    Common.cpp \
    DataModel.cpp \
    TiffReader.cpp \
    TiffWriter.cpp \
    main.cpp
    

RESOURCES += \
    qml.qrc


