#ifndef ABOUT_H
#define ABOUT_H

#include <QGroupBox>

namespace Ui {
class About;
}

class About : public QGroupBox
{
    Q_OBJECT

public:
    explicit About(QWidget *parent = 0);
    ~About();

private:
    Ui::About *ui;
};

#endif // ABOUT_H
