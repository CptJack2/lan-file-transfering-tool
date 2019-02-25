#include "FileView.h"

FileView::FileView(QLineEdit* ple, QListWidget* plw,QIcon(*p)[], FileListGetter*pf)
	:LineEditDir(ple), ListWidaetFile(plw), icons(p), flistgetter(pf) {

	connect(LineEditDir, SIGNAL(returnPressed()), this, SLOT(AddressBarReturnPressed()));
	connect(ListWidaetFile, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(FileListDoubleClick(QListWidgetItem *)));

}
void FileView::AddressBarReturnPressed() {	UpdateList(LineEditDir->text());}
void FileView::FileListDoubleClick(QListWidgetItem *item) {
	QString t = LineEditDir->text();
	QString str = item->text();
	//�����ڴ�������������̵����
	if (t == "all disks") {
		ShowRoute(str);
		return;
	}
	//�����ڴ��̸�Ŀ¼������ϵ����
	if (str == ".." && QRegExp(R"(.:/)").exactMatch(t)) {
		ShowRoute("all disks");
		return;
	}
	//����E:/֮������
	if (t[t.length() - 1] == '/')
		t.chop(1);
	//����һ��ʱ�ü�·��
	if (str == "..") {
		t.chop(t.length() - t.lastIndexOf("/"));
		if (t[t.length() - 1] == ':')
			t.append('/');
		ShowRoute(t);
		return;
	}
	else if (str == ".")
		return;
	ShowRoute(t + "/" + str);
}
void FileView::UpdateList(QString dir) {
	ListWidaetFile->clear();
	curfilelist = flistgetter->GetFileList(dir);
	for (unsigned int i = 0; i<curfilelist.count(); ++i) {
		QIcon* icon= &(*icons)[curfilelist[i].type];
		QListWidgetItem *tmp = new QListWidgetItem(*icon, curfilelist[i].name);
		ListWidaetFile->addItem(tmp);
	}
}
void FileView::ShowRoute(QString route) {
	UpdateList(route);
	LineEditDir->setText(route);
}