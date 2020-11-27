TARGET = 6DPAT

TEMPLATE = app

QT     += core gui widgets 3dcore 3dextras 3drender
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

# This check is only there for when you build the project with your
# own Qt libraries
lessThan(QT_MINOR_VERSION, 14) {
    error(Needs at least Qt >= 5.14 to run - found $$QT_VERSION.)
} else {
    lessThan(QT_MAJOR_VERSION, 5) {
        error(Needs at least Qt >= 5.14 to run - found $$QT_VERSION.)
    }
}

INCLUDEPATH += ../../../include/opencv4 \
            += ../../../include/qt3dwidget

LIBS += -L../../../lib/qt/lib -lQt5Core -lQt5Gui -lQt53DCore -lQt53DInput -lQt53DRender -lQt53DExtras \
        -L../../../lib/opencv4 -lopencv_core -lopencv_calib3d \
        -L../../../lib/qt3dwidget -lqt3dwidget

QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib/qt\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib/opencv4\'"
QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/lib/qt3dwidget\'"

include(controller/controller.pri)
include(misc/misc.pri)
include(model/model.pri)
include(settings/settings.pri)
include(view/view.pri)
include(3dparty/QtAwesome/QtAwesome/QtAwesome.pri)

SOURCES += main.cpp \

RESOURCES += resources/shaders/shaders.qrc \
             resources/images/images.qrc \
             resources/fonts/fonts.qrc \
             resources/stylesheets/stylesheets.qrc

