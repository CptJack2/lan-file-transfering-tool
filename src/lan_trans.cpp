#include "lan_trans.h"
#include"LocalFileGetter.h"
#include"RemoteFileGetter.h"
#include<qprogressbar.h>

Lan_trans::Lan_trans(QWidget *parent)
	: QMainWindow(parent){
	ui.setupUi(this);
	
	loadicons();
	//viewer，controller初始化
	fv1 = new FileView(ui.lineEdit_2, ui.listWidget, (QIcon(*)[])&icons,
		new LocalFileGetter());
	fv1->ShowRoute(QDir::currentPath());
	fv2 = new FileView(ui.lineEdit_3, ui.listWidget_2, (QIcon(*)[])&icons,
		new RemoteFileGetter(&slt1));
	slt1.pRemotefv = fv2;
	slt1.pPBar = ui.progressBar;
	slt1.pstbar = ui.statusBar;

	connect(ui.pushButton_2, &QPushButton::clicked, this, &Lan_trans::btn_con_click);
	connect(ui.pushButton, &QPushButton::clicked, this, &Lan_trans::btn_estab_click);
	connect(&slt1, &sloter::cli_connected, this, &Lan_trans::show_cli_connected);
	connect(ui.pushButton_3, &QPushButton::clicked, this, &Lan_trans::test_proc);
	connect(ui.pushButton_4, &QPushButton::clicked, this, &Lan_trans::btn_put_click);
	connect(ui.pushButton_5, &QPushButton::clicked, this, &Lan_trans::btn_get_click);
}	
Lan_trans::~Lan_trans(){

}
void Lan_trans::btn_estab_click() {	
	if (slt1.srv_listen())
		ui.statusBar->showMessage("successfully established",2000);
	else
		ui.statusBar->showMessage("something went wrong when establishing...",2000);
}
void Lan_trans::btn_con_click() {
	if (!QRegExp(R"(\d+\.\d+\.\d+\.\d+)").exactMatch(ui.lineEdit->text()))
		QMessageBox::critical(NULL, "mistake", "wrong ip address!");
	else
		if(slt1.cli_connect(ui.lineEdit->text())){
			ui.statusBar->showMessage("successfully connected",2000);
			fv2->ShowRoute("all disks");
			slt1.TellPeerGetMyDisks();
		}
		else
			ui.statusBar->showMessage("something went wrong when connecting...",2000);
}
void Lan_trans::show_cli_connected(){ 	
	ui.statusBar->showMessage("client has connected", 2000); 
}
void Lan_trans::btn_get_click() {
	int k = ui.listWidget_2->currentIndex().row();
	//k用于指示选中的要获取的文件，k==-1代表未选中
	if (k == -1 || fv2->curfilelist[k].type == 0)
		return;
	QString filen = ui.lineEdit_3->text() + "/" + fv2->curfilelist[k].name;
	if (ui.lineEdit_2->text().isEmpty() || ui.lineEdit_2->text() == "all disks")
		return;
	
	slt1.AddToGetQueue(filen, ui.lineEdit_2->text());
	ui.statusBar->showMessage(filen + " added to queue", 2000);
}
void Lan_trans::btn_put_click() {
	QListWidgetItem *t = ui.listWidget->currentItem();
	int k = ui.listWidget->currentIndex().row();
	//未选中，上传文件夹或磁盘
	if (k == -1 || fv1->curfilelist[k].type == 0
		|| fv1->curfilelist[k].type == 10)
		return;
	QString filen = ui.lineEdit_2->text() + "/" + fv1->curfilelist[k].name;
	if (ui.lineEdit_3->text().isEmpty() || ui.lineEdit_3->text() == "all disks")
		return;
	slt1.cli_put(filen, ui.lineEdit_3->text());
	int c = 1;
}
void Lan_trans::test_proc() {
	//slt1.mem_sending_opr_can_take = ui.lineEdit->text().toInt();
}
void Lan_trans::loadicons() {
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