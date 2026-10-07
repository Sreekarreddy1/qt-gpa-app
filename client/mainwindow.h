#pragma once
#include <QMainWindow>
#include <QTcpSocket>
class QLineEdit; class QTextEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
private slots:
    void sendRequest();
    void onConnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError);
private:
    QLineEdit *hostEdit, *portEdit, *marksEdit;
    QTextEdit *output;
    QTcpSocket socket;
};