#include "SettingsDialog.h"
#include "ColorButton.h"
#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>

SettingsDialog::SettingsDialog(AppSettings& settings, QWidget* parent)
    : QDialog(parent), m_settings(settings)
{
    setWindowTitle(tr("Einstellungen"));
    setMinimumWidth(400);

    auto* tabs = new QTabWidget(this);
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
    layout->addWidget(buttons);

    readFromSettings();
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
}

void SettingsDialog::readFromSettings() {
    m_gridVisible->setChecked(m_settings.grid.visible);
    m_gridSpacing->setValue(m_settings.grid.spacing);
    m_snapEnabled->setChecked(m_settings.grid.snapEnabled);
    m_axisColor->setColor(m_settings.grid.axisColor);
    m_gridColor->setColor(m_settings.grid.gridColor);
    m_labelColor->setColor(m_settings.grid.labelColor);

    m_background->setColor(m_settings.colors.background);
    m_point->setColor(m_settings.colors.point);
    m_pointFill->setColor(m_settings.colors.pointFill);
    m_line->setColor(m_settings.colors.line);
    m_circle->setColor(m_settings.colors.circle);
    m_selected->setColor(m_settings.colors.selected);
    m_highlighted->setColor(m_settings.colors.highlighted);
    m_construction->setColor(m_settings.colors.construction);
    m_watermark->setColor(m_settings.colors.watermark);
}

void SettingsDialog::writeToSettings() {
    m_settings.grid.visible     = m_gridVisible->isChecked();
    m_settings.grid.spacing     = m_gridSpacing->value();
    m_settings.grid.snapEnabled = m_snapEnabled->isChecked();
    m_settings.grid.axisColor   = m_axisColor->color();
    m_settings.grid.gridColor   = m_gridColor->color();
    m_settings.grid.labelColor  = m_labelColor->color();

    m_settings.colors.background   = m_background->color();
    m_settings.colors.point        = m_point->color();
    m_settings.colors.pointFill    = m_pointFill->color();
    m_settings.colors.line         = m_line->color();
    m_settings.colors.circle       = m_circle->color();
    m_settings.colors.selected     = m_selected->color();
    m_settings.colors.highlighted  = m_highlighted->color();
    m_settings.colors.construction = m_construction->color();
    m_settings.colors.watermark    = m_watermark->color();
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