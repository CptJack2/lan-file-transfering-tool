#pragma once
#include<QThread>
#include<qtcpsocket.h>
#include<qfile.h>
#include<qstring.h>
#include<qprogressbar.h>
class sloter;

class SendFileThread :public QThread {
	Q_OBJECT

public:
	SendFileThread(QTcpSocket*);
	//sloter* pslt;
	void inline setFileName(QString n) { name = n; }
private:
	QTcpSocket* ptranssock;
	QString  name="";
	int mem_upper_bound = 200 * 1024 * 1024;//指示在处理get命令时，向对方发送己方文件，qtcpsocket最多可占用多少内存作buffering
	int mem_lower_bound = 100 * 1024 * 1024;
protected:
	virtual void run();
signals:
	void progress(int);
	void send_done();
};