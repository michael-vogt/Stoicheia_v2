#pragma once
#include <QDialog>
#include <QDoubleValidator>
#include <QPointF>

namespace Ui { class CoordinateInputDialog; }

class CoordinateInputDialog : public QDialog {
    Q_OBJECT

public:
    explicit CoordinateInputDialog(QWidget* parent = nullptr);
    ~CoordinateInputDialog() override;

    void setCoordinates(double x, double y);
    QPointF coordinates() const;

private:
    Ui::CoordinateInputDialog* ui;
};
