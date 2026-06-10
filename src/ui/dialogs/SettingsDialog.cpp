#include "SettingsDialog.h"
#include "../../Constants.h"
#include "ui_settingsdialog.h"
#include "ColorButton.h"
#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <qdir.h>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>


using namespace Constants;

static auto buildLanguageMap(const QString& path) -> QHash<QString, QString> {
    QHash<QString, QString> map;

    QDir dir(path);
    if (!dir.exists()) {
        return map;
    }

    const QStringList files = dir.entryList({"app_*.qm"}, QDir::Files);

    for (const QString& file : files) {
        QString code = file;
        code.remove("app_");
        code.chop(3);

        QLocale locale(code);

        QString name = locale.nativeLanguageName();
        if (code == "en") {
            name = "English";
        }

        if (name.isEmpty()) {
            name = QLocale::languageToString(locale.language());
        }

        map.insert(code, name);
    }

    return map;
}

SettingsDialog::SettingsDialog(AppSettings& settings, QWidget* parent)
: QDialog(parent), ui(new Ui::SettingsDialog),
m_settings(settings),
m_snapshotGrid(settings.grid),
m_snapshotColors(settings.colors),
m_snapshotGeneral(settings.general)
{
    ui->setupUi(this);
    setWindowTitle(tr("Einstellungen"));
    setMinimumWidth(LayoutConstants::SETTINGSDIALOG_MINWIDTH);

    fillLanguages();

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Abbrechen"));
    ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Anwenden"));
    ui->buttonBox->button(QDialogButtonBox::Reset)->setText(tr("Standard"));

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() { apply(); accept(); });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);

    readFromSettings();
}

void SettingsDialog::reject() {
    m_settings.grid = m_snapshotGrid;
    m_settings.colors = m_snapshotColors;
    m_settings.general = m_snapshotGeneral;
    m_settings.save();
    emit settingsChanged();
    QDialog::reject();
}

void SettingsDialog::changeEvent(QEvent* event) {
    QDialog::changeEvent(event);
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        QTimer::singleShot(0, this, [this]() {
            ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
            ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Abbrechen"));
            ui->buttonBox->button(QDialogButtonBox::Apply)->setText(tr("Anwenden"));
            ui->buttonBox->button(QDialogButtonBox::Reset)->setText(tr("Standard"));
        });
    }
}

void SettingsDialog::apply() {
    writeToSettings();
    m_settings.save();
    emit settingsChanged();
}

void SettingsDialog::resetToDefaults() const {
    m_settings.resetToDefaults();
    readFromSettings();
}

void SettingsDialog::fillLanguages() const {
    auto languageMap = buildLanguageMap(":/i18n/");

    QVector<QPair<QString, QString>> list;
    for (auto it = languageMap.begin(); it != languageMap.end(); ++it) {
        list.append({it.key(), it.value()});
    }

    std::ranges::sort(list, [](const auto& entry_a, const auto& entry_b) -> auto {return entry_a.second < entry_b.second;});

    for (const auto& item : list) {
        ui->m_uiLanguage->addItem(item.second, item.first);
    }
}

void SettingsDialog::readFromSettings() const {
    ui->m_uiRecentMaxCount->setValue(m_settings.general.recentFiles.maxCount);
    int index = ui->m_uiLanguage->findData(m_settings.general.language);
    if (index != -1) {
        ui->m_uiLanguage->setCurrentIndex(index);
    }

    ui->m_gridVisible->setChecked(m_settings.grid.visible);
    ui->m_gridSpacing->setValue(m_settings.grid.spacing);
    ui->m_snapEnabled->setChecked(m_settings.grid.snapEnabled);
    ui->m_axisColor->setColor(m_settings.grid.axisColor);
    ui->m_gridColor->setColor(m_settings.grid.gridColor);
    ui->m_labelColor->setColor(m_settings.grid.labelColor);

    ui->m_background->setColor(m_settings.colors.background);
    ui->m_point->setColor(m_settings.colors.point);
    ui->m_pointFill->setColor(m_settings.colors.pointFill);
    ui->m_line->setColor(m_settings.colors.line);
    ui->m_circle->setColor(m_settings.colors.circle);
    ui->m_selected->setColor(m_settings.colors.selected);
    ui->m_highlighted->setColor(m_settings.colors.highlighted);
    ui->m_construction->setColor(m_settings.colors.construction);
    ui->m_watermark->setColor(m_settings.colors.watermark);
}

void SettingsDialog::writeToSettings() const {
    m_settings.general.recentFiles.maxCount = ui->m_uiRecentMaxCount->value();
    m_settings.general.language = ui->m_uiLanguage->currentData().toString();

    m_settings.grid.visible     = ui->m_gridVisible->isChecked();
    m_settings.grid.spacing     = ui->m_gridSpacing->value();
    m_settings.grid.snapEnabled = ui->m_snapEnabled->isChecked();
    m_settings.grid.axisColor   = ui->m_axisColor->color();
    m_settings.grid.gridColor   = ui->m_gridColor->color();
    m_settings.grid.labelColor  = ui->m_labelColor->color();

    m_settings.colors.background   = ui->m_background->color();
    m_settings.colors.point        = ui->m_point->color();
    m_settings.colors.pointFill    = ui->m_pointFill->color();
    m_settings.colors.line         = ui->m_line->color();
    m_settings.colors.circle       = ui->m_circle->color();
    m_settings.colors.selected     = ui->m_selected->color();
    m_settings.colors.highlighted  = ui->m_highlighted->color();
    m_settings.colors.construction = ui->m_construction->color();
    m_settings.colors.watermark    = ui->m_watermark->color();
}