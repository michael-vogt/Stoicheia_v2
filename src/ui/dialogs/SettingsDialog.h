#pragma once
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <qspinbox.h>

#include "AppSettings.h"

class QTabWidget;
class QDoubleSpinBox;
class QCheckBox;
class ColorButton;

namespace Ui { class SettingsDialog; }

/*static const QHash<QString, QString> languageMap = {
    {"en", "English"},
    {"de", "Deutsch" }
};*/
static QHash<QString, QString> buildLanguageMap(const QString& path);

class SettingsDialog : public QDialog {
    Q_OBJECT

    Ui::SettingsDialog* ui;

public:
    explicit SettingsDialog(AppSettings& settings, QWidget* parent = nullptr);

    signals:
        void settingsChanged();

private slots:
    void apply();
    void resetToDefaults();

private:
    /*QWidget* buildUITab();
    QWidget* buildGridTab();
    QWidget* buildColorsTab();*/

    void readFromSettings();
    void writeToSettings();

    AppSettings& m_settings;

    // UI-Tab
    /*QSpinBox*  m_uiRecentMaxCount = nullptr;
    QComboBox* m_uiLanguage       = nullptr;*/

    // Grid-Tab
    /*QCheckBox*      m_gridVisible  = nullptr;
    QDoubleSpinBox* m_gridSpacing  = nullptr;
    QCheckBox*      m_snapEnabled  = nullptr;
    ColorButton*    m_axisColor    = nullptr;
    ColorButton*    m_gridColor    = nullptr;
    ColorButton*    m_labelColor   = nullptr;*/

    // Farb-Tab
    /*ColorButton* m_background   = nullptr;
    ColorButton* m_point        = nullptr;
    ColorButton* m_pointFill    = nullptr;
    ColorButton* m_line         = nullptr;
    ColorButton* m_circle       = nullptr;
    ColorButton* m_selected     = nullptr;
    ColorButton* m_highlighted  = nullptr;
    ColorButton* m_construction = nullptr;
    ColorButton* m_watermark    = nullptr;*/
};