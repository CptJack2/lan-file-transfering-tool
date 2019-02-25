#include "lan_trans.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Lan_trans w;
	w.show();
	return a.exec();
}
