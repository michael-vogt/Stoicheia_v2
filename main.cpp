#include <iostream>
#include <QApplication>
#include <qevent.h>
#include <QTranslator>

#include "geometry/geometry.h"
#include "constructions/constructions.h"
#include "ui/MainWindow.h"
#include "ui/dialogs/AppSettings.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QTranslator translator;
    if (translator.load(":/i18n/app_en.qm")) {
        app.installTranslator(&translator);
    } else {
        std::cerr << "Error loading translator" << std::endl;
    }

    AppSettings::instance().load();
    MainWindow window("Στοιχεῖα");
    window.drawingBoard()->applySettings();

    if (argc > 1) {
        const QString filename(argv[1]);
        window.fileManager()->openFile(filename);
    }

    window.showMaximized();

    return QApplication::exec();
}
