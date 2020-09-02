#ifndef ACCOUNTADDER_H
#define ACCOUNTADDER_H

#include <QMainWindow>

namespace Ui {
class AccountAdder;
}

class AccountAdder : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountAdder(QStringList accountFileList, QString Lang, QWidget *parent = nullptr);
    ~AccountAdder();
    void changeLang(QString Lang);

signals:
    void createAccount(QString displayedName, QString username, QString password, QString lang, QString gfuid, QString cache, QString filename);
    void changedLang(QString lang);

private slots:
    void on_PB_ADD_clicked();

    void on_CB_LANG_currentIndexChanged(const QString &arg1);

private:
    Ui::AccountAdder *ui;
};

#endif // ACCOUNTADDER_H
