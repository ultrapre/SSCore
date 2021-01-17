#include "mainwindow.h"

#include <QApplication>

#ifdef Q_OS_ANDROID
#include <QtAndroid>


bool checkPermission() {
    QtAndroid::PermissionResult r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(r == QtAndroid::PermissionResult::Denied) {
        QtAndroid::requestPermissionsSync( QStringList() << "android.permission.WRITE_EXTERNAL_STORAGE" );
        r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
        if(r == QtAndroid::PermissionResult::Denied) {
             return false;
        }
   }
   return true;
}
#endif
//设置样式
static  void setStyle(const QString &styleName)
{
    QFile file(QString("%1").arg(styleName));
    file.open(QFile::ReadOnly);
    QString css = QLatin1String(file.readAll());
    qApp->setStyleSheet(css);
}
int main(int argc, char *argv[])
{
//    bool ok = copyDirectoryFiles("assets:/SSData",QDir::homePath()+"/SSData");
//    qDebug()<<"copy:"<<ok;
    QApplication a(argc, argv);
    setStyle(":/normalStyle.css");
    MainWindow w;
    w.show();
    return a.exec();
}
