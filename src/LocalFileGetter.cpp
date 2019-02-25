#include"LocalFileGetter.h"
#include<qregexp.h>

QList<FileDesc> LocalFileGetter::GetFileList(QString route) {
	QList<FileDesc> ret;
	if (route == "all disks"){
		for (QFileInfo my_info : QDir::drives()) 
			ret.push_back(FileDesc(my_info.absolutePath(), 10));}
	else {
		//����E:/����������..����ʾ��ȡȫ�������б�
		if (QRegExp(R"(.:/)").exactMatch(route))
			ret.push_back(FileDesc("..", 0));
		QStringList string;
		string << "*";
		QFileInfoList list = QDir(route).entryInfoList(string, QDir::AllEntries, QDir::DirsFirst);
		for (QFileInfo tt : list) {
			int type; 
			QString t = tt.suffix();
			if (tt.isDir())
				type=0;
			else if (t == "jpg" || t == "jpeg" || t == "png" || t == "gif")
				type=1;
			else if (t == "doc" || t == "docx")
				type=2;
			else if (t == "xls" || t == "xlsx")
				type=3;
			else if (t == "ppt" || t == "pptx")
				type=4;
			else if (t == "pdf")
				type=5;
			else if (t == "mp3" || t == "wav" || t == "m4a")
				type=6;
			else if (t == "mp4" || t == "rmvb" || t == "rm" || t == "mkv")
				type=7;
			else if (t == "zip" || t == "gz" || t == "rar" || t == "7z")
				type=8;
			else
				type=9;
			ret.push_back(FileDesc(tt.fileName(), type));
		}
	}
	return ret;
}