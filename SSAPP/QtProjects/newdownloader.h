#ifndef NEWDOWNLOADER_H
#define NEWDOWNLOADER_H
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QDebug>
class NewDownloader:public QObject
{
public:
    NewDownloader();
    QString url;

    QFile *myfile;

    //Downloading
    QNetworkAccessManager * networkManager;//
    QNetworkReply * downloadReply;//
    QNetworkReply * queryReply;//
    class StelProgressController * downloadProgressBar;//
    class StelProgressController * queryProgressBar;//

    QNetworkAccessManager* networkAccessManager;

    void startdl(QString urlString,QString filepath);
public slots:
//    void downloadComplete(QNetworkReply *reply);

    void doProcessReadyRead();
    void doProcessFinished();
signals:
    void downloadfin();
//    void doProcessDownloadProgress(qint64,qint64);
//    void doProcessError(QNetworkReply::NetworkError code);
};

#endif // NEWDOWNLOADER_H
