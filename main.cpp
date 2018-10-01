/*!
    @mainpage Программа "ГироКомпас"
    @author Александр Щербаков
    @date  13.09.18

*/
#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;

    QFile file(":style.qss");
    file.open(QFile::ReadOnly);
    QString strCSS = QLatin1String(file.readAll());
    qApp->setStyleSheet(strCSS);

    w.setWindowTitle(QObject::tr("ГироКомпас"));
    w.show();


    return a.exec();
}
