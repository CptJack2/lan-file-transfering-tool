#pragma once
#include<qstring.h>
//文件信息类型
struct FileDesc{
	QString name;
	//0-dir,1-pic,2-doc,3-xls,4-ppt,5-pdf
	//6-music,7-movie,8-compressed,9-other files
	//10-disk
	int type;
	FileDesc(QString n, int t) :name(n),type(t){}
};
//获取文件信息的controller抽象类
class FileListGetter {
public:
	virtual QList<FileDesc> GetFileList(QString route) = 0;
};