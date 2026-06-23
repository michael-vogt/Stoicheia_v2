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

    void setCoordinates(double pos_x, double pos_y);
    [[nodiscard]] auto coordinates() const -> QPointF;

private:
    Ui::CoordinateInputDialog* ui; // NOLINT: readabbvility-identifier-naming - Attribut MUSS ui heißen, da es sonst nicht mit dem UI-File zusammenpasst
};
