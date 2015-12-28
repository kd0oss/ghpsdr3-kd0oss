#include "smeter.h"
#include "Panadapter.h"
#include "UI.h"


sMeter::sMeter(QWidget* parent) : QFrame(parent)
{
    sMeterMain=new Meter("Main Rx", SIGMETER);
    sMeterSub=new Meter("Sub Rx", SIGMETER);
    meter_dbm = -121;
    sub_meter_dbm = -121;
    subRx = FALSE;
}

sMeter::~sMeter()
{

}

void sMeter::setSubRxState(bool state)
{
    subRx=state;
}

void sMeter::paintEvent(QPaintEvent*)
{
//qDebug() << "smeter.cpp - Meter value is equal to " << meter_dbm;
//return;
    // Draw the Main Rx S-Meter
    QPainter painter(this);
    QImage image=sMeterMain->getImage(meter_dbm, sub_meter_dbm);
    painter.drawImage(4,3,image);

    // Draw the Sub Rx S-Meter
    if (subRx)
    {
        image=sMeterSub->getImage(sub_meter_dbm, meter_dbm);
        painter.drawImage(4,image.height()+1,image);
    }
}
