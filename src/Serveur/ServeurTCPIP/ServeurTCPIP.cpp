#include "ServeurTCPIP.h"

// Constructeur IHM
ServeurTCPIP::ServeurTCPIP(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    socketServer = new QTcpServer(this); // Inialisation du socket server pour notre interface (IHM)
    QObject::connect(socketServer, SIGNAL(newConnection()), this, SLOT(onServerNewConnection())); // On connecte nos signaux et slots avec le signal newConnection

    socketServer->listen(QHostAddress::AnyIPv4, 1234); // On ouvre le serveur sur n'importe quelle adresse ipv4 (127.0.0.1 ou ip sur le réseaux) et sur le port 1234
}

// Destructeur IHM
ServeurTCPIP::~ServeurTCPIP()
{
    socketServer->close(); // On ferme la connexion
    delete socketServer; // On Supprime pour la mémoire
}

// Méthode du slot lorsque le signal newConnection est émis par socketServer
void ServeurTCPIP::onServerNewConnection()
{
    ui.labelStatus->setText("Un client s'est connecté");

    QTcpSocket* client = socketServer->nextPendingConnection(); // On récupère le client
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On connecte les signaux et slots du client avec le signal readyRead
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On connecte les signaux et slots du client avec le signal disconnected
}

// Méthode du slot lorsque le signal disconnected est émis par client
void ServeurTCPIP::onClientDisconnected()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender()); // On va récupèrer l'objet qui nous un envoyé le signal (conversion car on reçoie QObject* et il nous faut QTcpSocket*)
    
    QObject::disconnect(obj, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On déconnecte les signaux et slots du client avec le signal readyRead
    QObject::disconnect(obj, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On déconnecte les signaux et slots du client avec le signal disconnected

    obj->deleteLater(); // Suppresion de l'object pour libérer la mémoire
}

// Méthode du slot lorsque le signal readyRead est émis par client
void ServeurTCPIP::onClientReadyRead()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender()); // On va récupèrer l'objet qui nous un envoyé le signal (conversion car on reçoie QObject* et il nous faut QTcpSocket*)
    
    QByteArray data = obj->read(obj->bytesAvailable()); // On va lire les données reçu
    QString str(data); // On converti nos données en une chaine de caractère

    ui.labelStatus->setText("Message reçu du client : " + str); // On affiche les données reçu

    if (obj->state() == QTcpSocket::ConnectedState) // Si le client est bien connecté
    {
        obj->write("Bien reçu !"); // on lui  renvoie un message en réponse
    }
}
