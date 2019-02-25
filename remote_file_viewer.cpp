#include"remote_file_viewer.h"
void RemoteFileViewr::ShowDir(QString dir) {
	curfilelist = pslt->get_remote_dir_info(dir);
	LineEditDir->setText(dir);
	showFileInfoList(curfilelist);
}
RemoteFileViewr::RemoteFileViewr(QLineEdit* ple, QListWidget* plw, sloter*pt):
	LineEditDir(ple), ListWidaetFile(plw),pslt(pt){
	
	connect(LineEditDir, SIGNAL(returnPressed()), this, SLOT(le_slotShow()));
	connect(ListWidaetFile, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotShowDir(QListWidgetItem *)));

	loadicons();
}
void RemoteFileViewr::le_slotShow() {
	ShowDir(LineEditDir->text());
}
void RemoteFileViewr::slotShowDir(QListWidgetItem *item) {
	QString t = LineEditDir->text();
	QString str = item->text();
	//处理在磁盘总览进入磁盘的情况
	if (t == "all disks") {
		ShowDir(str);
		return;
	}
	//处理在磁盘根目录点击向上的情况
	if (str == ".." && QRegExp(R"(.:/)").exactMatch(LineEditDir->text())) {
		show_remote_disks();
		return;
	}
	//处理E:/之类的情况
	if (t[t.length()-1] == '/')
		t.chop(1);
	//向上一层时裁剪路径
	if (str == ".."){
		t.chop(t.length() - t.lastIndexOf("/"));
		if (t[t.length() - 1] == ':')
			t.append('/');
		ShowDir(t);
		return;}
	else if (str == ".")
		return;
	ShowDir(t+"/"+ str);
}
void RemoteFileViewr::showFileInfoList(QList<QPair<QString, bool>> list) {
	ListWidaetFile->clear();
	if (QRegExp(R"(.:/)").exactMatch(LineEditDir->text())) {
		QIcon* icon = &icons[10];
		curfilelist.push_front(QPair<QString, bool>("..", false));
		ListWidaetFile->addItem(new QListWidgetItem(*icon, ".."));
	}
	for (unsigned int i = 0; i<list.count(); i++) {
		QFileInfo temFileinfo = list[i].first;
		QString t = temFileinfo.suffix();
		QIcon* icon;
		if (!list[i].second)
			icon = &icons[0];
		else if (t == "jpg" || t == "jpeg" || t == "png" || t == "gif")
			icon = &icons[1];
		else if (t == "doc" || t == "docx")
			icon = &icons[2];
		else if (t == "xls" || t == "xlsx")
			icon = &icons[3];
		else if (t == "ppt" || t == "pptx")
			icon = &icons[4];
		else if (t == "pdf")
			icon = &icons[5];
		else if (t == "mp3" || t == "wav" || t == "m4a")
			icon = &icons[6];
		else if (t == "mp4" || t == "rmvb" || t == "rm" || t == "mkv")
			icon = &icons[7];
		else if (t == "zip" || t == "gz" || t == "rar" || t == "7z")
			icon = &icons[8];
		else
			icon = &icons[9];
		QString fileName = temFileinfo.fileName();
		QListWidgetItem *tmp = new QListWidgetItem(*icon, fileName);
		ListWidaetFile->addItem(tmp);
	}
}
void RemoteFileViewr::loadicons() {
	icons[0].addFile("./images/dir.png");
	icons[1].addFile("./images/pic.png");
	icons[2].addFile("./images/doc.png");
	icons[3].addFile("./images/xls.png");
	icons[4].addFile("./images/ppt.png");
	icons[5].addFile("./images/pdf.png");
	icons[6].addFile("./images/music.png");
	icons[7].addFile("./images/movie.png");
	icons[8].addFile("./images/tar.png");
	icons[9].addFile("./images/file.png");
	icons[10].addFile("./images/disk.png");
}
void RemoteFileViewr::show_remote_disks(){
	QIcon* icon = &icons[10];
	ListWidaetFile->clear();
	QStringList tt = pslt->get_remote_disk_info();
	for (QString my_info : tt) {
		ListWidaetFile->addItem(new QListWidgetItem(*icon, my_info));
		curfilelist.push_back(QPair<QString, bool>(my_info, false));
	}
	LineEditDir->setText("all disks");
	return;
}