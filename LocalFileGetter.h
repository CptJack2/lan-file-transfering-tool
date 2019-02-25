#pragma once
#include"GetFileList.h"
#include<qfileinfo.h>
#include<qdir.h>
class LocalFileGetter :public FileListGetter {
public:
	QList<FileDesc> GetFileList(QString route);
};