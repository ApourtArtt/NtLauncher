#ifndef UPDATEWIDGET_H
#define UPDATEWIDGET_H

#include <QWidget>
#include <QJsonObject>
#include "ui_updatewidget.h"

namespace Ui {
class UpdateWidget;
}

class UpdateWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateWidget(QJsonObject jsonObj, QWidget *parent = nullptr);
    ~UpdateWidget();

private:
    Ui::UpdateWidget *ui;
};

#endif // UPDATEWIDGET_H
