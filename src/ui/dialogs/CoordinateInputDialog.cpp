#include "CoordinateInputDialog.h"

#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QPushButton>

CoordinateInputDialog::CoordinateInputDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Koordinaten eingeben"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    m_xSpin = new QDoubleSpinBox(this);
    m_xSpin->setRange(-1e6, 1e6);
    m_xSpin->setDecimals(4);
    m_xSpin->setSingleStep(1.0);

    m_ySpin = new QDoubleSpinBox(this);
    m_ySpin->setRange(-1e6, 1e6);
    m_ySpin->setDecimals(4);
    m_ySpin->setSingleStep(1.0);

    auto* form = new QFormLayout;
    form->addRow(tr("X:"), m_xSpin);
    form->addRow(tr("Y:"), m_ySpin);

    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);

    // X-Feld beim Öffnen direkt auswählen
    m_xSpin->selectAll();
    m_xSpin->setFocus();
}

void CoordinateInputDialog::setCoordinates(double x, double y) {
    m_xSpin->setValue(x);
    m_ySpin->setValue(y);
    m_xSpin->selectAll();
    m_xSpin->setFocus();
}

QPointF CoordinateInputDialog::coordinates() const {
    return { m_xSpin->value(), m_ySpin->value() };
}
