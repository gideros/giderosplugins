QT += widget

TARGET = media
TEMPLATE = lib

INCLUDEPATH += ../../../Sdk/include
INCLUDEPATH += ./libpng
INCLUDEPATH += ./jpeglib
win32 {
INCLUDEPATH += "D:\\opencv\\build\\include" \
                   "D:\\opencv\\build\\include\\opencv" \
                   "D:\\opencv\\build\\include\\opencv2"
}

SOURCES += \
    mediabinder.cpp \
    media.cpp

HEADERS += \
    media.h \
    CImg.h \
    jpeglib/jpeglib.h \
    jpeglib/jmorecfg.h \
    jpeglib/jconfig.h \
    libpng/pngconf.h \
    libpng/pnglibconf.h \
    libpng/pngconf.h \
    libpng/png.h \
    gstdio.h \
    gpath.h

win32 {
LIBS += -L"../../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs -lpng -ljpeg
LIBS += -L"D:\\opencv\\build\\x86\\mingw\\install\\x86\\mingw\\bin" \
        -lopencv_core248 \
        -lopencv_imgproc248 \
        -lopencv_highgui248
QMAKE_LFLAGS = -enable-auto-import
}

macx {
LIBS += -L"../../../Sdk/lib/desktop" -llua -lgid -lgideros -lgvfs
LIBS += -ljpeg -lpng
INCLUDEPATH += /usr/local/include/opencv

LIBS += -L/usr/local/lib/ \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc
QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
}

DEFINES += GIDEROS_LIBRARY

macx {
QMAKE_POST_LINK += install_name_tool -change liblua.1.dylib "@executable_path/../Frameworks/liblua.1.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change liblua.1.dylib "@executable_path/../Frameworks/liblua.1.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change libgid.1.dylib "@executable_path/../Frameworks/libgid.1.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change libgideros.1.dylib "@executable_path/../Frameworks/libgideros.1.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change libgvfs.1.dylib "@executable_path/../Frameworks/libgvfs.1.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change /Users/ar2rsawseen/QtSDK/Desktop/Qt/4.8.1/gcc/lib/QtCore.framework/Versions/4/QtCore "@executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change /Users/ar2rsawseen/QtSDK/Desktop/Qt/4.8.1/gcc/lib/QtGui.framework/Versions/4/QtGui "@executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change /usr/local/lib/libjpeg.9.dylib "@executable_path/../Frameworks/libjpeg.9.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change /usr/local/libpng-1.6.8/lib/libpng16.16.dylib "@executable_path/../Frameworks/libpng16.16.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change lib/libopencv_core.2.4.dylib "@executable_path/../../../Plugins/libopencv_core.2.4.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change lib/libopencv_highgui.2.4.dylib "@executable_path/../../../Plugins/libopencv_highgui.2.4.dylib" $(TARGET);
QMAKE_POST_LINK += install_name_tool -change lib/libopencv_imgproc.2.4.dylib "@executable_path/../../../Plugins/libopencv_imgproc.2.4.dylib" $(TARGET);
}
