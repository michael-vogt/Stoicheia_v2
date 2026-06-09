#pragma once
#include <QPushButton>
#include <QColor>

constexpr int DEFAULT_BUTTON_WIDTH = 40;
constexpr int DEFAULT_BUTTON_HEIGHT = 24;

class ColorButton : public QPushButton {
    Q_OBJECT
public:
    explicit ColorButton(QWidget* parent = nullptr);
    explicit ColorButton(const QColor& color, QWidget* parent = nullptr);

    [[nodiscard]] auto color() const -> QColor { return m_color; }
    void setColor(const QColor& color);

signals:
    void colorChanged(const QColor& color);

private:
    void updateAppearance();
    QColor m_color;
};