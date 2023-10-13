#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ClientTCPIP.h"
#include <QTcpSocket>

// Classe IHM
class ClientTCPIP : public QMainWindow
{
    Q_OBJECT

public:
    ClientTCPIP(QWidget *parent = Q_NULLPTR);
    ~ClientTCPIP();

private:
    Ui::ClientTCPIPClass ui;
    QTcpSocket* socketClient; // Création du socket Client

public slots:
    void onConnectButtonClicked(); // Clic Bouton de connexion
    void onSocketConnected(); // socketClient signal connecter
    void onSocketDisconnected(); // socketClient signal déconnecter
    void onSendMessageButtonClicked(); // Clic Bouton d'envoie de message
    void onSocketReadyRead(); // socketClient signal readyRead
    void onSendCelsiusButtonClicked(); // Demande Celsius Button
    void onSendFarhenheitButtonClicked(); // Demande Farheneit Button
    void onSendHygrometrieButtonClicked(); // Demande Hygrometrie Button
};
