#include<Windows.h>
#include"SendFileThread.h"
//void SendFileThread::SendFile(QString route) {
//	
//}
SendFileThread::SendFileThread(QTcpSocket*p) :ptranssock(p){}
void SendFileThread::run() {
	QFile f(name);
	f.open(QIODevice::ReadOnly);
	while (!f.atEnd()) {
		QByteArray ba = f.read(4 * 1024 * 1024);
		ptranssock->write(ba);
		//读入文件，并从数据流送出
		//如果程序占用内存大于ubound，等待其flush直到小于lbound再写入
		if (ptranssock->bytesToWrite() > mem_upper_bound)
			while (ptranssock->bytesToWrite() > mem_lower_bound) {
				ptranssock->flush();
				sleep(100);
			}
		ptranssock->flush();
	}
	f.close();
	while (ptranssock->bytesToWrite() != 0){
		ptranssock->flush();
		Sleep(100);
	}
}