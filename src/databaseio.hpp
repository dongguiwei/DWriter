/* Copyright (c) 2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DATABASEIO_HPP
#define DATABASEIO_HPP

#include <QObject>
#include <bb/data/SqlConnection>

/*
 * @brief Declaration of our application's class (as opposed to the BB Cascades
 *  application class that contains our application).
 */

class DatabaseIo: public QObject
{
    // Classes that inherit from QObject must have the Q_OBJECT macro so
    // the meta-object compiler (MOC) can add supporting code to the application.
    Q_OBJECT

public:
    DatabaseIo();
    ~DatabaseIo();

    // "Q_INVOKABLE" allows these functions to be called from main.qml
    bool createDatabase();
    void dropTable();
    void createTable();
    void queryTable();
    void createRecord(const QString &firstName, const QString &lastName);
    void addRecord(const QString &firstName, const QString &lastName);
    void createTableAsync(); // This is an example of how you make asynchronous calls to the database.

    int getCount();
    QString getEvent(int eventId);

private slots:
    // This is the callback used for executing asynchronous queries.
    void onLoadAsyncResultData(const bb::data::DataAccessReply &reply);

private:
    // Helper method to show a alert dialog
    void alert(const QString &message);

    // The connection to the SQL database
    bb::data::SqlConnection* m_sqlConnection;
};

#endif
