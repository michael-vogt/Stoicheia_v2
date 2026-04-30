#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    window.resize(400,300);
    window.setWindowTitle("Stoicheia");
    window.show();
    return app.exec();
}
