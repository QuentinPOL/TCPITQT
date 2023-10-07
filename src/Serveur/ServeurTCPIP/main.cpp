#include "ServeurTCPIP.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    srand(time(NULL)); // Séquence speudo aléatoire pour générer les valeurs des températures et de l'hygrométrie

    QApplication a(argc, argv);
    ServeurTCPIP w;
    w.show();
    return a.exec();
}
