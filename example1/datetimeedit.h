#ifndef DATETIMEEDIT_H
#define DATETIMEEDIT_H

#include <QObject>
#include <QDateTimeEdit>
#include <QKeyEvent>

class DateTimeEdit: public QDateTimeEdit
{
    Q_OBJECT
public:
    DateTimeEdit(QWidget *parent);
    void keyPressEvent(QKeyEvent *event);
signals:
    void EnterPressed();
};

#endif // DATETIMEEDIT_H
