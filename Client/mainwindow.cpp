#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidget>
#include <QStringBuilder>
#include <QDir>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

#include <enums.h>

Config::Settings Config::settings;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    socket = new QTcpSocket(this);

    Config config;
    config.Read();
     // read from config
    socket->connectToHost(Config::settings.server_ip, Config::settings.server_port);

    qDebug() << QCoreApplication::applicationDirPath();


    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);

    this->setStyleSheet(Style_Sheete());

    // Config config;
    //config.Read();
    //



    QIcon buton_icon(Get_Path(Directorys::IMAGED, Files::BUTON));
    ui->pushButton->setIcon(buton_icon);

    ui->listWidget_2->setSpacing(7);

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::slotReadyRead() {
    QDataStream in(socket);

    in.setVersion(QDataStream::Qt_6_0);

    if (in.status() == QDataStream::Ok) {
        QString str;
        in >> str;

        QStringList parts = str.split(",");
        int messType = parts[0].toInt();

        switch (messType) {

        case ID_MY: // my_identifier
        {
            MySocket = parts[2];
            QString num = parts[2];
            setWindowTitle(num);
            qDebug() << "My socket" << MySocket;
            break;
        }

        case ID_CLIENT: // the_identifier
        {
            if (!Sockets.contains(parts[2])) {
                Sockets.push_back(parts[2]);
                MainWindow::Socket_print();
            }

            parts.clear();
            break;
        }

        case ID_DELETE: // delete_client;
        {
            Sockets.removeAll(parts[2]);
            MainWindow::Socket_delete(parts[2]);
            break;
        }

        case MESAGE: // message
        {
            ui->listWidget_2->addItem(parts[3]);
            break;
        }
        }
    }
}

void MainWindow::SendToServer(const QString &str) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_0);
        out << str;
        socket->write(data);
        qDebug() << socket;
    } else {
        qDebug() << "Socket not connected";
    }
}

void MainWindow::on_lineEdit_returnPressed() {
    QString message = QString("%1,%2,%3,%4")
    .arg(MESAGE)
        .arg(Interlocutor)
        .arg(MySocket)
        .arg(ui->lineEdit->text());

    QListWidgetItem *item = new QListWidgetItem(ui->lineEdit->text());

    item->setTextAlignment(Qt::AlignRight);

    ui->listWidget_2->addItem(item);

    SendToServer(message);

    ui->lineEdit->clear();
}

void MainWindow::on_pushButton_clicked() { on_lineEdit_returnPressed(); }



void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QString num = item->text();
     item->setBackground(QBrush(QColor(QColorConstants::Svg::lightblue)));
    Interlocutor = num;
    ui->lineEdit->setFocus();

    for(int i = 0; i < ui->listWidget->count(); i++){
        QListWidgetItem *item = ui->listWidget->item(i);
        if(item->text() == num)item->setBackground(QBrush(QColor(QColorConstants::Svg::lightblue)));
        else item->setBackground(Qt::NoBrush); // Сбрасывает цвет фона
    }
}

void MainWindow::Socket_print() {
    for (auto i : Sockets) {
        QList<QListWidgetItem *> item =
            ui->listWidget->findItems(i, Qt::MatchExactly);
        if (item.isEmpty()) {
            QString appDir = QCoreApplication::applicationDirPath();
            QListWidgetItem *item =
                new QListWidgetItem(QIcon(Get_Path(Directorys::IMAGED, Files::USER)), i);
            ui->listWidget->setIconSize(QSize(25, 25));

            ui->listWidget->addItem(item);
        }
    }
}

void MainWindow::Socket_delete(QString socket_to_delete) {

    QList<QListWidgetItem *> items =
        ui->listWidget->findItems(socket_to_delete, Qt::MatchExactly);
    if (!items.isEmpty()) {
        for (QListWidgetItem *item : items) {
            int row = ui->listWidget->row(item); // Получить строку элемента
            delete ui->listWidget->takeItem(row);
        }
    }
}

QString MainWindow::Get_Path(Directorys target, Files directory) {
    QString path;
    QString file;
    QString extention;

    if (target == Directorys::IMAGED) {
        path = "images";
        extention = ".png";
    }
    if (target == Directorys::STYLES) {
        path = "styles";
        extention = ".qss";
    }
    if (target == Directorys::CONFIG) {
        file = "config";
        extention = ".json";        
    }
    if (directory == Files::USER) {
        file = "user";
    }
    if (directory == Files::BUTON) {
        file = "send";
    }
    if (directory == Files::STYLE) {
        file = "style";
    }

    QString appDir = QCoreApplication::applicationDirPath();
    QDir baseDir(appDir);
    baseDir.cdUp(); // Поднимаемся к папке build
    baseDir.cdUp(); // Поднимаемся к папке Client
    baseDir.cdUp();
    if (directory != Files::NON)
        baseDir.cd(path);

    QString filePath = baseDir.filePath(file + extention);
    qDebug() << "File path:" << filePath;

    return filePath;
}

QString MainWindow::Style_Sheete() {

    QString res = "" ;

    QFile styleFile(Get_Path(Directorys::STYLES, Files::STYLE)); // Убедитесь, что путь корректный
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        styleFile.close();
        res = styleSheet;

    }
    qDebug() << "Style File not open";

    return res;

}

void Config::Read() {
    QFile file(mainWindow->Get_Path(Directorys::CONFIG, Files::NON));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Error open config";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    // Парсим JSON
    QJsonDocument config_json = QJsonDocument::fromJson(data);

    if (config_json.isNull()) {
        qDebug() << "Error worck with config";
        return;
    }

    QJsonObject config_obj = config_json.object();

    Config::settings.server_ip =
        config_obj.value("Settings").toObject().value("server-ip").toString();
    Config::settings.server_port =
        config_obj.value("Settings").toObject().value("server-port").toInt();


}

