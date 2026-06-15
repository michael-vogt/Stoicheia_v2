#include "CoordinateInputDialog.h"
#include "ui_CoordinateInputDialog.h"

#include <QDoubleSpinBox>
#include <QPushButton>

CoordinateInputDialog::CoordinateInputDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::CoordinateInputDialog)
{
    ui->setupUi(this);

    //setWindowTitle(tr("Koordinaten eingeben"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    ui->labelX->setFocusPolicy(Qt::NoFocus);
    ui->labelY->setFocusPolicy(Qt::NoFocus);

    ui->xEdit->setValidator(new QDoubleValidator(-1e6, 1e6, 0, this));
    ui->yEdit->setValidator(new QDoubleValidator(-1e6, 1e6, 0, this));

    // X-Feld beim Öffnen direkt auswählen
    ui->xEdit->selectAll();
    ui->xEdit->setFocus();
}

CoordinateInputDialog::~CoordinateInputDialog() {
    delete ui;
}

void CoordinateInputDialog::setCoordinates(double x, double y) {
    ui->xEdit->setText(QString::number(x));
    ui->yEdit->setText(QString::number(y));
    ui->xEdit->selectAll();
    ui->xEdit->setFocus();
}

QPointF CoordinateInputDialog::coordinates() const {
    return { ui->xEdit->text().toDouble(), ui->yEdit->text().toDouble() };
}
