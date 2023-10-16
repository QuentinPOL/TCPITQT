#include "ClientTCPIP.h"

// Constructeur IHM
ClientTCPIP::ClientTCPIP(QWidget *parent) : QMainWindow(parent)
{
    ui.setupUi(this);

    // Rendre invisible et désactiver les boutons tant que la connexion n'est pas établie
    ui.pushBtnCelsius->setVisible(false);
    ui.pushBtnCelsius->setEnabled(false);

    ui.pushBtnFahrenheit->setVisible(false);
    ui.pushBtnFahrenheit->setEnabled(false);

    ui.pushBtnHygrometie->setVisible(false);
    ui.pushBtnHygrometie->setEnabled(false);

    // Rendre invisible le champ et le label du numéro du capteur
    ui.numcapteur->setVisible(false);
    ui.numcapteur->setEnabled(false);

    ui.labelCapteur->setVisible(false);
    ui.labelCapteur->setEnabled(false);

    socketClient = new QTcpSocket(this); // Inialisation du socket client pour notre interface (IHM)
    QObject::connect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected())); // On connecte nos signaux et slots avec le signal connected
    QObject::connect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected())); // On connecte nos signaux et slots avec le signal disconnected
    QObject::connect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead())); // On connecte nos signaux et slots avec le signal readyRead
}

// Destructeur IHM
ClientTCPIP::~ClientTCPIP()
{
    socketClient->close(); // On ferme la connexion
    delete socketClient; // On Supprime pour la mémoire
}

// Méthode du slot lors du clic sur le bouton de connexion
void ClientTCPIP::onConnectButtonClicked()
{
    // On récupère ce qui a été saisi
    QString ip = ui.lineEditIP->text();
    QString port = ui.lineEditPort->text();

    // Vérifier si les champs ne sont pas pas vide
    if (!ip.isEmpty() && !port.isEmpty())
    {
        // On converti le port de String en int
        bool ok;
        int portAsInt = port.toInt(&ok);

        if (ok) // Si la conversion est réussie
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

// Méthode du slot lorsque le signal connected est émis par socketClient
void ClientTCPIP::onSocketConnected()
{
    ui.labelStatus->setText("Status connexion : Connecter");

    // Rendre visible et activer les boutons et effacer le texte d'erreur
    ui.pushBtnCelsius->setVisible(true);
    ui.pushBtnCelsius->setEnabled(true);

    ui.pushBtnFahrenheit->setVisible(true);
    ui.pushBtnFahrenheit->setEnabled(true);

    ui.pushBtnHygrometie->setVisible(true);
    ui.pushBtnHygrometie->setEnabled(true);

    ui.label_error->setText("");

    // désactiver et cacher le bouton connexion
    ui.pushBtnConnect->setEnabled(false);
    ui.pushBtnConnect->setVisible(false);

    // désactiver les labels ip et port
    ui.labelIP->setVisible(false);
    ui.labelPort->setVisible(false);

    // désactiver et cacher les champs de saisi ip et port
    ui.lineEditIP->setEnabled(false);
    ui.lineEditIP->setVisible(false);
    ui.lineEditPort->setEnabled(false);
    ui.lineEditPort->setVisible(false);

    // Rendre visible le champ du numéro et du label du capteur
    ui.numcapteur->setVisible(true);
    ui.numcapteur->setEnabled(true);

    ui.labelCapteur->setVisible(true);
    ui.labelCapteur->setEnabled(true);
 
}

// Méthode du slot lorsque le signal disconnected est émis par socketClient
void ClientTCPIP::onSocketDisconnected()
{
    ui.labelStatus->setText("Status connexion : Déconnecter");

    // Rendre invisible et désactiver les boutons tant que la connexion n'est plus établie
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

    // Rendre invisible le champ et le label du numéro du capteur
    ui.numcapteur->setVisible(false);
    ui.numcapteur->setEnabled(false);

    ui.labelCapteur->setVisible(false);
    ui.labelCapteur->setEnabled(false);
}

// Méthode du slot lorsque le signal readyRead est émis par socketClient
void ClientTCPIP::onSocketReadyRead()
{
    QByteArray data = socketClient->read(socketClient->bytesAvailable()); // On va lire les données reçu
    QString str(data); // On converti nos données en une chaine de caractère

	if (str.left(2) == "Td")
	{
		ui.labelStatus->setText("Message du serveur : " + str.right(6) + " Capteur " + str.mid(2, 1) + str.mid(3, 1)); // On affiche les données reçu
	}
	else if (str.left(2) == "Tf")
	{
		ui.labelStatus->setText("Message du serveur : " + str.right(6) + " Capteur " + str.mid(2, 1) + str.mid(3, 1)); // On affiche les données
	}
	else if (str.left(2) == "Hr")
	{
		ui.labelStatus->setText("Message du serveur : " + str.right(5) + " Capteur " + str.mid(2, 1) + str.mid(3, 1)); // On affiche les données
	}
	else
	{
		ui.labelStatus->setText("Message du serveur : " + str + " Capteur "); // On affiche les données reçu
	}
}

// Méthode du slot quand on demande la température en Celsius
void ClientTCPIP::onSendCelsiusButtonClicked()
{
    // On récupère la valeur de la spinbox
    int numcapteur = ui.numcapteur->value();

    QString requestServer;

    if (numcapteur < 10) // Si le numéro ne dépasse pas 10
    {
        requestServer = "Td0" + QString::number(numcapteur) + "?"; // On rajoute un 0 devant
    }
    else
    {
        requestServer = "Td" + QString::number(numcapteur) + "?";
    }

    if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
    {
        socketClient->write(requestServer.toUtf8()); // On envoie le message au serveur
    }
}

// Méthode du slot lorsque l'on demande une température en Farenheit
void ClientTCPIP::onSendFarhenheitButtonClicked()
{
    // On récupère la valeur de la spinbox
    int numcapteur = ui.numcapteur->value();

    QString requestServer;

    if (numcapteur < 10) // Si le numéro ne dépasse pas 10
    {
        requestServer = "Tf0" + QString::number(numcapteur) + "?"; // On rajoute un 0 devant
    }
    else
    {
        requestServer = "Tf" + QString::number(numcapteur) + "?";
    }

    if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
    {
        socketClient->write(requestServer.toUtf8()); // On envoie le message au serveur
    }
}

// Méthode du slot quand on demande une valeur Hygrometrie
void ClientTCPIP::onSendHygrometrieButtonClicked()
{
    // On récupère la valeur de la spinbox
    int numcapteur = ui.numcapteur->value();

    QString requestServer;

    if (numcapteur < 10) // Si le numéro ne dépasse pas 10
    {
        requestServer = "Hr0" + QString::number(numcapteur) + "?"; // On rajoute un 0 devant
    }
    else
    {
        requestServer = "Hr" + QString::number(numcapteur) + "?";
    }

    if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
    {
        socketClient->write(requestServer.toUtf8()); // On envoie le message au serveur
    }
}
