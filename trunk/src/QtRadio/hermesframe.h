#ifndef HERMESFRAME_H
#define HERMESFRAME_H

#include <QFrame>

namespace Ui {
class HermesFrame;
}

class HermesFrame : public QFrame
{
    Q_OBJECT

public:
    explicit HermesFrame(QWidget *parent = 0);
    ~HermesFrame();

private:
    Ui::HermesFrame *ui;
};

#endif // HERMESFRAME_H
