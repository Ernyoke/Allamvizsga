#include "showclients.h"
#include "ui_showclients.h"

ShowClients::ShowClients(ClientModel *tableModel, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShowClients)
{
    ui->setupUi(this);
    this->model = tableModel;
    this->ui->clientTable->setModel(model);
}

ShowClients::~ShowClients()
{
    delete ui;
}
