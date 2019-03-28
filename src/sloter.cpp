#include"sloter.h"
#include"FileView.h"
void sloter::new_client() {
	msrvSock = mServer->nextPendingConnection();
	connect(msrvSock, &QTcpSocket::readyRead, this, &sloter::msrvSock_Read_Data);
	//服务端被连接上，主动连接数客户端的transfer port
	if (Im_server) {
		mSocket = new QTcpSocket();
		connect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
		mSocket->connectToHost(msrvSock->peerAddress(), transfer_port, QTcpSocket::ReadWrite);
		mSocket->waitForConnected();
		//msocket为数据流，新建传输线程
		//psendfilethread = new SendFileThread(mSocket);
		emit cli_connected();
	}
	//运行到这里，客户端和服务端都确定了数据连接，可以开启上传文件守护线程
	pgetfilethread = new GetFileThread(this);
	pgetfilethread->start();
	//QTcpSocket* trans = GetTransSock();
	psendfilethread = new SendFileThread(GetTransSock());
	//connect(psendfilethread, &QThread::finished, this, &sloter::SendFileFin);
	connect(psendfilethread, SIGNAL(progress(int)), this, SLOT(progress_slot(int)));
	//psendfilethread->pslt = this;
	psendfilethread->start();
}
//服务端读出数据，客户端读出控制信息
void sloter::mSocket_Read_Data() {
	//如果是服务端，此socket读出的是数据
	if (Im_server) {
		DataArrivedProcedure(mSocket);
	}
	//如果是客户端，读出的是控制信息
	else {
		ControlCmdArrivedProcedure(mSocket , msrvSock);
	}
}
//控制流收到命令时的处理办法
void sloter::ControlCmdArrivedProcedure(QTcpSocket* pctrlsock, QTcpSocket* ptranssock) {
	QByteArray buffer;
	buffer = pctrlsock->readAll();
	QString cmd = QString::fromUtf8(buffer);
	cmd = cmd.trimmed();
	QStringList cmdlst;
	cmdlst = cmd.split(' ', QString::SkipEmptyParts);
	if (cmdlst.empty())return;
	//客户需要下载文件
	if (cmdlst[0] == "get") {
		QFileInfo f(de_EscapeRoute(cmdlst[1]));
		//设置文件名，使传输线程传输文件
		psendfilethread->setFileName(de_EscapeRoute(cmdlst[1]));
	}
	else if (cmdlst[0] == "put")
		//put指令，这边发get指令去拉取
		cli_get(de_EscapeRoute(cmdlst[1]), de_EscapeRoute(cmdlst[2]));
	else if (cmdlst[0] == "getdirinfo") {
		//获取要查找的文件夹下的文件并序列化，发送
		QStringList string;
		string << "*";
		QDir dir(de_EscapeRoute(cmdlst[1]));
		QFileInfoList list = dir.entryInfoList(string, QDir::AllEntries, QDir::DirsFirst);
		QDataStream ds(pctrlsock);
		//先发送列表长度
		ds << list.size();
		for (int i = 0; i < list.size();++i){
			ds << QPair<QString, bool>(list[i].fileName(), list[i].isFile());
			if (i % 20 == 0)
				//每20个列表项写入一次系统缓冲区，以免溢出丢失
				pctrlsock->waitForBytesWritten();
		}
	}
	else if (cmdlst[0] == "getdiskinfo") {
		//获取所有disk信息并序列化，发送
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
//服务端读出控制信息,客户端读出数据
void sloter::msrvSock_Read_Data() {
	//如果是服务端，此socket读出的是控制信息
	if (Im_server) {
		ControlCmdArrivedProcedure(msrvSock,mSocket);
	}
	//如果是客户端，读出的是数据
	else {
		DataArrivedProcedure(msrvSock);
	}
}
//数据流收到数据的处理办法
void sloter::DataArrivedProcedure(QTcpSocket* ptranssock) {
	//头8个字节为文件长度
	if (recv_file_len == 0) {
		QDataStream ds(ptranssock);
		ds>>recv_file_len;
	}
	//写入到文件描述符
	file_recv_buffer.append(ptranssock->readAll());
	file_desc->write(file_recv_buffer);
	file_recv_buffer.clear();
	//更新pPBar
	unsigned long long sc = file_desc->size() + file_recv_buffer.length();
	static int c = 0;//根据进度发送进度信号的辅助变量
	if (sc / 20 * 1024 * 1024 > c || sc >= recv_file_len){
		pPBar->setValue(sc / (recv_file_len/100 )  );
		pstbar->showMessage("receiving " + save_name);
		++c;}
	//根据已写的大小判断是否写入完毕
	if ((long long)file_desc->size() + (long long)file_recv_buffer.length() >= recv_file_len) {
		file_desc->close();
		file_desc->~QFile();
		file_desc = NULL;
		emit recv_done();
		c = 0;
		recv_file_len = 0;//重置接收文件长度
	}
}
bool sloter::srv_listen() {
	return mServer->listen(QHostAddress::Any, control_port);
}
bool sloter::cli_connect(QString addr) {
	//重新连接服务端，将本实例修改为客户端
	Im_server = false;
	if (mSocket)
		mSocket->~QTcpSocket();
	mSocket = new QTcpSocket();
	mSocket->connectToHost(addr, control_port);
	if (!mSocket->waitForConnected())//连接失败
		return false;
	connect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
	//建立传送流，等待服务端连接
	mServer->close();
	if(!mServer->listen(QHostAddress::Any, transfer_port))
		return false;
}
void sloter::cli_get(QString filen,QString saveroute) {
	//断开ctrlsock上的data处理函数
	QTcpSocket* ctrl = GetCtrlSock_DisconnectSlot();
	//获取路径中的纯文件名
	QFileInfo qfi(filen);
	save_name = qfi.fileName();
	//恢复ctrlsock上的data处理函数
	RecoverSockSlot();
	//打开保存的文件描述符
	file_desc = new QFile(saveroute+"/"+save_name);
	file_desc->open(QIODevice::WriteOnly);
	//向服务器发送命令
	ctrl->write(QString("get " + EscapeRoute(filen)).toUtf8());
	ctrl->waitForBytesWritten();
	//利用qeventloop阻塞等待recv_done信号
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
	int listSize;//指示文件列表大小
	ds >> listSize;
	QList<QPair<QString, bool>>  list;
	QPair<QString, bool> tt;
	while (list.size() < listSize) {
		qint64 ti = ctrl->bytesAvailable();
		if (ti==0)//若信息尚未接收完全，则等待下一批信息到达
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
	//根据服务端指示变量判断哪个socket是控制流和传输流
	QTcpSocket* ctrl = GetCtrlSock();
	ctrl->write(("put " + EscapeRoute(file) + " " + de_EscapeRoute(addr)).toUtf8());
	ctrl->waitForBytesWritten();
}
QTcpSocket* sloter::GetCtrlSock_DisconnectSlot(){
	//根据服务端指示变量判断哪个socket是控制流和传输流
	//并且把控制处理信息函数断开，因为待会返回文件大小时可能被这个函数截获
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
	//恢复控制函数连接
	if (!Im_server)
		connect(mSocket, &QTcpSocket::readyRead, this, &sloter::mSocket_Read_Data);
	else
		connect(msrvSock, &QTcpSocket::readyRead, this, &sloter::msrvSock_Read_Data);
}
//用于客户端连接并get remote disks后，通知服务端可以get remote disks了
void sloter::TellPeerGetMyDisks() {
	//根据服务端指示变量判断哪个socket是控制流和传输流
	QTcpSocket* ctrl = GetCtrlSock();
	ctrl->write("get_my_disks");
	ctrl->waitForBytesWritten();
}
QString sloter::EscapeRoute(QString route) { return route.replace(" ", "╋"); }//随便找一个用不到的字符替代空格
QString sloter::de_EscapeRoute(QString route) { return route.replace("╋", " "); }//否则在服务端命令解析的地方，带空格的文件名会被解析成多个命令
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