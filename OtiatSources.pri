QT       += core gui 3dcore 3drender 3dinput 3dextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./QtAwesome/QtAwesome.pri)

SOURCES += \
    $$PWD/src/main/view/mainwindow.cpp \
    $$PWD/src/main/controller/maincontroller.cpp \
    $$PWD/src/main/model/objectimagecorrespondence.cpp \
    $$PWD/src/main/model/image.cpp \
    $$PWD/src/main/model/objectmodel.cpp \
    $$PWD/src/main/model/loadandstorestrategy.cpp \
    $$PWD/src/main/model/textfileloadandstorestrategy.cpp \
    $$PWD/src/main/model/cachingmodelmanager.cpp \
    $$PWD/src/main/model/modelmanager.cpp \
    $$PWD/src/main/view/breadcrumb/breadcrumbview.cpp \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.cpp \
    $$PWD/src/main/view/gallery/gallery.cpp \
    $$PWD/src/main/view/gallery/galleryimagemodel.cpp \
    $$PWD/src/main/view/gallery/galleryobjectmodelmodel.cpp \
    $$PWD/src/main/view/gallery/iconexpandinglistview.cpp \
    $$PWD/src/main/view/aboutdialog/aboutdialog.cpp \
    $$PWD/src/main/view/settings/settingsdialog.cpp \
    $$PWD/src/main/view/settings/settingsgeneralpage.cpp \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.cpp \
    $$PWD/src/main/view/settings/settingsitem.cpp \
    $$PWD/src/main/view/settings/settingsdialogdelegate.cpp \
    $$PWD/src/main/view/rendering/objectmodelrenderable.cpp \
    $$PWD/src/main/view/rendering/imagerenderable.cpp \
    $$PWD/src/main/view/correspondenceviewer/correspondenceviewer.cpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.cpp

HEADERS  += \
    $$PWD/src/main/model/cachingmodelmanager.hpp \
    $$PWD/src/main/model/image.hpp \
    $$PWD/src/main/model/loadandstorestrategy.hpp \
    $$PWD/src/main/model/modelmanager.hpp \
    $$PWD/src/main/model/objectimagecorrespondence.hpp \
    $$PWD/src/main/model/objectmodel.hpp \
    $$PWD/src/main/model/textfileloadandstorestrategy.hpp \
    $$PWD/src/main/controller/maincontroller.hpp \
    $$PWD/src/main/view/mainwindow.hpp \
    $$PWD/src/main/misc/otiathelper.h \
    $$PWD/src/main/view/correspondenceviewer/correspondenceviewer.hpp \
    $$PWD/src/main/misc/globaltypedefs.h \
    $$PWD/src/main/view/aboutdialog/aboutdialog.hpp \
    $$PWD/src/main/view/breadcrumb/breadcrumbview.hpp \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.hpp \
    $$PWD/src/main/view/gallery/gallery.hpp \
    $$PWD/src/main/view/gallery/galleryimagemodel.hpp \
    $$PWD/src/main/view/gallery/galleryobjectmodelmodel.hpp \
    $$PWD/src/main/view/gallery/iconexpandinglistview.hpp \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.hpp \
    $$PWD/src/main/view/rendering/imagerenderable.hpp \
    $$PWD/src/main/view/rendering/objectmodelrenderable.hpp \
    $$PWD/src/main/view/settings/settingsdialog.hpp \
    $$PWD/src/main/view/settings/settingsdialogdelegate.hpp \
    $$PWD/src/main/view/settings/settingsgeneralpage.hpp \
    $$PWD/src/main/view/settings/settingsitem.hpp \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.hpp

FORMS    += \
    $$PWD/src/main/view/mainwindow.ui \
    $$PWD/src/main/view/navigationcontrols/navigationcontrols.ui \
    $$PWD/src/main/view/gallery/gallery.ui \
    $$PWD/src/main/view/aboutdialog/aboutdialog.ui \
    $$PWD/src/main/view/settings/settingsdialog.ui \
    $$PWD/src/main/view/settings/settingsgeneralpage.ui \
    $$PWD/src/main/view/settings/settingssegmentationcodespage.ui \
    $$PWD/src/main/view/correspondenceviewer/correspondenceviewer.ui \
    $$PWD/src/main/view/correspondenceeditor/correspondenceeditor.ui

INCLUDEPATH += \
    $$PWD/src/main/view/breadcrumb \
    $$PWD/src/main/view/navigationcontrols \
    $$PWD/src/main/view/gallery \
    $$PWD/src/main/view/correspondenceviewer \
    $$PWD/src/main \
    $$PWD/src/main/misc \
    $$PWD/src/main/controller \
    $$PWD/src/main/view/settings \
    /usr/local/include/opencv

LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d
