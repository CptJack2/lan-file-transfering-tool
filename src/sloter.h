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
#include<qprogressbar.h>
#include<qstatusbar.h>
#include<deque>
#include"GetFileThread.h"
class FileView;

class sloter : public QObject
{
	Q_OBJECT

private:
	int control_port = 6666;//������Ĭ�϶˿ں�
	int transfer_port = 6667;//������Ĭ�϶˿ں�
	QTcpServer* mServer = NULL;//�����׽���
	QTcpSocket* msrvSock = NULL;//�ݴ������Ϸ���˵Ŀͻ��׽��֣��ͻ��˴������ݣ�����˴�������
	QTcpSocket* mSocket = NULL;//�����׽���,�ͻ��˴����������˴�������
	bool Im_server = true;//ָʾ��ǰ�����Ƿ���˻��ǿͻ��ˣ�Ϊtrueʱ��mServerΪ��������mSocketΪ������
	QString save_name;//������������getָ����ر�����ļ����ݴ����
	QByteArray file_recv_buffer;//������������getָ����ر����ļ���buffer
	unsigned long long recv_file_len=0;//������������getָ��ݴ�������ļ�����
	QFile* file_desc;//������������getָ����ر����ļ���������
	SendFileThread* psendfilethread;
	GetFileThread* pgetfilethread;
	std::deque<QString> sendtaskqueue;//todo ���ƴ���������л�
	std::deque<QString> gettaskqueue;//get�������

	void ControlCmdArrivedProcedure(QTcpSocket*, QTcpSocket*);//�������յ�����ʱ�Ĵ���취
	void DataArrivedProcedure(QTcpSocket*);//�������յ����ݵĴ���취
	QTcpSocket* GetCtrlSock_DisconnectSlot();//�����ж�msrvSock��mSocket�ĸ��ǿ����������Ͽ���Ӧ��
	QTcpSocket* GetCtrlSock();//��ȡ������
	QTcpSocket* GetTransSock();//��ȡ������
	void RecoverSockSlot();//�ָ�readyread�Ĳ�
	QString EscapeRoute(QString);//��·���еĿո�ת���@�������ڽ��ն˵�����������̻Ὣ���ո��·���������ɶ������
	QString de_EscapeRoute(QString);//��·���е�@��ת��ɿո�
	void SendFileFin();//�����߳���ɺ󣬴˺������������߳�
	
signals:
	void cli_connected();
	void recv_done();
	void progress(int);
public slots:
	void new_client();//mServer�����Ӵ�����
	void msrvSock_Read_Data();
	void mSocket_Read_Data();
	bool cli_connect(QString addr);//����ʵ�����ͻ��ˣ����ӷ�����
	bool srv_listen();//����ʵ��������ˣ���������������
	void cli_get(QString,QString);//����get����
	void cli_put(QString, QString);//����put����
	void progress_slot(int);
public:
	sloter();
	int get_control_port(); 
	int get_transfer_port();
	QList<QPair<QString, bool>>  get_remote_dir_info(QString);
	QList<QString>  get_remote_disk_info();
	void TellPeerGetMyDisks();//�����Է����Ի�ȡ�ҷ�disks info��ָ��
	FileView* pRemotefv;//Զ��fileview��ָ�룬���ڷ���˻�ȡ�ͻ��˵�disk info
	QProgressBar* pPBar;//ʺһ������϶�
	QStatusBar* pstbar;
	void AddToGetQueue(QString, QString);//��ӵ�getfilethread�Ķ��еİ�װ����
};