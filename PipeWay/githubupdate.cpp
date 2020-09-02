#include "githubupdate.h"
#include "ui_githubupdate.h"

GithubUpdate::GithubUpdate(QMainWindow *parent) :
    QMainWindow(parent),
    ui(new Ui::GithubUpdate)
{
    NetworkRequester netReq;
    QNetworkRequest req(QUrl("https://api.github.com/repos/ApourtArtt/NtLauncher/releases/latest"));
    QByteArray response = netReq.get(req);
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QJsonObject jsonObj = jsonDoc.object();
    if(jsonObj.value(QString("tag_name")).toString() > VERSION)
    {
        ui->setupUi(this);
        setWindowIcon(QIcon(":/images/chaussons.png"));
        QString ver = jsonObj.value(QString("tag_name")).toString();
        QString notes = "Summary :\n" + jsonObj.value(QString("name")).toString() + "\n\nChangelog :\n"
                + jsonObj.value(QString("body")).toString();
        QString prelink = jsonObj.value(QString("html_url")).toString();
        QString link = "<a href=\"" + prelink + "\">" + prelink + "</a>";
        ui->L_VERSION->setText(ver);
        ui->LE_NOTE->insertPlainText(notes);
        ui->L_LINKRELEASE->setText(link);
        QTimer::singleShot(500, [=]{show();});
    }
}

GithubUpdate::~GithubUpdate()
{
    delete ui;
}
