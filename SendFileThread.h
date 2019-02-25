#pragma once
#include<QThread>
#include<qtcpsocket.h>
#include<qfile.h>
#include<qstring.h>
class SendFileThread :public QThread {
public:
	SendFileThread(QTcpSocket*);
	void inline setFileName(QString n) { name = n; }
private:
	QTcpSocket* ptranssock;
	QString name;
	int mem_upper_bound = 200 * 1024 * 1024;//ָʾ�ڴ���get����ʱ����Է����ͼ����ļ���qtcpsocket����ռ�ö����ڴ���buffering
	int mem_lower_bound = 100 * 1024 * 1024;
protected:
	virtual void run();
};