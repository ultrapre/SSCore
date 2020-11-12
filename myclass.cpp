#include "myclass.h"

MyClass::MyClass(QObject *parent) : QObject(parent)
{
    this->m_pstring=new QString("moren");
}

MyClass::~MyClass()
{
    delete this->m_pstring;
}
QString MyClass::getString()
{
    setString("xx");
    return *(this->m_pstring);
}
void MyClass::setString(QString string){
    *(this->m_pstring)=string;
    emit mystringChanged(string,"pp");
}
