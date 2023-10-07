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

    // On va convertir les deux caract�res apr�s "Td" ou "Tf" ou "Hr" pour savoir si ce sont des entiers
    bool isInt, isInt2;
    int int1 = str.mid(2, 1).toInt(&isInt);
    int int2 = str.mid(3, 1).toInt(&isInt2);

    float randomTemp = 00.00; // Cr�ation de la temp�rature

    if (str.left(2) == "Td" && isInt && isInt2) // Si c'est une demande de temp�rature en �C
    {
        ui.labelStatus->setText("Demande du client : Td" + int1 + int2); // On affiche les donn�es re�u

        randomTemp = (-20.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (35.00 - (-20.00)))); // Num�ro random entre [-20.00�C;35.00�C] 
        str = "Td" + int1 + int2 + QString::number(randomTemp, 'f', 2); // On fait la r�ponse et on met une pr�cision de 2 chiffres derri�re la virgule pour un float
    }
    else if (str.left(2) == "Tf" && isInt && isInt2) // Si c'est une demande de temp�rature en �F
    {
        ui.labelStatus->setText("Demande du client : Tf" + int1 + int2); // On affiche les donn�es re�u

        randomTemp = (-04.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (95.00 - (-04.00)))); // Num�ro random entre [-20.00�C;35.00�C] -> [-04.00�F;95.00�F]
        str = "Tf" + int1 + int2 + QString::number(randomTemp, 'f', 2); // On fait la r�ponse et on met une pr�cision de 2 chiffres derri�re la virgule pour un float
    }
    else if (str.left(2) == "Hr" && isInt && isInt2) // Si c'est une demande d'hygrom�trie en % 
    {
        ui.labelStatus->setText("Demande du client : Hr" + int1 + int2); // On affiche les donn�es re�u

        randomTemp = (00.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (99.99 - (00.00)))); // Num�ro random entre [00.00%;99.99%]
        str = "Hr" + int1 + int2 + QString::number(randomTemp, 'f', 2); // On fait la r�ponse et on met une pr�cision de 2 chiffre1 derri�re la virgule pour un float
    }

    // Reconversion de la QString en QByteArray pour l'envoie du message
    data = str.toUtf8();

    if (obj->state() == QTcpSocket::ConnectedState) // Si le client est bien connect�
    {
        obj->write(data); // on lui  renvoie un message en r�ponse
    }
}
