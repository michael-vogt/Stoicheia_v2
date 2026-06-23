#include <iostream>
#include <QApplication>
#include <qevent.h>
#include <QTranslator>
#include <QLibraryInfo>

#include "ui/MainWindow.h"
#include "ui/dialogs/AppSettings.h"

auto main(int argc, char *argv[]) -> int { // NOLINT
    QApplication app(argc, argv);
    AppSettings::instance().load();
    QString language_code = AppSettings::instance().general.language;

    QTranslator qt_translator;
    if (qt_translator.load(QLocale(), "qtbase", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        QApplication::installTranslator(&qt_translator);
    }

    QTranslator translator;
    if (translator.load(QString(":/i18n/app_%1.qm").arg(language_code))) {
        QApplication::installTranslator(&translator);
    } else {
        std::cerr << "Error loading translator\n";
    }

    QApplication::setWindowIcon(QIcon(":/resources/icon.ico"));
    MainWindow window("Stoicheia (Στοιχεῖα)", &translator);
    window.drawingBoard()->applySettings();

    if (argc > 1) {
        const QString filename(argv[1]);
        window.fileManager()->openFile(filename);
    }

    window.showMaximized();

    return QApplication::exec();
}
