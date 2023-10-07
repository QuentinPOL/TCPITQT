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

    // On va convertir les deux caractères après "Td" ou "Tf" ou "Hr" pour savoir si ce sont des entiers
    bool isInt, isInt2;
    int int1 = str.mid(2, 1).toInt(&isInt);
    int int2 = str.mid(3, 1).toInt(&isInt2);

    float randomTemp = 00.00; // Création de la température

    if (str.left(2) == "Td" && isInt && isInt2) // Si c'est une demande de température en °C
    {
        ui.labelStatus->setText("Demande du client : Td" + int1 + int2); // On affiche les données reçu

        randomTemp = (-20.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (35.00 - (-20.00)))); // Numéro random entre [-20.00°C;35.00°C] 
        str = "Td" + int1 + int2 + QString::number(randomTemp, 'f', 2); // On fait la réponse et on met une précision de 2 chiffres derrière la virgule pour un float
    }
    else if (str.left(2) == "Tf" && isInt && isInt2) // Si c'est une demande de température en °F
    {
        ui.labelStatus->setText("Demande du client : Tf" + int1 + int2); // On affiche les données reçu

        randomTemp = (-04.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (95.00 - (-04.00)))); // Numéro random entre [-20.00°C;35.00°C] -> [-04.00°F;95.00°F]
        str = "Tf" + int1 + int2 + QString::number(randomTemp, 'f', 2); // On fait la réponse et on met une précision de 2 chiffres derrière la virgule pour un float
    }
    else if (str.left(2) == "Hr" && isInt && isInt2) // Si c'est une demande d'hygrométrie en % 
    {
        ui.labelStatus->setText("Demande du client : Hr" + int1 + int2); // On affiche les données reçu

        randomTemp = (00.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (99.99 - (00.00)))); // Numéro random entre [00.00%;99.99%]
        str = "Hr" + int1 + int2 + QString::number(randomTemp, 'f', 2); // On fait la réponse et on met une précision de 2 chiffre1 derrière la virgule pour un float
    }

    // Reconversion de la QString en QByteArray pour l'envoie du message
    data = str.toUtf8();

    if (obj->state() == QTcpSocket::ConnectedState) // Si le client est bien connecté
    {
        obj->write(data); // on lui  renvoie un message en réponse
    }
}
