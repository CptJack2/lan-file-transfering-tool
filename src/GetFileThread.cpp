#include"GetFileThread.h"
#include<qfileinfo.h>
#include"sloter.h"
GetFileThread::GetFileThread(sloter*p):pslt(p){}
void GetFileThread::run() {
	while (1) {
		if (getqueue.empty()) { 
			msleep(300); 
			continue; }
		pslt->cli_get(getqueue.front().first,getqueue.front().second);
		getqueue.pop_front();
	}
}
void GetFileThread::AddToQueue(QString filen, QString route) {
	getqueue.push_back(pair<QString, QString>(filen, route));
}