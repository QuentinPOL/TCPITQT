#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ServeurTCPIP.h"
#include <QTcpSocket>
#include <QTcpServer>

// Classe IHM
class ServeurTCPIP : public QMainWindow
{
    Q_OBJECT

public:
    ServeurTCPIP(QWidget *parent = Q_NULLPTR);
    ~ServeurTCPIP();
    QTcpServer* socketServer; // Création du socket Serveur

private:
    Ui::ServeurTCPIPClass ui;

public slots:
    void onServerNewConnection(); // Nouvelle connexion au serveur
    void onClientDisconnected(); // client signal déconnecter
    void onClientReadyRead(); // client signal readyRead
};
