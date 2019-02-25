#ifndef LAN_TRANS_H
#define LAN_TRANS_H

#include <QtWidgets/QMainWindow>
#include "ui_lan_trans.h"
#include"file_viewer.h"
#include"sloter.h"
#include<qregexp.h>
#include<qmessagebox.h>
#include"remote_file_viewer.h"
#include"FileView.h"

class Lan_trans : public QMainWindow
{
	Q_OBJECT

public:
	Lan_trans(QWidget *parent = 0);
	~Lan_trans();

private:
	Ui::Lan_transClass ui;
	FileView* fv1;//�����ļ���Ϣչʾ
	FileView* fv2;//Զ���ļ���Ϣչʾ
	sloter slt1;//������ؿ�����
	void btn_con_click();//���connect��ť
	void btn_estab_click();//���establish��ť����������
	void show_cli_connected();//���յ�client�����źţ���status barչʾ
	void test_proc();
	void btn_get_click();//���ذ�ť
	void btn_put_click();//�ϴ���ť
	void loadicons();//����ͼ����Դ
	QIcon icons[11];//listwidgetͼ����Դ
};

#endif // LAN_TRANS_H
