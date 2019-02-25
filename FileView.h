#pragma once
#include"ui_lan_trans.h"
#include"GetFileList.h"
#include<qicon.h>

class FileView :public QObject {
	Q_OBJECT
public:
	FileView(QLineEdit*, QListWidget*, QIcon(*icons)[], FileListGetter*);
	void UpdateList(QString route);//����listwidget
	void ShowRoute(QString route);//��listwidget��չʾ�ļ�����Ϣ������UpdateList����������lineedit
	QList<FileDesc> curfilelist;//boolָʾ�Ƿ��ļ�
	QIcon(*icons)[];//ͼ���ļ�����ָ��
private:
	QLineEdit *LineEditDir;//������lineedit��ָ��
	QListWidget *ListWidaetFile;//������listwidget��ָ��
	FileListGetter* flistgetter;//��ȡ�ļ���controller
private slots:
	void AddressBarReturnPressed();//�ڵ�ַ�����»س���չʾ��ַ�����·��
	void FileListDoubleClick(QListWidgetItem *item);//��list˫������ʾ�Ǹ��ļ�����Ϣ
};
