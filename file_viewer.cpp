#include "file_viewer.h"

FileViewr::FileViewr(QLineEdit* ple, QListWidget* plw)
	:LineEditDir(ple), ListWidaetFile(plw) {

	connect(LineEditDir, SIGNAL(returnPressed()), this, SLOT(le_slotShow()));
	connect(ListWidaetFile, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(slotShowDir(QListWidgetItem *)));

	loadicons();
}
FileViewr::~FileViewr(){

}
void FileViewr::le_slotShow() {
	ShowDir(QDir(LineEditDir->text()));
}
/*
* slotShow 负责显示dir目录下的所有文件
**/
void FileViewr::ShowDir(QDir dir) {
	QStringList string;
	string << "*";
	QFileInfoList list = dir.entryInfoList(string, QDir::AllEntries, QDir::DirsFirst);
	curfilelist.clear();
	for (QFileInfo t : list) 
		curfilelist.push_back( QPair<QString, bool>(t.fileName(), t.isFile()));
	LineEditDir->setText(dir.absolutePath());
	showFileInfoList(list);
}
void FileViewr::slotShowDir(QListWidgetItem *item) {
	QString str = item->text();
	QDir dir;
	//处理在磁盘总览进入磁盘的情况
	if (LineEditDir->text() == "all disks") {
		dir.setPath(str);
		ShowDir(dir);
		return;
	}
	//处理在磁盘根目录点击向上的情况
	if (str==".." && QRegExp(R"(.:/)").exactMatch(LineEditDir->text())) {
		QIcon* icon=&icons[10];
		ListWidaetFile->clear();
		for(QFileInfo my_info:QDir::drives()){
			ListWidaetFile->addItem(new QListWidgetItem(*icon, my_info.absolutePath()));
			curfilelist.clear();
			curfilelist.push_back(QPair<QString, bool>(my_info.absolutePath(), false));
		}
		LineEditDir->setText("all disks");
		return;
	}
	dir.setPath(LineEditDir->text());
	dir.cd(str);
	//LineEditDir->setText(dir.absolutePath());
	ShowDir(dir);
}
void FileViewr::showFileInfoList(QFileInfoList list) {
	ListWidaetFile->clear();
	if (QRegExp(R"(.:/)").exactMatch(LineEditDir->text())) {
		QIcon* icon=&icons[10]; 
		curfilelist.push_front(QPair<QString, bool>("..", false));
		ListWidaetFile->addItem(new QListWidgetItem(*icon, ".."));
	}
	for (unsigned int i = 0; i<list.count(); i++) {
		QFileInfo temFileinfo = list.at(i);
		QString t = temFileinfo.suffix();
		QIcon* icon;
		if (temFileinfo.isDir())
			icon = &icons[0];
		else if(t=="jpg" || t=="jpeg" || t == "png" || t == "gif")
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
void FileViewr::loadicons() {
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