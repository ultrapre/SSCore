#include "newdownloader.h"

NewDownloader::NewDownloader()
{
    networkManager = new QNetworkAccessManager(this);
}

void NewDownloader::startdl(QString urlString,QString filepath)
{
    QUrl url(urlString);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "Unknown operating system");
    #if QT_VERSION >= 0x050600
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    #endif
    downloadReply = networkManager->get(request);
    connect(downloadReply,&QNetworkReply::readyRead,this,&NewDownloader::doProcessReadyRead);                //可读
    connect(downloadReply,&QNetworkReply::finished,this,&NewDownloader::doProcessFinished);                  //结束


    myfile = new QFile(filepath);
    bool ret = myfile->open(QIODevice::WriteOnly|QIODevice::Truncate);    //创建文件
    if(!ret)
    {
        qDebug()<<"download open fail";
//        QMessageBox::warning(this,"warning","open fail");
        return;
    }
}


//void NewDownloader::downloadComplete(QNetworkReply *reply)
//{
////    disconnect(downloadReply, &QNetworkReply::finished, this, &NewDownloader::downloadComplete);
////    disconnect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(&downloadComplete(QNetworkReply*)));
//    if(reply->error() || reply->bytesAvailable()==0)
//    {
//        qWarning() << "Download error: While downloading"
//                   << reply->url().toString()
//                   << "the following error occured:"
//                   << reply->errorString();
//        reply->deleteLater();
//        downloadReply = Q_NULLPTR;
//        return;
//    }
//    QFile file(filename);
//    if (file.open(QIODevice::WriteOnly))
//    {
//        file.write(reply->readAll());
//        file.close();
//        qDebug()<<"download finish";
//    }
//}



void NewDownloader::doProcessReadyRead()             //读取并写入
{
    while(!downloadReply->atEnd())
    {
        QByteArray ba = downloadReply->readAll();
//        qDebug()<<ba;
        myfile->write(ba);
    }
}

void NewDownloader::doProcessFinished()
{

    myfile->close();
    qDebug()<<"do finish";
}
