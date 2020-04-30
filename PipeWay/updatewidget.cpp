#include "updatewidget.h"

UpdateWidget::UpdateWidget(QJsonObject jsonObj, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UpdateWidget)
{
    ui->setupUi(this);
    QString ver = jsonObj.value(QString("tag_name")).toString();
    QString notes = "Summary :\n" + jsonObj.value(QString("name")).toString() + "\n\nChangelog :\n"
                    + jsonObj.value(QString("body")).toString();
    QString prelink = jsonObj.value(QString("html_url")).toString();
    QString link = "<a href=\"" + prelink + "\">" + prelink + "</a>";
    ui->L_VERSION->setText(ver);
    ui->LE_NOTE->insertPlainText(notes);
    ui->L_LINKRELEASE->setText(link);
}

UpdateWidget::~UpdateWidget()
{
    delete ui;
}
