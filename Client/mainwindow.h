#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include<QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // void on_pushButton_clicked();
    void slotReadyRead();
    void on_pushButton_2_clicked();
    void on_lineEdit_returnPressed();
    // void getIdentificator();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_itemActivated(QListWidgetItem *item);

private:
    void SendToServer(const QString& str);
    void closeEvent(QCloseEvent* event) ;


    Ui::MainWindow *ui;
    QTcpSocket *socket;
    QByteArray Data;
    QString MySocket;
    QVector<QString> ActiveDialog;
    QString Interlocutor;

};
#endif // MAINWINDOW_H
