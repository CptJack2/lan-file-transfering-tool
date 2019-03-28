#pragma once
#include<QThread>
#include<qtcpsocket.h>
#include<qfile.h>
#include<qstring.h>
#include<qprogressbar.h>
#include<deque>
using namespace std;
class sloter;

class GetFileThread :public QThread {
	Q_OBJECT

public:
	GetFileThread(sloter*);
	void AddToQueue(QString filen, QString route);
private:
	sloter* pslt;
	deque<pair<QString,QString>> getqueue;
protected:
	virtual void run();
//signals:
//	void progress(int);
//	void send_done();
};