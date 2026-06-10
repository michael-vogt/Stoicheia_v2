#include "ColorButton.h"
#include <QColorDialog>
#include "../../Constants.h"


using namespace Constants;

ColorButton::ColorButton(QWidget* parent) : ColorButton(Qt::white, parent) {}

ColorButton::ColorButton(const QColor& color, QWidget* parent)
    : QPushButton(parent), m_color(color)
{
    setFixedSize(LayoutConstants::COLORBUTTON_WIDTH, LayoutConstants::COLORBUTTON_HEIGHT);
    updateAppearance();
    connect(this, &QPushButton::clicked, [this]() -> void {
        QColor color = QColorDialog::getColor(
            m_color, this, tr("Farbe wählen"),
            QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            setColor(color);
        }
    });
}

void ColorButton::setColor(const QColor& color) {
    if (m_color == color) {
        return;
    }
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