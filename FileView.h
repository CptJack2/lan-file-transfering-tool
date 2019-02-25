#pragma once
#include"ui_lan_trans.h"
#include"GetFileList.h"
#include<qicon.h>

class FileView :public QObject {
	Q_OBJECT
public:
	FileView(QLineEdit*, QListWidget*, QIcon(*icons)[], FileListGetter*);
	void UpdateList(QString route);//更新listwidget
	void ShowRoute(QString route);//在listwidget里展示文件夹信息（调用UpdateList），并更新lineedit
	QList<FileDesc> curfilelist;//bool指示是否文件
	QIcon(*icons)[];//图标文件数组指针
private:
	QLineEdit *LineEditDir;//包含的lineedit的指针
	QListWidget *ListWidaetFile;//包含的listwidget的指针
	FileListGetter* flistgetter;//获取文件的controller
private slots:
	void AddressBarReturnPressed();//在地址栏按下回车，展示地址栏里的路径
	void FileListDoubleClick(QListWidgetItem *item);//在list双击，显示那个文件夹信息
};
