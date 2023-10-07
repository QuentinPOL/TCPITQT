#include "ServeurTCPIP.h"

// Constructeur IHM
ServeurTCPIP::ServeurTCPIP(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    socketServer = new QTcpServer(this); // Inialisation du socket server pour notre interface (IHM)
    QObject::connect(socketServer, SIGNAL(newConnection()), this, SLOT(onServerNewConnection())); // On connecte nos signaux et slots avec le signal newConnection

    socketServer->listen(QHostAddress::AnyIPv4, 1234); // On ouvre le serveur sur n'importe quelle adresse ipv4 (127.0.0.1 ou ip sur le r�seaux) et sur le port 1234
}

// Destructeur IHM
ServeurTCPIP::~ServeurTCPIP()
{
    socketServer->close(); // On ferme la connexion
    delete socketServer; // On Supprime pour la m�moire
}

// M�thode du slot lorsque le signal newConnection est �mis par socketServer
void ServeurTCPIP::onServerNewConnection()
{
    ui.labelStatus->setText("Un client s'est connect�");

    QTcpSocket* client = socketServer->nextPendingConnection(); // On r�cup�re le client
    QObject::connect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On connecte les signaux et slots du client avec le signal readyRead
    QObject::connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On connecte les signaux et slots du client avec le signal disconnected
}

// M�thode du slot lorsque le signal disconnected est �mis par client
void ServeurTCPIP::onClientDisconnected()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender()); // On va r�cup�rer l'objet qui nous un envoy� le signal (conversion car on re�oie QObject* et il nous faut QTcpSocket*)
    
    QObject::disconnect(obj, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On d�connecte les signaux et slots du client avec le signal readyRead
    QObject::disconnect(obj, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On d�connecte les signaux et slots du client avec le signal disconnected

    obj->deleteLater(); // Suppresion de l'object pour lib�rer la m�moire
}

// M�thode du slot lorsque le signal readyRead est �mis par client
void ServeurTCPIP::onClientReadyRead()
{
    QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender()); // On va r�cup�rer l'objet qui nous un envoy� le signal (conversion car on re�oie QObject* et il nous faut QTcpSocket*)
    
    QByteArray data = obj->read(obj->bytesAvailable()); // On va lire les donn�es re�u
    QString str(data); // On converti nos donn�es en une chaine de caract�re

    ui.labelStatus->setText("Message re�u du client : " + str); // On affiche les donn�es re�u

    if (obj->state() == QTcpSocket::ConnectedState) // Si le client est bien connect�
    {
        obj->write("Bien re�u !"); // on lui  renvoie un message en r�ponse
    }
}
