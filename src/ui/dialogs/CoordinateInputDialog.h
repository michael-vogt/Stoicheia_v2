#pragma once
#include <QDialog>
#include <QPointF>

class QDoubleSpinBox;

class CoordinateInputDialog : public QDialog {
    Q_OBJECT

public:
    explicit CoordinateInputDialog(QWidget* parent = nullptr);

    void setCoordinates(double x, double y);
    QPointF coordinates() const;

private:
    QDoubleSpinBox* m_xSpin;
    QDoubleSpinBox* m_ySpin;
};
