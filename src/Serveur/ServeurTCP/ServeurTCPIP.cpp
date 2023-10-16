#include "ServeurTCPIP.h"

// Constructeur 
ServeurTCPIP::ServeurTCPIP(QObject *parent) : QObject(parent)
{
	socketServer = new QTcpServer(this); // Inialisation du socket server pour notre interface (IHM)
	connect(socketServer, SIGNAL(newConnection()), this, SLOT(onServerNewConnection())); // On connecte nos signaux et slots avec le signal newConnection

	// On ouvre le serveur sur n'importe quelle adresse ipv4 (127.0.0.1 ou ip sur le r�seaux) et sur le port 1234s
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
	delete socketServer; // On Supprime pour la m�moire
}

// M�thode du slot lorsque le signal newConnection est �mis par socketServer
void ServeurTCPIP::onServerNewConnection()
{
	QTcpSocket* client = socketServer->nextPendingConnection(); // On r�cup�re le client

	qDebug() << "Un client s'est connecter : " << client->peerAddress();

	connect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On connecte les signaux et slots du client avec le signal readyRead
	connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On connecte les signaux et slots du client avec le signal disconnected
}

// M�thode du slot lorsque le signal disconnected est �mis par client
void ServeurTCPIP::onClientDisconnected()
{
	QTcpSocket* client = qobject_cast<QTcpSocket*>(sender()); // On va r�cup�rer l'objet qui nous un envoy� le signal (conversion car on re�oie QObject* et il nous faut QTcpSocket*)

	qDebug() << "Un client s'est deconnecter : " << client->peerAddress();

	disconnect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On d�connecte les signaux et slots du client avec le signal readyRead
	disconnect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On d�connecte les signaux et slots du client avec le signal disconnected

	client->deleteLater(); // Suppresion de l'object pour lib�rer la m�moire
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

	QString strNumber;

	float randomTemp = 00.00; // Cr�ation de la temp�rature

	if (str.left(2) == "Td" && isInt && isInt2) // Si c'est une demande de temp�rature en �C
	{
		qDebug() << "Demande du client : " << str << " [" << obj->peerAddress() << "]";

		randomTemp = (-20.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (37.00 - (-20.00)))); // Num�ro random entre [-20.00�C;35.00�C] 

		if (randomTemp < 10 || randomTemp < -10)
		{
			strNumber = "0" + QString::number(randomTemp, 'f', 2);
		}
		else
		{
			strNumber = QString::number(randomTemp, 'f', 2);
		}

		if (randomTemp > 0)
		{
			str = "Td" + QString::number(int1) + QString::number(int2) + ",+" + QString::number(randomTemp, 'f', 2); // On fait la r�ponse et on met une pr�cision de 2 chiffres derri�re la virgule pour un float
		}
		else
		{
			str = "Td" + QString::number(int1) + QString::number(int2) + "," + QString::number(randomTemp, 'f', 2); // On fait la r�ponse et on met une pr�cision de 2 chiffres derri�re la virgule pour un float
		}
	}
	else if (str.left(2) == "Tf" && isInt && isInt2) // Si c'est une demande de temp�rature en �F
	{
		qDebug() << "Demande du client : " << str << " [" << obj->peerAddress() << "]";

		randomTemp = (-04.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (95.00 - (-04.00)))); // Num�ro random entre [-20.00�C;35.00�C] -> [-04.00�F;95.00�F]

		if (randomTemp < 10 || randomTemp < -10)
		{
			strNumber = "0" + QString::number(randomTemp, 'f', 2);
		}
		else
		{
			strNumber = QString::number(randomTemp, 'f', 2);
		}

		if (randomTemp > 0)
		{
			str = "Tf" + QString::number(int1) + QString::number(int2) + ",+" + strNumber; // On fait la r�ponse et on met une pr�cision de 2 chiffres derri�re la virgule pour un float
		}
		else
		{
			str = "Tf" + QString::number(int1) + QString::number(int2) + "," + strNumber; // On fait la r�ponse et on met une pr�cision de 2 chiffres derri�re la virgule pour un float
		}
	}
	else if (str.left(2) == "Hr" && isInt && isInt2) // Si c'est une demande d'hygrom�trie en % 
	{
		qDebug() << "Demande du client : " << str << " [" << obj->peerAddress() << "]";

		randomTemp = (00.00) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (99.99 - (00.00)))); // Num�ro random entre [00.00%;99.99%]

		if (randomTemp < 10)
		{
			strNumber = "0" + QString::number(randomTemp, 'f', 2);
		}
		else
		{
			strNumber = QString::number(randomTemp, 'f', 2);
		}

		str = "Hr" + QString::number(int1) + QString::number(int2) + "," + strNumber; // On fait la r�ponse et on met une pr�cision de 2 chiffre1 derri�re la virgule pour un float
	}

	// Reconversion de la QString en QByteArray pour l'envoie du message
	data = str.toUtf8();

	if (obj->state() == QTcpSocket::ConnectedState) // Si le client est bien connect�
	{
		obj->write(data); // on lui  renvoie un message en r�ponse
	}
}
