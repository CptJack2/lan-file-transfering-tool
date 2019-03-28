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
	//���е�����ͻ��˺ͷ���˶�ȷ�����������ӣ����Կ����ϴ��ļ��ػ��߳�
	pgetfilethread = new GetFileThread(this);
	pgetfilethread->start();
	//QTcpSocket* trans = GetTransSock();
	psendfilethread = new SendFileThread(GetTransSock());
	//connect(psendfilethread, &QThread::finished, this, &sloter::SendFileFin);
	connect(psendfilethread, SIGNAL(progress(int)), this, SLOT(progress_slot(int)));
	//psendfilethread->pslt = this;
	psendfilethread->start();
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
		//�����ļ�����ʹ�����̴߳����ļ�
		psendfilethread->setFileName(de_EscapeRoute(cmdlst[1]));
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
	//ͷ8���ֽ�Ϊ�ļ�����
	if (recv_file_len == 0) {
		QDataStream ds(ptranssock);
		ds>>recv_file_len;
	}
	//д�뵽�ļ�������
	file_recv_buffer.append(ptranssock->readAll());
	file_desc->write(file_recv_buffer);
	file_recv_buffer.clear();
	//����pPBar
	unsigned long long sc = file_desc->size() + file_recv_buffer.length();
	static int c = 0;//���ݽ��ȷ��ͽ����źŵĸ�������
	if (sc / 20 * 1024 * 1024 > c || sc >= recv_file_len){
		pPBar->setValue(sc / (recv_file_len/100 )  );
		pstbar->showMessage("receiving " + save_name);
		++c;}
	//������д�Ĵ�С�ж��Ƿ�д�����
	if ((long long)file_desc->size() + (long long)file_recv_buffer.length() >= recv_file_len) {
		file_desc->close();
		file_desc->~QFile();
		file_desc = NULL;
		emit recv_done();
		c = 0;
		recv_file_len = 0;//���ý����ļ�����
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
	//�Ͽ�ctrlsock�ϵ�data������
	QTcpSocket* ctrl = GetCtrlSock_DisconnectSlot();
	//��ȡ·���еĴ��ļ���
	QFileInfo qfi(filen);
	save_name = qfi.fileName();
	//�ָ�ctrlsock�ϵ�data������
	RecoverSockSlot();
	//�򿪱�����ļ�������
	file_desc = new QFile(saveroute+"/"+save_name);
	file_desc->open(QIODevice::WriteOnly);
	//���������������
	ctrl->write(QString("get " + EscapeRoute(filen)).toUtf8());
	ctrl->waitForBytesWritten();
	//����qeventloop�����ȴ�recv_done�ź�
	QEventLoop loop;
	connect(this, &sloter::recv_done, &loop, &QEventLoop::quit);
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
	QTcpSocket* ctrl = GetCtrlSock();
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
	QTcpSocket* ctrl = GetCtrlSock();
	ctrl->write("get_my_disks");
	ctrl->waitForBytesWritten();
}
QString sloter::EscapeRoute(QString route) { return route.replace(" ", "��"); }//�����һ���ò������ַ�����ո�
QString sloter::de_EscapeRoute(QString route) { return route.replace("��", " "); }//�����ڷ������������ĵط������ո���ļ����ᱻ�����ɶ������
void sloter::SendFileFin() {
	psendfilethread->quit();
	delete psendfilethread;
	psendfilethread = nullptr;
}
QTcpSocket* sloter::GetCtrlSock() {
	if (!Im_server)
		return mSocket;
	else
		return msrvSock;
}
QTcpSocket* sloter::GetTransSock() {
	if (Im_server)
		return mSocket;
	else
		return msrvSock;
}
void sloter::progress_slot(int t) {
	pPBar->setValue(t);
}
void sloter::AddToGetQueue(QString f, QString r) {
	pgetfilethread->AddToQueue(f, r);
}