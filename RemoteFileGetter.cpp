#include"RemoteFileGetter.h"

RemoteFileGetter::RemoteFileGetter(sloter* p):pslt(p) {}
QList<FileDesc> RemoteFileGetter::GetFileList(QString route) {
	QList<FileDesc> ret;
	if (route == "all disks") {
		QStringList tt = pslt->get_remote_disk_info();
		for (QString t : tt)
			ret.push_back(FileDesc(t, 10));
	}
	else {
		//遇到E:/的情况，添加".."(获取全部磁盘列表)
		if (QRegExp(R"(.:/)").exactMatch(route))
			ret.push_back(FileDesc("..", 0));
		QList<QPair<QString,bool>> list= pslt->get_remote_dir_info(route);
		for (QPair<QString, bool> ttt : list) {
			int type;
			QFileInfo tt = ttt.first;
			QString t = tt.suffix();
			if (!ttt.second)
				type = 0;
			else if (t == "jpg" || t == "jpeg" || t == "png" || t == "gif")
				type = 1;
			else if (t == "doc" || t == "docx")
				type = 2;
			else if (t == "xls" || t == "xlsx")
				type = 3;
			else if (t == "ppt" || t == "pptx")
				type = 4;
			else if (t == "pdf")
				type = 5;
			else if (t == "mp3" || t == "wav" || t == "m4a")
				type = 6;
			else if (t == "mp4" || t == "rmvb" || t == "rm" || t == "mkv")
				type = 7;
			else if (t == "zip" || t == "gz" || t == "rar" || t == "7z")
				type = 8;
			else
				type = 9;
			ret.push_back(FileDesc(ttt.first, type));
		}
	}
	return ret;
}