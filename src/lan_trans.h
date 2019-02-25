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
	FileView* fv1;//本地文件信息展示
	FileView* fv2;//远端文件信息展示
	sloter slt1;//网络相关控制类
	void btn_con_click();//点击connect按钮
	void btn_estab_click();//点击establish按钮，开启侦听
	void show_cli_connected();//接收到client连接信号，在status bar展示
	void test_proc();
	void btn_get_click();//下载按钮
	void btn_put_click();//上传按钮
	void loadicons();//加载图标资源
	QIcon icons[11];//listwidget图标资源
};

#endif // LAN_TRANS_H
