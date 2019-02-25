#pragma once

#include"ui_lan_trans.h"
#include <QtGui>
#include"sloter.h"

class RemoteFileViewr :public QObject {
	Q_OBJECT
public:
	RemoteFileViewr(QLineEdit*, QListWidget*,sloter*);
	void showFileInfoList(QList<QPair<QString, bool>> list);
	void ShowDir(QString);
	void show_remote_disks();
	QList<QPair<QString, bool>> curfilelist;//bool指示是否文件
private:
	QLineEdit *LineEditDir;
	QListWidget *ListWidaetFile;
	sloter* pslt;
	QIcon icons[11];
	void loadicons();
private slots:
	void le_slotShow();
	void slotShowDir(QListWidgetItem *item);
};
