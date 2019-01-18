#include "hermesframe.h"
#include "ui_hermesframe.h"

HermesFrame::HermesFrame(QWidget *parent) : QFrame(parent), ui(new Ui::HermesFrame)
{
    ui->setupUi(this);
} // end constructor


HermesFrame::~HermesFrame()
{
    delete ui;
} // end destructor
