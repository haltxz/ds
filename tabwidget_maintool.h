#ifndef TABWIDGET_MAINTOOL_H
#define TABWIDGET_MAINTOOL_H

#include <QPushButton>
#include <QTabWidget>

namespace Ui {
class TabWidget_MainTool;
}

class TabWidget_MainTool : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget_MainTool(QWidget *parent = 0);
    ~TabWidget_MainTool();
signals:
    void ButtonClicked(QString name, QString title);

private slots:
    void on_pushButton_clicked();


private:
    Ui::TabWidget_MainTool *ui;
    QList<QPushButton*> btnList;
};

#endif // TABWIDGET_MAINTOOL_H
