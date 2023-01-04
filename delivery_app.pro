QT += quick widgets
QT += quickcontrols2
QT += websockets
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#Mongodb src
INCLUDEPATH += /usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi
LIBS += -L/usr/local/lib -lmongocxx -lbsoncxx


SOURCES += \
        CourierToGotoMapRoute.cpp \
        CouriersMonitor.cpp \
        FileWatcher.cpp \
        Models/ClientsLocationsModel.cpp \
        Models/CouriersModel.cpp \
        Models/CouriersModelManager.cpp \
        Models/FilteredAndSortedGotosModel.cpp \
        Models/FiltredOnlineCouriersModel.cpp \
        Models/GotosModel.cpp \
        Models/MapViewedCouriersProxyModel.cpp \
        Models/MapViewedGotosProxyModel.cpp \
        MongoDb/MongoDBCollection.cpp \
        MongoDb/MongoDbPool.cpp \
        UiNotifier.cpp \
        WebSocket/WSClient.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CourierToGotoMapRoute.h \
    CouriersMonitor.h \
    FileWatcher.h \
    Models/ClientsLocationsModel.h \
    Models/CouriersModel.h \
    Models/CouriersModelManager.h \
    Models/FilteredAndSortedGotosModel.h \
    Models/FiltredOnlineCouriersModel.h \
    Models/GotosModel.h \
    Models/MapViewedCouriersProxyModel.h \
    Models/MapViewedGotosProxyModel.h \
    MongoDb/MongoDBCollection.h \
    MongoDb/MongoDbPool.h \
    UiHelper.h \
    UiNotifier.h \
    VObject.h \
    WebSocket/Tea.h \
    WebSocket/WSClient.h
