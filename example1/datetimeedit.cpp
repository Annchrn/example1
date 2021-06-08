#include "datetimeedit.h"
#include <QDebug>

DateTimeEdit::DateTimeEdit(QWidget* parent) : QDateTimeEdit(parent)
{
}

void DateTimeEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        emit EnterPressed();
        }
    QDateTimeEdit::keyPressEvent(event);
}
