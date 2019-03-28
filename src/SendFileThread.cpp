#include<Windows.h>
#include"SendFileThread.h"
#include"sloter.h"

SendFileThread::SendFileThread(QTcpSocket*p) :ptranssock(p){}
void SendFileThread::run() {
	while (1) {
		if (name.isEmpty()) {
			msleep(500);
			continue;
		}
		QFile f(name);
		f.open(QIODevice::ReadOnly);
		long long fsize = f.size();
		long long sc = 0;
		int c = 0;
		//发送文件的大小
		unsigned long long t = f.size();
		/*QByteArray ba = QByteArray::number(t);
		ptranssock->write(ba);*/
		QDataStream ds(ptranssock);
		ds << t;
		ptranssock->waitForBytesWritten();
		//TODO 改造成守护线程
		//发送文件
		while (!f.atEnd()) {
			QByteArray ba = f.read(4 * 1024 * 1024);
			sc+=ptranssock->write(ba);
			//读入文件，并从数据流送出
			//如果程序占用内存大于ubound，等待其flush直到小于lbound再写入
			if (ptranssock->bytesToWrite() > mem_upper_bound)
				while (ptranssock->bytesToWrite() > mem_lower_bound) {
					ptranssock->flush();
					msleep(100);
				}
			ptranssock->flush();
			if (sc / 20 * 1024 * 1024 > c || sc>=fsize) {
				emit progress(sc / (fsize / 100));
				++c;
			}
		}
		f.close();
		while (ptranssock->bytesToWrite() != 0){
			ptranssock->flush();
			Sleep(100);
		}
		name.clear();
	}
}