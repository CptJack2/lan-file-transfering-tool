#pragma once

#include"ui_lan_trans.h"
#include <QtGui>

class FileViewr:public QObject{
	Q_OBJECT
public:
	FileViewr(QLineEdit* , QListWidget*);
	~FileViewr();
	void showFileInfoList(QFileInfoList list);
	void ShowDir(QDir dir); 
	QList<QPair<QString, bool>> curfilelist;//bool指示是否文件
private:
	QLineEdit *LineEditDir;
	QListWidget *ListWidaetFile;
	QIcon icons[11];
	void loadicons();
private slots:
	void le_slotShow();
	void slotShowDir(QListWidgetItem *item);
};
