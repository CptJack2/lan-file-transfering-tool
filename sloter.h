#pragma once
#include <QTcpServer>  
#include <QTcpSocket>
#include<qobject.h>
#include<qfile.h>
#include<qfileinfo.h>
#include<qtextcodec.h>
#include<qeventloop.h>
#include <QtGui>
#include<qdatastream.h>
#include<qlineedit.h>
#include"SendFileThread.h"
class FileView;

class sloter : public QObject
{
	Q_OBJECT

private:
	//QTcpServer* server;
	//QTcpSocket* socket;
	int control_port = 6666;//控制流默认端口号
	int transfer_port = 6667;//数据流默认端口号
	QTcpServer* mServer = NULL;//侦听套接字
	QTcpSocket* msrvSock = NULL;//暂存连接上服务端的客户套接字，客户端传输数据，服务端传输命令
	QTcpSocket* mSocket = NULL;//连接套接字,客户端传输命令，服务端传输数据
	bool Im_server = true;//指示当前进程是服务端还是客户端，为true时，mServer为控制流，mSocket为传输流
	QString save_name;//当控制流发出get指令，本地保存的文件名暂存变量
	QByteArray file_recv_buffer;//当控制流发出get指令，本地保存文件的buffer
	int recv_file_len;//当控制流发出get指令，暂存待保存文件长度
	QFile* file_desc;//当控制流发出get指令，本地保存文件的描述符
	void ControlCmdArrivedProcedure(QTcpSocket*, QTcpSocket*);//控制流收到命令时的处理办法
	void DataArrivedProcedure(QTcpSocket*);//数据流收到数据的处理办法
	QTcpSocket* GetCtrlSock_DisconnectSlot();//用于判断msrvSock，mSocket哪个是控制流，并断开相应槽
	void RecoverSockSlot();//恢复readyread的槽
	QString EscapeRoute(QString);//将路径中的空格转义成@，否则在接收端的命令解析过程会将含空格的路径名解析成多个命令
	QString de_EscapeRoute(QString);//将路径中的@反转义成空格
	SendFileThread* psendfilethread;
	void SendFileFin();//传输线程完成后，此函数负责销毁线程
signals:
	void cli_connected();
	void transfer_done();
public slots:
	void new_client();//mServer新连接处理函数
	void msrvSock_Read_Data();
	void mSocket_Read_Data();
	bool cli_connect(QString addr);//将本实例作客户端，连接服务器
	bool srv_listen();//将本实例作服务端，启动服务器侦听
	void cli_get(QString,QString);//发出get命令
	void cli_put(QString, QString);//发出put命令
public:
	//QLineEdit* pRouteEdit;//存储路径的line edit
	//void parsecmd(QString);
	sloter();
	int get_control_port(); 
	int get_transfer_port();
	QList<QPair<QString, bool>>  get_remote_dir_info(QString);
	QList<QString>  get_remote_disk_info();
	void TellPeerGetMyDisks();//发出对方可以获取我方disks info的指令
	FileView* pRemotefv;//远端fileview的指针，用于服务端获取客户端的disk info
};