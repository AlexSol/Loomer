#ifndef CONFIG_H
#define CONFIG_H

#include <QJsonObject>

#include "mainwindow.h"

class Config {
private:
    MainWindow *mainWindow;

public:
    Config(){}
    void Read();

    // Config(MainWindow *mw) : mainWindow(mw) {}

    struct Settings {
        QJsonObject config_obj;
        quint16 server_port;
        QString server_ip;
    };

    static Settings settings;

};
#endif // CONFIG_H
