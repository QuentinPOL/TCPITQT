#include "ServeurTCPIP.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    srand(time(NULL)); // S�quence speudo al�atoire pour g�n�rer les valeurs des temp�ratures et de l'hygrom�trie

    QApplication a(argc, argv);
    ServeurTCPIP w;
    w.show();
    return a.exec();
}
