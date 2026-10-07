#include "mainwindow.h"
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

MainWindow::MainWindow() {
    auto *central = new QWidget;
    auto *layout = new QVBoxLayout(central);

    hostEdit  = new QLineEdit("127.0.0.1");
    portEdit  = new QLineEdit("5000");
    marksEdit = new QLineEdit("8 9 7.5");
    auto *btn = new QPushButton("Send to server");
    output = new QTextEdit; output->setReadOnly(true);

    layout->addWidget(new QLabel("Server host:")); layout->addWidget(hostEdit);
    layout->addWidget(new QLabel("Port:"));        layout->addWidget(portEdit);
    layout->addWidget(new QLabel("Marks (space separated):")); layout->addWidget(marksEdit);
    layout->addWidget(btn);
    layout->addWidget(output);
    setCentralWidget(central);
    setWindowTitle("GPA Client");

    connect(btn, &QPushButton::clicked, this, &MainWindow::sendRequest);
    connect(&socket, &QTcpSocket::connected, this, &MainWindow::onConnected);
    connect(&socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(&socket, &QAbstractSocket::errorOccurred, this, &MainWindow::onError);
}

void MainWindow::sendRequest() {
    output->append("Connecting...");
    socket.abort();
    socket.connectToHost(hostEdit->text(), portEdit->text().toUShort());
}

void MainWindow::onConnected() {
    socket.write((marksEdit->text() + "\n").toUtf8());
}

void MainWindow::onReadyRead() {
    output->append("Server: " + QString::fromUtf8(socket.readAll()).trimmed());
}

void MainWindow::onError(QAbstractSocket::SocketError) {
    output->append("Error: " + socket.errorString());
}