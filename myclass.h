#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include<QString>
class MyClass : public QObject
{
    Q_OBJECT
    //暴露给QML 属性
    Q_PROPERTY(QString mystring READ getString WRITE setString NOTIFY mystringChanged)
private:
    QString * m_pstring;
public:
    explicit MyClass(QObject *parent = 0);
     Q_INVOKABLE QString getString(); //Q_INVOCABLE暴露给 QML
    Q_INVOKABLE void setString(QString string);
    ~MyClass();

signals:
   void  mystringChanged(QString value,QString newstr); //信号
public slots:

};

#endif // MYCLASS_H
