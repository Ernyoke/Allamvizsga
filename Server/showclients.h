#ifndef SHOWCLIENTS_H
#define SHOWCLIENTS_H

#include <QMainWindow>
#include "clientmodel.h"

namespace Ui {
class ShowClients;
}

class ShowClients : public QMainWindow
{
    Q_OBJECT

public:
    explicit ShowClients(ClientModel *tableModel, QWidget *parent = 0);
    ~ShowClients();

private:
    Ui::ShowClients *ui;

    ClientModel *model;
};

#endif // SHOWCLIENTS_H
