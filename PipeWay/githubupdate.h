#ifndef GITHUBUPDATE_H
#define GITHUBUPDATE_H

#include <QMainWindow>
#include "networkrequester.h"

namespace Ui {
class GithubUpdate;
}

class GithubUpdate : public QMainWindow
{
    Q_OBJECT

public:
    explicit GithubUpdate(QMainWindow *parent = nullptr);
    ~GithubUpdate();

private:
    Ui::GithubUpdate *ui;

    const QString VERSION = "1.6";
};

#endif // GITHUBUPDATE_H
