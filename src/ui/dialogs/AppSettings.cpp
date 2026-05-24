#include "AppSettings.h"

AppSettings::AppSettings()
    : m_settings("Stoicheia", "Stoicheia")
{}

AppSettings& AppSettings::instance() {
    static AppSettings s;
    return s;
}

void AppSettings::addRecentFile(const QString &fileName) {
    general.recentFiles.files.removeAll(fileName);
    general.recentFiles.files.prepend(fileName);
    while (general.recentFiles.files.size() > general.recentFiles.maxCount) {
        general.recentFiles.files.removeLast();
    }
    save();
}

void AppSettings::load() {
    m_settings.beginGroup("General");
    m_settings.beginGroup("recentFiles");
    general.recentFiles.files = m_settings.value("files", QStringList()).toStringList();
    general.recentFiles.maxCount = m_settings.value("maxCount",  5).toInt();
    m_settings.endGroup();
    general.language = m_settings.value("language", general.language).toString();
    m_settings.endGroup();

    m_settings.beginGroup("Grid");
    grid.visible     = m_settings.value("visible",     grid.visible).toBool();
    grid.spacing     = m_settings.value("spacing",     grid.spacing).toDouble();
    grid.snapEnabled = m_settings.value("snapEnabled", grid.snapEnabled).toBool();
    grid.axisColor   = m_settings.value("axisColor",   grid.axisColor).value<QColor>();
    grid.gridColor   = m_settings.value("gridColor",   grid.gridColor).value<QColor>();
    grid.labelColor  = m_settings.value("labelColor",  grid.labelColor).value<QColor>();
    m_settings.endGroup();

    m_settings.beginGroup("Colors");
    colors.background   = m_settings.value("background",   colors.background).value<QColor>();
    colors.point        = m_settings.value("point",        colors.point).value<QColor>();
    colors.pointFill    = m_settings.value("pointFill",    colors.pointFill).value<QColor>();
    colors.line         = m_settings.value("line",         colors.line).value<QColor>();
    colors.circle       = m_settings.value("circle",       colors.circle).value<QColor>();
    colors.selected     = m_settings.value("selected",     colors.selected).value<QColor>();
    colors.highlighted  = m_settings.value("highlighted",  colors.highlighted).value<QColor>();
    colors.construction = m_settings.value("construction", colors.construction).value<QColor>();
    colors.watermark    = m_settings.value("watermark",    colors.watermark).value<QColor>();
    m_settings.endGroup();
}

void AppSettings::save() {
    m_settings.beginGroup("General");
    m_settings.beginGroup("recentFiles");
    m_settings.setValue("files", general.recentFiles.files);
    m_settings.setValue("maxCount",  general.recentFiles.maxCount);
    m_settings.endGroup();
    m_settings.setValue("language",   general.language);
    m_settings.endGroup();

    m_settings.beginGroup("Grid");
    m_settings.setValue("visible",     grid.visible);
    m_settings.setValue("spacing",     grid.spacing);
    m_settings.setValue("snapEnabled", grid.snapEnabled);
    m_settings.setValue("axisColor",   grid.axisColor);
    m_settings.setValue("gridColor",   grid.gridColor);
    m_settings.setValue("labelColor",  grid.labelColor);
    m_settings.endGroup();

    m_settings.beginGroup("Colors");
    m_settings.setValue("background",   colors.background);
    m_settings.setValue("point",        colors.point);
    m_settings.setValue("pointFill",    colors.pointFill);
    m_settings.setValue("line",         colors.line);
    m_settings.setValue("circle",       colors.circle);
    m_settings.setValue("selected",     colors.selected);
    m_settings.setValue("highlighted",  colors.highlighted);
    m_settings.setValue("construction", colors.construction);
    m_settings.setValue("watermark",    colors.watermark);
    m_settings.endGroup();
}

void AppSettings::resetToDefaults() {
    general = General{};
    grid    = GridSettings{};
    colors  = ColorScheme{};
}