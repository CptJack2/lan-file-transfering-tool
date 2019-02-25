#include"sloter.h"
#include"FileView.h"
void sloter::new_client() {
	msrvSock = mServer->nextPendingConnection();
	connect(msrvSock, &QTcpSocket::readyRead, this, &sloter::msrvSock_Read_Data);
	//����˱������ϣ������������ͻ��˵�transfer port
	if (Im_server) {
		mSocket = new QTcpSocket();
		connect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
		mSocket->connectToHost(msrvSock->peerAddress(), transfer_port, QTcpSocket::ReadWrite);
		mSocket->waitForConnected();
		//msocketΪ���������½������߳�
		//psendfilethread = new SendFileThread(mSocket);
		emit cli_connected();
	}
	else{
		//msrvSockΪ���������½������߳�
		//psendfilethread = new SendFileThread(msrvSock);
	}
}
//����˶������ݣ��ͻ��˶���������Ϣ
void sloter::mSocket_Read_Data() {
	//����Ƿ���ˣ���socket������������
	if (Im_server) {
		DataArrivedProcedure(mSocket);
	}
	//����ǿͻ��ˣ��������ǿ�����Ϣ
	else {
		ControlCmdArrivedProcedure(mSocket , msrvSock);
	}
}
//�������յ�����ʱ�Ĵ���취
void sloter::ControlCmdArrivedProcedure(QTcpSocket* pctrlsock, QTcpSocket* ptranssock) {
	QByteArray buffer;
	buffer = pctrlsock->readAll();
	QString cmd = QString::fromUtf8(buffer);
	cmd = cmd.trimmed();
	QStringList cmdlst;
	cmdlst = cmd.split(' ', QString::SkipEmptyParts);
	if (cmdlst.empty())return;
	//�ͻ���Ҫ�����ļ�
	if (cmdlst[0] == "get") {
		QFileInfo f(de_EscapeRoute(cmdlst[1]));
		//f.open(QIODevice::ReadOnly);
		//�ӿ����������ļ��Ĵ�С
		int t = f.size();
		QByteArray ba= QByteArray::number(t);
		pctrlsock->write(ba);
		pctrlsock->waitForBytesWritten();
		//�ȴ��Է����Ϳ�ʼ�����ź�
		pctrlsock->waitForReadyRead();
		cmd = QString::fromUtf8(pctrlsock->readAll());
		if (cmd != "start transfer")return;
		//�������̴߳����ļ�
		psendfilethread = new SendFileThread(msrvSock);
		psendfilethread->setFileName(de_EscapeRoute(cmdlst[1]));
		connect(psendfilethread, &QThread::finished, this,&sloter::SendFileFin);
		psendfilethread->start();
	}
	else if (cmdlst[0] == "put")
		//putָ���߷�getָ��ȥ��ȡ
		cli_get(de_EscapeRoute(cmdlst[1]), de_EscapeRoute(cmdlst[2]));
	else if (cmdlst[0] == "getdirinfo") {
		//��ȡҪ���ҵ��ļ����µ��ļ������л�������
		QStringList string;
		string << "*";
		QDir dir(de_EscapeRoute(cmdlst[1]));
		QFileInfoList list = dir.entryInfoList(string, QDir::AllEntries, QDir::DirsFirst);
		QDataStream ds(pctrlsock);
		//�ȷ����б���
		ds << list.size();
		for (int i = 0; i < list.size();++i){
			ds << QPair<QString, bool>(list[i].fileName(), list[i].isFile());
			if (i % 20 == 0)
				//ÿ20���б���д��һ��ϵͳ�����������������ʧ
				pctrlsock->waitForBytesWritten();
		}
	}
	else if (cmdlst[0] == "getdiskinfo") {
		//��ȡ����disk��Ϣ�����л�������
		QList<QString> tt;
		for (QFileInfo t : QDir::drives())
			tt.push_back(t.absolutePath());
		QDataStream ds(pctrlsock);
		ds << tt;
	}
	else if (cmdlst[0] == "get_my_disks") {
		pRemotefv->ShowRoute("all disks");
	}
}
//����˶���������Ϣ,�ͻ��˶�������
void sloter::msrvSock_Read_Data() {
	//����Ƿ���ˣ���socket�������ǿ�����Ϣ
	if (Im_server) {
		ControlCmdArrivedProcedure(msrvSock,mSocket);
	}
	//����ǿͻ��ˣ�������������
	else {
		DataArrivedProcedure(msrvSock);
	}
}
//�������յ����ݵĴ���취
void sloter::DataArrivedProcedure(QTcpSocket* ptranssock) {
	//д�뵽�ļ�������
	file_recv_buffer.append(ptranssock->readAll());
	file_desc->write(file_recv_buffer);
	file_recv_buffer.clear();
	//������д�Ĵ�С�ж��Ƿ�д�����
	if (file_desc->size() + file_recv_buffer.length() >= recv_file_len) {
		file_desc->close();
		file_desc->~QFile();
		file_desc = NULL;
		emit transfer_done();
	}
}
bool sloter::srv_listen() {
	return mServer->listen(QHostAddress::Any, control_port);
}
bool sloter::cli_connect(QString addr) {
	//�������ӷ���ˣ�����ʵ���޸�Ϊ�ͻ���
	Im_server = false;
	if (mSocket)
		mSocket->~QTcpSocket();
	mSocket = new QTcpSocket();
	mSocket->connectToHost(addr, control_port);
	if (!mSocket->waitForConnected())//����ʧ��
		return false;
	connect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
	//�������������ȴ����������
	mServer->close();
	if(!mServer->listen(QHostAddress::Any, transfer_port))
		return false;
}
void sloter::cli_get(QString filen,QString saveroute) {
	//disconnect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
	QTcpSocket* ctrl = GetCtrlSock_DisconnectSlot();
	//���������������
	//TODO disconnect()ctrl proc
	ctrl->write(QString("get "+EscapeRoute(filen)).toUtf8());
	ctrl->waitForBytesWritten();
	//��ȡ·���еĴ��ļ���
	QFileInfo qfi(filen);
	save_name = qfi.fileName();
	//�ȴ��Է������ļ���С
	ctrl->waitForReadyRead(/*-1*/);
	recv_file_len = ctrl->readAll().toInt();
	RecoverSockSlot();
	//���Ϳ�ʼ����ָ��
	ctrl->write("start transfer");
	ctrl->waitForBytesWritten();
	//�򿪱�����ļ�������
	file_desc = new QFile(saveroute+"/"+save_name);
	file_desc->open(QIODevice::WriteOnly);
	//����qeventloop�����ȴ�transfer_done�ź�
	QEventLoop loop;
	connect(this, &sloter::transfer_done, &loop, &QEventLoop::quit);
	loop.exec();
}
sloter::sloter() {
	mServer = new QTcpServer();
	connect(mServer, SIGNAL(newConnection()), this, SLOT(new_client()));
}
int sloter::get_control_port(){ return control_port; }
int sloter::get_transfer_port(){ return transfer_port; }
QList<QPair<QString, bool>>  sloter::get_remote_dir_info(QString remote_dir) {
	QTcpSocket* ctrl=GetCtrlSock_DisconnectSlot();
	
	ctrl->write(QString("getdirinfo " + EscapeRoute(remote_dir)).toUtf8());
	ctrl->waitForBytesWritten();

	ctrl->waitForReadyRead();
	QDataStream ds(ctrl);
	int listSize;//ָʾ�ļ��б��С
	ds >> listSize;
	QList<QPair<QString, bool>>  list;
	QPair<QString, bool> tt;
	while (list.size() < listSize) {
		qint64 ti = ctrl->bytesAvailable();
		if (ti==0)//����Ϣ��δ������ȫ����ȴ���һ����Ϣ����
			ctrl->waitForReadyRead();
		ds >> tt;
		list.push_back(tt);
	}
	RecoverSockSlot();
	return list;
}
QList<QString>  sloter::get_remote_disk_info() {
	QTcpSocket* ctrl = GetCtrlSock_DisconnectSlot();
	ctrl->write(QString("getdiskinfo").toUtf8());
	ctrl->waitForBytesWritten();
	ctrl->waitForReadyRead();
	QDataStream ds(ctrl);
	QList<QString>  list;
	ds >> list;
	RecoverSockSlot();
	return list;
}
void sloter::cli_put(QString file, QString addr) {
	//���ݷ����ָʾ�����ж��ĸ�socket�ǿ������ʹ�����
	QTcpSocket* ctrl;
	if (!Im_server)
		ctrl = mSocket;
	else
		ctrl = msrvSock;
	ctrl->write(("put " + EscapeRoute(file) + " " + de_EscapeRoute(addr)).toUtf8());
	ctrl->waitForBytesWritten();
}
QTcpSocket* sloter::GetCtrlSock_DisconnectSlot(){
	//���ݷ����ָʾ�����ж��ĸ�socket�ǿ������ʹ�����
	//���Ұѿ��ƴ�����Ϣ�����Ͽ�����Ϊ���᷵���ļ���Сʱ���ܱ���������ػ�
	if (!Im_server) {
		disconnect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
		return mSocket;
	}
	else {
		disconnect(msrvSock, &QTcpSocket::readyRead, this, &sloter::msrvSock_Read_Data);
		return msrvSock;
	}
}
void sloter::RecoverSockSlot() {
	//�ָ����ƺ�������
	if (!Im_server)
		connect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
	else
		connect(msrvSock, &QTcpSocket::readyRead, this, &sloter::msrvSock_Read_Data);
}
//���ڿͻ������Ӳ�get remote disks��֪ͨ����˿���get remote disks��
void sloter::TellPeerGetMyDisks() {
	//���ݷ����ָʾ�����ж��ĸ�socket�ǿ������ʹ�����
	QTcpSocket* ctrl;
	if (!Im_server)
		ctrl = mSocket;
	else
		ctrl = msrvSock;
	ctrl->write("get_my_disks");
	ctrl->waitForBytesWritten();
}
QString sloter::EscapeRoute(QString route) { return route.replace(" ", "@"); }
QString sloter::de_EscapeRoute(QString route) { return route.replace("@", " "); }
void sloter::SendFileFin() {
	psendfilethread->quit();
	delete psendfilethread;
	psendfilethread = nullptr;
}