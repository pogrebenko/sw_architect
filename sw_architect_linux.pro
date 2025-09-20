QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

qtHaveModule(printsupport): QT += printsupport

DEFINES += BUILDDATE=\\\"$$system( date "+%d.%m.%Y_%H:%M:%S" )\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/IOStreamer.cpp \
    common/Utils.cpp \
    data/History.cpp \
    data/Options.cpp \
    data/databases/SQLFactory.cpp \
    data/databases/SQLStreamer.cpp \
    data/databases/mysql.cpp \
    data/general/Class.cpp \
    data/general/Field.cpp \
    data/general/Relation.cpp \
    data/shapes/Ellipse.cpp \
    data/shapes/FigureFactory.cpp \
    data/shapes/Rectangle.cpp \
    data/shapes/Triangle.cpp \
    engine/Builder.cpp \
    ui/ClassDetails.cpp \
    ui/FieldDetails.cpp \
    ui/OptionDetails.cpp \
    ui/RelationDetails.cpp \
    ui/TableEdit.cpp \
    view/Painter.cpp \
    canvas.cpp \
    mainwindow.cpp \
    app.cpp \
    main.cpp

HEADERS += \
    common/Compare.h \
    common/Consts.h \
    common/IOStreamer.h \
    common/Logger.h \
    common/Structs.h \
    common/Utils.h \
    common/VectorOfPtr.h \
    data/History.h \
    data/Options.h \
    data/databases/SQLFactory.h \
    data/databases/SQLStreamer.h \
    data/databases/mysql.h \
    data/general/Class.h \
    data/general/Field.h \
    data/general/Relation.h \
    data/shapes/Ellipse.h \
    data/shapes/FigureFactory.h \
    data/shapes/Rectangle.h \
    data/shapes/Triangle.h \
    templates.h \
    ui/ClassDetails.h \
    ui/FieldDetails.h \
    ui/OptionDetails.h \
    ui/RelationDetails.h \
    ui/TableEdit.h \
    engine/Builder.h \
    view/Painter.h \
    app.h \
    canvas.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    LICENSE \
    README.md

RESOURCES += \
    sw_architect.qrc

#ICON = sw_architect.icns
