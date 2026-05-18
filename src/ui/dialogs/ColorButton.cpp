#include "ColorButton.h"
#include <QColorDialog>

ColorButton::ColorButton(const QColor& color, QWidget* parent)
    : QPushButton(parent), m_color(color)
{
    setFixedSize(40, 24);
    updateAppearance();
    connect(this, &QPushButton::clicked, [this]() {
        QColor c = QColorDialog::getColor(
            m_color, this, tr("Farbe wählen"),
            QColorDialog::ShowAlphaChannel);
        if (c.isValid())
            setColor(c);
    });
}

void ColorButton::setColor(const QColor& color) {
    if (m_color == color) return;
    m_color = color;
    updateAppearance();
    emit colorChanged(m_color);
}

void ColorButton::updateAppearance() {
    setStyleSheet(QString(
        "QPushButton { background-color: %1; border: 1px solid #888; }"
        "QPushButton:hover { border: 2px solid #444; }")
        .arg(m_color.name(QColor::HexArgb)));
}