#include "ClientTCPIP.h"

// Constructeur IHM
ClientTCPIP::ClientTCPIP(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    // Rendre invisible et d�sactiver les boutons tant que la connexion n'est pas �tablie
    ui.pushBtnCelsius->setVisible(false);
    ui.pushBtnCelsius->setEnabled(false);

    ui.pushBtnFahrenheit->setVisible(false);
    ui.pushBtnFahrenheit->setEnabled(false);

    ui.pushBtnHygrometie->setVisible(false);
    ui.pushBtnHygrometie->setEnabled(false);

    socketClient = new QTcpSocket(this); // Inialisation du socket client pour notre interface (IHM)
    QObject::connect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected())); // On connecte nos signaux et slots avec le signal connected
    QObject::connect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected())); // On connecte nos signaux et slots avec le signal disconnected
    QObject::connect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead())); // On connecte nos signaux et slots avec le signal readyRead
}

// Destructeur IHM
ClientTCPIP::~ClientTCPIP()
{
    socketClient->close(); // On ferme la connexion
    delete socketClient; // On Supprime pour la m�moire
}

// M�thode du slot lors du clic sur le bouton de connexion
void ClientTCPIP::onConnectButtonClicked()
{
    // On r�cup�re ce qui a �t� saisi
    QString ip = ui.lineEditIP->text();
    QString port = ui.lineEditPort->text();

    // V�rifier si les champs ne sont pas pas vide
    if (!ip.isEmpty() && !port.isEmpty())
    {
        // On converti le port de String en int
        bool ok;
        int portAsInt = port.toInt(&ok);

        if (ok) // Si la conversion est r�ussie
        {
            // On va essayer de se connecter au serveur
            socketClient->connectToHost(ip, portAsInt);
        }
    }
    else
    {
        // le(s) champ(s) est vide -> Message d'erreur
        ui.label_error->setText("<font color='red'>Veuillez remplir les champs</font>");
    }
}

// M�thode du slot lorsque le signal connected est �mis par socketClient
void ClientTCPIP::onSocketConnected()
{
    ui.labelStatus->setText("Status connexion : Connect�");

    // Rendre visible et activer les boutons et effacer le texte d'erreur
    ui.pushBtnCelsius->setVisible(true);
    ui.pushBtnCelsius->setEnabled(true);

    ui.pushBtnFahrenheit->setVisible(true);
    ui.pushBtnFahrenheit->setEnabled(true);

    ui.pushBtnHygrometie->setVisible(true);
    ui.pushBtnHygrometie->setEnabled(true);

    ui.label_error->setText("");

    // d�sactiver et cacher le bouton connexion
    ui.pushBtnConnect->setEnabled(false);
    ui.pushBtnConnect->setVisible(false);

    // d�sactiver les labels ip et port
    ui.labelIP->setVisible(false);
    ui.labelPort->setVisible(false);

    // d�sactiver et cacher les champs de saisi ip et port
    ui.lineEditIP->setEnabled(false);
    ui.lineEditIP->setVisible(false);
    ui.lineEditPort->setEnabled(false);
    ui.lineEditPort->setVisible(false);
}

// M�thode du slot lorsque le signal disconnected est �mis par socketClient
void ClientTCPIP::onSocketDisconnected()
{
    ui.labelStatus->setText("Status connexion : D�connect�");

    // Rendre invisible et d�sactiver les boutons tant que la connexion n'est plus �tablie
    ui.pushBtnCelsius->setVisible(false);
    ui.pushBtnCelsius->setEnabled(false);

    ui.pushBtnFahrenheit->setVisible(false);
    ui.pushBtnFahrenheit->setEnabled(false);

    ui.pushBtnHygrometie->setVisible(false);
    ui.pushBtnHygrometie->setEnabled(false);

    // activer et rendre visible le bouton connexion
    ui.pushBtnConnect->setEnabled(true);
    ui.pushBtnConnect->setVisible(true);

    // rendre visible les labels ip et port
    ui.labelIP->setVisible(true);
    ui.labelPort->setVisible(true);

    // activer et rendre visible les champs de saisi ip et port
    ui.lineEditIP->setEnabled(true);
    ui.lineEditIP->setVisible(true);
    ui.lineEditPort->setEnabled(true);
    ui.lineEditPort->setVisible(true);
}

// M�thode du slot lors du clic sur le bouton d'envoie de message au serveur
void ClientTCPIP::onSendMessageButtonClicked()
{
    if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connect�
    {
        socketClient->write("TEST1234\n"); // On envoie le message au serveur
    }
}

// M�thode du slot lorsque le signal readyRead est �mis par socketClient
void ClientTCPIP::onSocketReadyRead()
{
    QByteArray data = socketClient->read(socketClient->bytesAvailable()); // On va lire les donn�es re�u
    QString str(data); // On converti nos donn�es en une chaine de caract�re

    ui.labelStatus->setText("Message re�u du serveur : " + str); // On affiche les donn�es re�u
}