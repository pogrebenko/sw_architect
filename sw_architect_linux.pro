QT += core gui
QT += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

qtHaveModule(printsupport): QT += printsupport

DEFINES += BUILDDATE=\\\"$$system( date "+%d.%m.%Y_%H:%M:%S" )\\\"

INCLUDEPATH += /usr/include
LIBS += -lodbc -lsqlite3 -lmysqlclient

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
    psql/sptr.cpp \
    psql/sqlbribge.cpp \
    psql/sqlinfo.cpp \
    psql/sqlmanager.cpp \
    psql/sqlpipe.cpp \
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
    extern/linux/sql/mysql/client_plugin.h \
    extern/linux/sql/mysql/errmsg.h \
    extern/linux/sql/mysql/field_types.h \
    extern/linux/sql/mysql/my_command.h \
    extern/linux/sql/mysql/my_compress.h \
    extern/linux/sql/mysql/my_list.h \
    extern/linux/sql/mysql/mysql.h \
    extern/linux/sql/mysql/mysql_com.h \
    extern/linux/sql/mysql/mysql_time.h \
    extern/linux/sql/mysql/mysql_version.h \
    extern/linux/sql/mysql/mysqld_error.h \
    extern/linux/sql/mysql/mysqlx_ername.h \
    extern/linux/sql/mysql/mysqlx_error.h \
    extern/linux/sql/mysql/mysqlx_version.h \
    extern/linux/sql/mysql/plugin_auth_common.h \
    extern/linux/sql/mysql/udf_registration_types.h \
    extern/linux/sql/odbc/sql.h \
    extern/linux/sql/odbc/sqlext.h \
    extern/linux/sql/odbc/sqltypes.h \
    extern/linux/sql/odbc/sqlucode.h \
    extern/linux/sql/odbc/unixodbc.h \
    extern/linux/sql/sqlite/sqlite3.h \
    psql/sptr.h \
    psql/sqlbridge.h \
    psql/sqlinfo.h \
    psql/sqlmanager.h \
    psql/sqlpipe.h \
    psql/sqltype.h \
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
