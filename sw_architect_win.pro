QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

qtHaveModule(printsupport): QT += printsupport

#DEFINES += BUILDDATE=\\\"$$system( date "+%d.%m.%Y_%H:%M:%S" )\\\"
DEFINES += BUILDDATE=__DATE__/__TIME__

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    app.cpp \
    canvas.cpp \
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
    main.cpp \
    mainwindow.cpp \
    ui/ClassDetails.cpp \
    ui/FieldDetails.cpp \
    ui/OptionDetails.cpp \
    ui/RelationDetails.cpp \
    ui/TableEdit.cpp \
    view/Painter.cpp

HEADERS += \
    app.h \
    canvas.h \
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
    engine/Builder.h \
    mainwindow.h \
    templates.h \
    ui/ClassDetails.h \
    ui/FieldDetails.h \
    ui/OptionDetails.h \
    ui/RelationDetails.h \
    ui/TableEdit.h \
    view/Painter.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/adjustments-horizontal.svg \
    images/arrow-down-tray-24x24.svg \
    images/arrow-down-tray.svg \
    images/arrow-path.svg \
    images/arrow-right-start-on-rectangle.svg \
    images/arrow-up-tray.svg \
    images/arrow-uturn-left.svg \
    images/arrow-uturn-right.svg \
    images/arrows-pointing-out.svg \
    images/bars-3-center-center.svg \
    images/bars-3-center-down-16x16.svg \
    images/bars-3-center-left-16x16.svg \
    images/bars-3-center-left.svg \
    images/bars-3-center-right-16x16.svg \
    images/bars-3-center-right.svg \
    images/bars-3-center-up-16x16.svg \
    images/check.svg \
    images/circle-stack.svg \
    images/clipboard-16x16.svg \
    images/clipboard-24x24.svg \
    images/clipboard-document-16x16.svg \
    images/clipboard-document-24x24.svg \
    images/cloud-arrow-down.svg \
    images/cloud-arrow-up.svg \
    images/cog-6-tooth-16x16.svg \
    images/cog-6-tooth.svg \
    images/cursor-arrow-ripple.svg \
    images/database.svg \
    images/document.svg \
    images/exclamation-triangle.svg \
    images/home.svg \
    images/key.svg \
    images/lock-closed.svg \
    images/lock-open.svg \
    images/magnifying-glass-minus-16x16.svg \
    images/magnifying-glass-minus.svg \
    images/magnifying-glass-plus-16x16.svg \
    images/magnifying-glass-plus.svg \
    images/magnifying-glass.svg \
    images/paper-clip.svg \
    images/pencil.svg \
    images/plus-circle.svg \
    images/printer.svg \
    images/rectangle-group.svg \
    images/rocket-launch.svg \
    images/share.svg \
    images/square-3-stack-3d.svg \
    images/squaresplus.svg \
    images/star.svg \
    images/swatch.svg \
    images/trash.svg \
    images/x-circle.svg \
    images/x-mark.svg

RESOURCES += \
    sw_architect.qrc
