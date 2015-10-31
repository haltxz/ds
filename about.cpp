#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QGroupBox(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
}

About::~About()
{
    delete ui;
}
