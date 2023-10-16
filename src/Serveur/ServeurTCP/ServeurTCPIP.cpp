#include "ServeurTCPIP.h"

#include <iostream>

// Constructeur 
ServeurTCPIP::ServeurTCPIP(QObject *parent) : QObject(parent)
{
	socketServer = new QTcpServer(this); // Inialisation du socket server pour notre interface (IHM)
	connect(socketServer, SIGNAL(newConnection()), this, SLOT(onServerNewConnection())); // On connecte nos signaux et slots avec le signal newConnection

	// On ouvre le serveur sur n'importe quelle adresse ipv4 (127.0.0.1 ou ip sur le réseaux) et sur le port 1234s
	if (socketServer->listen(QHostAddress::AnyIPv4, 1234))
	{
		qDebug() << "Ouverture du serveur sur le port 1234";
	}
	else
	{
		qDebug() << "ERREUR lors de l'ouverture du port";
	}
}

// Destructeur 
ServeurTCPIP::~ServeurTCPIP()
{
	socketServer->close(); // On ferme la connexion
	delete socketServer; // On Supprime pour la mémoire
}

// Méthode du slot lorsque le signal newConnection est émis par socketServer
void ServeurTCPIP::onServerNewConnection()
{
	QTcpSocket* client = socketServer->nextPendingConnection(); // On récupère le client

	qDebug() << "Un client s'est connecter : " << client->peerAddress();

	connect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On connecte les signaux et slots du client avec le signal readyRead
	connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On connecte les signaux et slots du client avec le signal disconnected
}

// Méthode du slot lorsque le signal disconnected est émis par client
void ServeurTCPIP::onClientDisconnected()
{
	QTcpSocket* client = qobject_cast<QTcpSocket*>(sender()); // On va récupèrer l'objet qui nous un envoyé le signal (conversion car on reçoie QObject* et il nous faut QTcpSocket*)

	qDebug() << "Un client s'est deconnecter : " << client->peerAddress();

	disconnect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On déconnecte les signaux et slots du client avec le signal readyRead
	disconnect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On déconnecte les signaux et slots du client avec le signal disconnected

	client->deleteLater(); // Suppresion de l'object pour libérer la mémoire
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

	QString strNumber;
	const char* tempFormat;

	float randomTemp = 00.00; // Création de la température

	if (str.left(2) == "Td" && isInt && isInt2) // Si c'est une demande de température en °C
	{
		qDebug() << "Demande du client : " << str << " [" << obj->peerAddress() << "]";

		randomTemp = (-20.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (37.00 - (-20.00)))); // Numéro random entre [-20.00°C;35.00°C] 

		if (randomTemp < 10 || randomTemp < -10)
		{
			if (randomTemp >= 0) {
				tempFormat = "+%05.2f";
			}
			else if (randomTemp >= -10) {
				tempFormat = "%06.2f"; // Correction ici
			}
			else {
				tempFormat = "%06.2f";
			}

			strNumber = QString::asprintf(tempFormat, randomTemp);
			qDebug() << strNumber;
		}
		else
		{
			strNumber = QString::number(randomTemp, 'f', 2);
		}

		if (randomTemp > 0)
		{
			str = "Td" + QString::number(int1) + QString::number(int2) + ",+" + strNumber; // On fait la réponse et on met une précision de 2 chiffres derrière la virgule pour un float
		}
		else
		{
			str = "Td" + QString::number(int1) + QString::number(int2) + "," + strNumber; // On fait la réponse et on met une précision de 2 chiffres derrière la virgule pour un float
		}
	}
	else if (str.left(2) == "Tf" && isInt && isInt2) // Si c'est une demande de température en °F
	{
		qDebug() << "Demande du client : " << str << " [" << obj->peerAddress() << "]";

		randomTemp = (-04.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (98.60 - (-04.00)))); // Numéro random entre [-20.00°C;35.00°C] -> [-04.00°F;95.00°F]

		if (randomTemp < 10 || randomTemp < -10)
		{
			if (randomTemp >= 0) {
				tempFormat = "+%05.2f";
			}
			else if (randomTemp >= -10) {
				tempFormat = "%06.2f"; // Correction ici
			}
			else {
				tempFormat = "%06.2f";
			}

			strNumber = QString::asprintf(tempFormat, randomTemp);
			qDebug() << strNumber;
		}
		else
		{
			strNumber = QString::number(randomTemp, 'f', 2);
		}

		if (randomTemp > 0)
		{
			str = "Tf" + QString::number(int1) + QString::number(int2) + ",+" + strNumber; // On fait la réponse et on met une précision de 2 chiffres derrière la virgule pour un float
		}
		else
		{
			str = "Tf" + QString::number(int1) + QString::number(int2) + "," + strNumber; // On fait la réponse et on met une précision de 2 chiffres derrière la virgule pour un float
		}
	}
	else if (str.left(2) == "Hr" && isInt && isInt2) // Si c'est une demande d'hygrométrie en % 
	{
		qDebug() << "Demande du client : " << str << " [" << obj->peerAddress() << "]";

		randomTemp = (00.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (99.99 - (00.00)))); // Numéro random entre [00.00%;99.99%]

		str = "Hr" + QString::number(int1) + QString::number(int2) + "," + QString::number(randomTemp, 'f', 2); // On fait la réponse et on met une précision de 2 chiffre1 derrière la virgule pour un float
	}

	// Reconversion de la QString en QByteArray pour l'envoie du message
	data = str.toUtf8();

	if (obj->state() == QTcpSocket::ConnectedState) // Si le client est bien connecté
	{
		obj->write(data); // on lui  renvoie un message en réponse
	}
}
