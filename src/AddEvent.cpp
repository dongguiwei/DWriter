/*
 * AddEvent.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: daviddong
 */
#include <stdio.h>
#include <sys/slog.h>

#include "AddEvent.hpp"

AddEvent::AddEvent(QObject *parent, DatabaseIo *dataio)
	: QObject(parent)
	, m_currentTime(QDateTime::currentDateTime())
	, m_dataIo(dataio)
{

}

void AddEvent::setText(const QString &text)
{
    if (m_textEvent == text)
        return;

    m_textEvent = text;

    emit textChanged();
}

QString AddEvent::text() const
{
    return m_textEvent;
}

QString AddEvent::currentTime() const
{
    if (!m_currentTime.isValid())
        return tr("n/a");
    else
        return m_currentTime.toString();
}

void AddEvent::addEventDone()
{
	if(m_dataIo == NULL) {
		qDebug() << "Save Event data error, dataio is NULL";
		return;
	}

	m_dataIo->addRecord(m_currentTime.toString(),m_textEvent);
}
