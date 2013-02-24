/*
 * AddEvent.hpp
 *
 *  Created on: Feb 14, 2013
 *      Author: daviddong
 */

#ifndef ADDEVENT_HPP_
#define ADDEVENT_HPP_

#include <QtCore/QObject>
#include <QDateTime>
#include "databaseio.hpp"


class AddEvent : public QObject
{
    Q_OBJECT
    // The data properties of the message that is displayed
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString currentTime READ currentTime NOTIFY currentTimeChanged)


public:
	AddEvent(QObject *parent = 0, DatabaseIo *dataio = 0);

Q_SIGNALS:
    // The change notification signals of the properties
    void textChanged();
    void currentTimeChanged();

public Q_SLOTS:
    void addEventDone();

private:
    // The change notification signals of the properties
    QString text() const;
    QString currentTime() const;
    void setText(const QString &body);

    // The property values
    QString m_textEvent;
    QDateTime m_currentTime;
    DatabaseIo *m_dataIo;
};

#endif /* ADDEVENT_HPP_ */

