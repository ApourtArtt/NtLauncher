#include "accountadder.h"
#include "ui_accountadder.h"

AccountAdder::AccountAdder(QStringList accountFileList, QString Lang, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AccountAdder)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/images/chaussons.png"));
    ui->CB_LANG->setCurrentIndex(ui->CB_LANG->findText(Lang));
    ui->CB_ACCOUNTFILE->addItems(accountFileList);
    if(accountFileList.isEmpty())
        ui->CB_ACCOUNTFILE->addItem("accounts.acc");
}

AccountAdder::~AccountAdder()
{
    delete ui;
}

void AccountAdder::changeLang(QString Lang)
{
    ui->CB_LANG->setCurrentIndex(ui->CB_LANG->findText(Lang));
}

void AccountAdder::on_PB_ADD_clicked()
{
    QString fn = ui->CB_ACCOUNTFILE->currentText();
    fn = fn.endsWith(".acc") ? fn : fn + ".acc";
    emit createAccount(ui->LE_NICKNAME->text(), ui->LE_USERNAME->text(), ui->LE_PASSWORD->text(),
                       ui->CB_LANG->currentText(), ui->LE_GFUID->text(), ui->LE_CACHE->text(), fn);
}

void AccountAdder::on_CB_LANG_currentIndexChanged(const QString &arg1)
{
    emit changedLang(arg1);
}
