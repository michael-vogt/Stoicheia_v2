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

static auto buildLanguageMap(const QString& path) -> QHash<QString, QString>;

constexpr int DEFAULT_SETTINGSDIALOG_MINWIDTH = 400;

class SettingsDialog : public QDialog {
    Q_OBJECT

    Ui::SettingsDialog* ui;

public:
    explicit SettingsDialog(AppSettings& settings, QWidget* parent = nullptr);

public slots: // NOLINT
    void reject() override;

signals:
    void settingsChanged();

protected:
    void changeEvent(QEvent* event) override;

private slots:
    void apply();
    void resetToDefaults() const;

private: // NOLINT
    void fillLanguages() const;

    void readFromSettings() const;
    void writeToSettings() const;

    AppSettings& m_settings;
    GridSettings m_snapshotGrid;
    ColorScheme m_snapshotColors;
    General m_snapshotGeneral;
};