#pragma once
#include <QDialog>
#include "AppSettings.h"

class QTabWidget;
class QDoubleSpinBox;
class QCheckBox;
class ColorButton;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(AppSettings& settings, QWidget* parent = nullptr);

    signals:
        void settingsChanged();

private slots:
    void apply();
    void resetToDefaults();

private:
    QWidget* buildGridTab();
    QWidget* buildColorsTab();

    void readFromSettings();
    void writeToSettings();

    AppSettings& m_settings;

    // Grid-Tab
    QCheckBox*      m_gridVisible  = nullptr;
    QDoubleSpinBox* m_gridSpacing  = nullptr;
    QCheckBox*      m_snapEnabled  = nullptr;
    ColorButton*    m_axisColor    = nullptr;
    ColorButton*    m_gridColor    = nullptr;
    ColorButton*    m_labelColor   = nullptr;

    // Farb-Tab
    ColorButton* m_background   = nullptr;
    ColorButton* m_point        = nullptr;
    ColorButton* m_pointFill    = nullptr;
    ColorButton* m_line         = nullptr;
    ColorButton* m_circle       = nullptr;
    ColorButton* m_selected     = nullptr;
    ColorButton* m_highlighted  = nullptr;
    ColorButton* m_construction = nullptr;
    ColorButton* m_watermark    = nullptr;
};