#include "SettingsDialog.h"
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

static QHash<QString, QString> buildLanguageMap(const QString& path) {
    QHash<QString, QString> map;

    QDir dir(path);
    const QStringList files = dir.entryList({"app_*.qm"}, QDir::Files);

    for (const QString& file : files) {
        QString code = file;
        code.remove("app_");
        code.chop(3);

        QLocale locale;

        QString name = locale.nativeLanguageName();
        if (code == "en")
            name = "English";

        if (name.isEmpty())
            name = QLocale::languageToString(locale.language());

        map.insert(code, name);
    }

    return map;
}

SettingsDialog::SettingsDialog(AppSettings& settings, QWidget* parent)
    : QDialog(parent), ui(new Ui::SettingsDialog), m_settings(settings)
{
    ui->setupUi(this);
    setWindowTitle(tr("Einstellungen"));
    setMinimumWidth(400);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() { apply(); accept(); });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::apply);
    connect(ui->buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);
    //connect(ui->buttonBox, &QPushButton::clicked, this, [this]() { apply(); accept(); });
    //connect(ui->buttonBox->Cancel, &QPushButton::clicked, this, &QDialog::reject);
    //connect(ui->buttonBox->Apply, &QPushButton::clicked, &SettingsDialog::apply);
    //connect(ui->buttonBox->Reset, &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);

    /*auto* tabs = new QTabWidget(this);
    tabs->addTab(buildUITab(),     tr("Appearance"));
    tabs->addTab(buildGridTab(),   tr("Raster"));
    tabs->addTab(buildColorsTab(), tr("Farben"));

    auto* buttons = new QDialogButtonBox(this);
    auto* okBtn     = buttons->addButton(QDialogButtonBox::Ok);
    auto* cancelBtn = buttons->addButton(QDialogButtonBox::Cancel);
    auto* applyBtn  = buttons->addButton(QDialogButtonBox::Apply);
    auto* resetBtn  = buttons->addButton(tr("Standard"), QDialogButtonBox::ResetRole);

    connect(okBtn,     &QPushButton::clicked, this, [this]() { apply(); accept(); });
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(applyBtn,  &QPushButton::clicked, this, &SettingsDialog::apply);
    connect(resetBtn,  &QPushButton::clicked, this, &SettingsDialog::resetToDefaults);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(tabs);
    layout->addWidget(buttons);*/

    readFromSettings();
}

/*QWidget *SettingsDialog::buildUITab() {
    auto* widget = new QWidget;
    auto* form = new QFormLayout(widget);
    form->setRowWrapPolicy(QFormLayout::WrapLongRows);

    m_uiRecentMaxCount = new QSpinBox(widget);
    m_uiRecentMaxCount->setMinimum(1);
    m_uiRecentMaxCount->setMaximum(10);
    m_uiRecentMaxCount->setValue(5);
    m_uiRecentMaxCount->setSuffix(tr(" (max)"));
    form->addRow(tr("Zuletzt verwendete Dateien:"), m_uiRecentMaxCount);

    m_uiLanguage = new QComboBox(widget);

    auto languageMap = buildLanguageMap(":/i18n/");
    QVector<QPair<QString, QString>> list;
    for (auto it = languageMap.begin(); it != languageMap.end(); ++it) {
        list.append({it.key(), it.value()});
    }

    std::ranges::sort(list, [](const auto& a, const auto& b) {return a.second < b.second;});

    for (const auto& item : list) {
        m_uiLanguage->addItem(item.second, item.first);
    }

    form->addRow(tr("Sprache:"), m_uiLanguage);

    return widget;
}

QWidget* SettingsDialog::buildGridTab() {
    auto* widget = new QWidget;
    auto* form   = new QFormLayout(widget);
    form->setRowWrapPolicy(QFormLayout::WrapLongRows);

    m_gridVisible = new QCheckBox(tr("Raster anzeigen"), widget);
    form->addRow(m_gridVisible);

    m_gridSpacing = new QDoubleSpinBox(widget);
    m_gridSpacing->setRange(5.0, 500.0);
    m_gridSpacing->setSingleStep(5.0);
    m_gridSpacing->setSuffix(" px");
    form->addRow(tr("Rasterabstand:"), m_gridSpacing);

    m_snapEnabled = new QCheckBox(tr("Immer einrasten"), widget);
    form->addRow(m_snapEnabled);

    form->addRow(new QLabel(tr("<b>Farben</b>")));

    m_axisColor  = new ColorButton(m_settings.grid.axisColor,  widget);
    m_gridColor  = new ColorButton(m_settings.grid.gridColor,  widget);
    m_labelColor = new ColorButton(m_settings.grid.labelColor, widget);

    form->addRow(tr("Achsenfarbe:"),     m_axisColor);
    form->addRow(tr("Rasterfarbe:"),     m_gridColor);
    form->addRow(tr("Beschriftungsfarbe:"), m_labelColor);

    return widget;
}

QWidget* SettingsDialog::buildColorsTab() {
    auto* widget = new QWidget;
    auto* form   = new QFormLayout(widget);

    m_background   = new ColorButton(m_settings.colors.background,   widget);
    m_point        = new ColorButton(m_settings.colors.point,        widget);
    m_pointFill    = new ColorButton(m_settings.colors.pointFill,    widget);
    m_line         = new ColorButton(m_settings.colors.line,         widget);
    m_circle       = new ColorButton(m_settings.colors.circle,       widget);
    m_selected     = new ColorButton(m_settings.colors.selected,     widget);
    m_highlighted  = new ColorButton(m_settings.colors.highlighted,  widget);
    m_construction = new ColorButton(m_settings.colors.construction, widget);
    m_watermark    = new ColorButton(m_settings.colors.watermark,    widget);

    form->addRow(tr("Hintergrund:"),      m_background);
    form->addRow(tr("Punkt:"),            m_point);
    form->addRow(tr("Punktfüllung:"),     m_pointFill);
    form->addRow(tr("Linie:"),            m_line);
    form->addRow(tr("Kreis:"),            m_circle);
    form->addRow(tr("Selektiert:"),       m_selected);
    form->addRow(tr("Hervorgehoben:"),    m_highlighted);
    form->addRow(tr("Konstruktion:"),     m_construction);
    form->addRow(tr("Wasserzeichen:"),    m_watermark);

    return widget;
}*/

void SettingsDialog::readFromSettings() {
    ui->m_uiRecentMaxCount->setValue(m_settings.ui.recentFiles.maxCount);
    int index = ui->m_uiLanguage->findData(m_settings.ui.language);
    if (index != -1)
        ui->m_uiLanguage->setCurrentIndex(index);

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

void SettingsDialog::writeToSettings() {
    m_settings.ui.recentFiles.maxCount = ui->m_uiRecentMaxCount->value();
    m_settings.ui.language = ui->m_uiLanguage->currentData().toString();

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

void SettingsDialog::apply() {
    writeToSettings();
    m_settings.save();
    emit settingsChanged();
}

void SettingsDialog::resetToDefaults() {
    m_settings.resetToDefaults();
    readFromSettings();
}