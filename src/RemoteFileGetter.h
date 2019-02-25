#pragma once
#include"GetFileList.h"
#include"sloter.h"
class RemoteFileGetter :public FileListGetter {
public:
	QList<FileDesc> GetFileList(QString route);
	RemoteFileGetter(sloter*);
private:
	sloter* pslt;
};