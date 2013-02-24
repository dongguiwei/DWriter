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
#include "databaseio.hpp"


#include <bb/cascades/QmlDocument>
#include <bb/data/SqlDataAccess>
#include <bb/system/SystemDialog>

#include <QtSql/QtSql>

using namespace bb::cascades;
using namespace bb::system;
using namespace bb::data;

const QString DATABASENAME = "./data/DWriteData.db";

#define INITIAL_LOAD_ID 10
#define ASYNCH_LOAD_ID 20

#define ASYNCH_BATCH_SIZE 10

//! [0]
DatabaseIo::DatabaseIo()
    : m_sqlConnection(0)
{
    // Since we need read and write access to the database, it has
    // to be moved to a folder where we have access to it. First,
    // we check if the file already exists (previously copied).
    QFile newFile(DATABASENAME);

    if (!newFile.exists()) {
    	createDatabase();
    }

    // 1. Create a new SqlConnection object and point it at the database
    //    It will automatically open a connection to the database using the QSqlDatabase object
    //    if necessary. Note that a reference to the SqlConnection object is tracked so that
    //    it can be freed later. Alternatively, you could assign the SqlConnection
    m_sqlConnection = new SqlConnection(DATABASENAME);

    // 2. Connect a slot to the SqlConnection objects 'reply' signal which is emitted when the
    //    executed query is complete.
    //    Note: types are fully qualified with namespaces (e,g, bb::data::) when used with all
    //    MOC macros (in this case, SIGNAL and SLOT)
    connect(m_sqlConnection, SIGNAL(reply(const bb::data::DataAccessReply&)),
            this, SLOT(onLoadAsyncResultData(const bb::data::DataAccessReply&)));
}

DatabaseIo::~DatabaseIo()
{
    if (m_sqlConnection->isRunning()) {
    	m_sqlConnection->stop();
    }

    // Free the SqlConnection object as it will not be used any more.
    m_sqlConnection->deleteLater();
    m_sqlConnection = 0;
}
//! [0]

//! [1]
bool DatabaseIo::createDatabase()
{
    // 1. Create an instance of QSqlDatabase with the driver for SQLite databases.
    //    Note: QSqlDatabase::addDatabase(QString type, QString connectionName = default)
    //    The above prototype shows that QSqlDatabase can take an optional second parameter
    //    referred to as the 'connectionName'. The 'connectionName' should be a unique string
    //    the represents your database connection. It allows you to get access to your database
    //    connection in the future via:
    //    QSqlDatabase db = QSqlDatabase::database(QString connectionName);
    //    If the connectionName is not specified, then it is assumed that this connection will
    //    become the default connection which can be referred to without using the connectionName
    //    parameter. (See step 1 of 'DatabaseIo::createTable') for more information.
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    bool success = false;

    // 2. Set the path of where the database will be located.
    //    Note: The db extension is not required
    database.setDatabaseName(DATABASENAME);

    // 3. Open a connection to the database, if the database does not exist
    //    one will be created if permitted.
    if (database.open()) {
        alert(tr("Database created/registered."));
        success = true;
    } else {
        // If the database fails to open, error information can be accessed via
        // the lastError function.
        const QSqlError error = database.lastError();
        alert(tr("Error opening connection to the database: %1").arg(error.text()));
    }

    // 2. Create a query to execute.
    //    The below query creates a table with customerID, firstName, and lastName columns
    //    if it does not already exist.
    const QString createSQL = "CREATE TABLE IF NOT EXISTS events ( "
                              "                eventID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "                timeStamp VARCHAR, "
                              "                textEvent VARCHAR"
                              ");";

    // 3. Create an QSqlQuery object with which you can execute queries
    //    NOTE: The QSqlDatabase function has an 'exec' function, however this
    //    is deprecated and should be avoided.
    QSqlQuery query(database);
    if (query.exec(createSQL)) {
        alert(tr("Table creation query execute successfully"));
    } else {
        // If 'exec' fails, error information can be accessed via the lastError function
        // the last error is reset every time exec is called.
        const QSqlError error = query.lastError();
        alert(tr("Create table error: %1").arg(error.text()));
    }

    // 4. Close the connection to the database.
    //    Be warned, closing the database will invalidate any SqlQuery objects (see below)
    database.close();

    return success;
}
//! [1]

//! [2]
// -----------------------------------------------------------------------------------------------
// Synchronous Database Functionality with QSqlDatabase and QSqlQuery
void DatabaseIo::createTable()
{
    // 1. Get a reference to the database, which will automatically open the connection
    //    if it is not already open.
    //    NOTE: The below code assumes that the database being accessed is the 'default
    //    connection' database. If a database connection was created with a registered
    //    connection name then you would access the database via:
    //    QSqlDatabase db = QSqlDatabase::database(<connectionName>);
    QSqlDatabase database = QSqlDatabase::database();

    // 2. Create a query to execute.
    //    The below query creates a table with customerID, firstName, and lastName columns
    //    if it does not already exist.
    const QString createSQL = "CREATE TABLE IF NOT EXISTS events ( "
                              "                eventID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "                timeStamp VARCHAR, "
                              "                textEvent VARCHAR"
                              ");";

    // 3. Create an QSqlQuery object with which you can execute queries
    //    NOTE: The QSqlDatabase function has an 'exec' function, however this
    //    is deprecated and should be avoided.
    QSqlQuery query(database);
    if (query.exec(createSQL)) {
        alert(tr("Table creation query execute successfully"));
    } else {
        // If 'exec' fails, error information can be accessed via the lastError function
        // the last error is reset every time exec is called.
        const QSqlError error = query.lastError();
        alert(tr("Create table error: %1").arg(error.text()));
    }

    // 4. Optionally close the database connection if we no longer plan to use it
    //    Note that calling QSqlDatabase::database() will automatically re-open
    //    the connection for us.
    //    NOTE: Closing the database invalidates any QSqlQuery objects you have created
    //    with this database connection.
    database.close();
}

// -----------------------------------------------------------------------------------------------
// Synchronous Database Functionality with QSqlDatabase and QSqlQuery
void DatabaseIo::queryTable()
{
    // 1. Get a reference to the database, which will automatically open the connection
    //    if it is not already open.
    //    NOTE: The below code assumes that the database being accessed is the 'default
    //    connection' database. If a database connection was created with a registered
    //    connection name then you would access the database via:
    //    QSqlDatabase db = QSqlDatabase::database(<connectionName>);
    QSqlDatabase database = QSqlDatabase::database();

    // 2. Create a query to execute.
    //    The below query creates a table with customerID, firstName, and lastName columns
    //    if it does not already exist.
    const QString querySQL = "select * from events";

    // 3. Create an QSqlQuery object with which you can execute queries
    //    NOTE: The QSqlDatabase function has an 'exec' function, however this
    //    is deprecated and should be avoided.
    QSqlQuery query(database);
    if (query.exec(querySQL)) {
        alert(tr("Table creation query execute successfully"));
    } else {
        // If 'exec' fails, error information can be accessed via the lastError function
        // the last error is reset every time exec is called.
        const QSqlError error = query.lastError();
        alert(tr("Create table error: %1").arg(error.text()));
    }

    // 4. Optionally close the database connection if we no longer plan to use it
    //    Note that calling QSqlDatabase::database() will automatically re-open
    //    the connection for us.
    //    NOTE: Closing the database invalidates any QSqlQuery objects you have created
    //    with this database connection.
    database.close();
}

void DatabaseIo::dropTable()
{
    // 1. Get a reference to the database, which will automatically open the connection
    //    if it is not already open.
    //    NOTE: The below code assumes that the database being accessed is the 'default
    //    connection' database. If a database connection was created with a registered
    //    connection name then you would access the database via:
    //    QSqlDatabase db = QSqlDatabase::database(<connectionName>);
    QSqlDatabase database = QSqlDatabase::database();

    // 2. When dropping a table, you should first verify if it exists or not.
    //    Alternatively, you can embed into your SQL statement a check to see
    //    if the table exists.
    //    The below example embeds the check into the SQL statement.

    // NOTE: If you wish to check if the table exists in code, you can use the
    //    below example:
    //        if(database.tables().contains("customers")) {
    //            alert(tr("The 'customers' table exists"));
    //      }

    // 3. Create an QSqlQuery object with which you can execute queries
    //    NOTE: The QSqlDatabase function has an 'exec' function, however this
    //    is deprecated and should be avoided.
    QSqlQuery query(database);

    const QString dropSQL = "DROP TABLE IF EXISTS events";
    if (query.exec(dropSQL)) {
        alert(tr("Table drop query executed successfully."));
    } else {
        // If 'exec' fails, error information can be accessed via the lastError function
        // the last error is reset every time exec is called.
        const QSqlError error = query.lastError();
        alert(tr("Drop table error: %1").arg(error.text()));
    }

    // 4. Optionally close the database connection if we no longer plan to use it
    //    Note that calling QSqlDatabase::database() will automatically re-open
    //    the connection for us.
    //    NOTE: Closing the database invalidates any QSqlQuery objects you have created
    //    with this database connection.
    database.close();
}

void DatabaseIo::addRecord(const QString &timeStamp, const QString &textEvent)
{
   // Execute query with named binding using named placeholders
    QVariantMap eventsValues;
    eventsValues["timeStamp"] = timeStamp;
    eventsValues["textEvent"] = textEvent;
    m_sqlConnection->execute(
        "INSERT INTO events (timeStamp, textEvent) VALUES(:timeStamp, :textEvent)",
        eventsValues);
}
void DatabaseIo::createRecord(const QString &timeStamp, const QString &textEvent)
{
    // 1. Get a reference to the database, which will automatically open the connection
    //    if it is not already open.
    //    NOTE: The below code assumes that the database being accessed is the 'default
    //    connection' database. If a database connection was created with a registered
    //    connection name then you would access the database via:
    //    QSqlDatabase db = QSqlDatabase::database(<connectionName>);
    QSqlDatabase database = QSqlDatabase::database();

    // 2. Verify the table exists first, always a good safety pre-caution.
    //    For performance, on application startup, you would verify that all tables exist
    //    and can create or update any that do not. This would allow you to
    //    skip this check to see if a table exists or not in the database.
    if (!database.tables().contains("events")) {
        alert(tr("Create record error: customers table does not exist."));
    } else {
        // 3. Create an QSqlQuery object with which you can execute queries
        //    In this example we bind parameters in the query. A large advantage to using
        //    bindings (aside from performance enhancements) is that input is automatically
        //    escaped avoiding potential issues with odd characters (quotes) and prevents
        //    SQL Injection attacks.
        //    Note that for databases that do not support bindings, Qt simulates the binding
        //    effects.
        //    IMPORTANT NOTE: If ever accepting user information without using bindings,
        //    be sure to 'escape' your queries.
        QSqlQuery query(database);
        query.prepare("INSERT INTO events (timeStamp, textEvent) VALUES(:timeStamp, :textEvent)");
        query.bindValue(":timeStamp", timeStamp);
        query.bindValue(":textEvent", textEvent);
        query.exec();

        // Note that no SQL Statement is passed to 'exec' as it is a prepared statement.
        if (query.exec()) {
            alert(tr("Record created"));
        } else {
            // If 'exec' fails, error information can be accessed via the lastError function
            // the last error is reset every time exec is called.
            const QSqlError error = query.lastError();
            alert(tr("Create record error: %1").arg(error.text()));
        }
    }

    // 4. Optionally close the database connection if we no longer plan to use it
    //    Note that calling QSqlDatabase::database() will automatically re-open
    //    the connection for us.
    //    NOTE: Closing the database invalidates any QSqlQuery objects you have created
    //    with this database connection.
    database.close();
}
//! [2]

//! [3]
// -----------------------------------------------------------------------------------------------
// Asynchronous Database Functionality with SqlConnection
// The above examples all used QSqlQuery to interact with the database synchronous.
// This below example demonstrates how interactions can be done asynchronously
// by creating a database table.
void DatabaseIo::createTableAsync()
{

    // Sanity check to ensure that we don't execute the m_sqlConnection twice using the same object
    if (m_sqlConnection) {
        alert(tr("Error: previous async SqlConnection still executing"));
        return;
    }

    // 1. Create a new SqlConnection object and point it at the database
    //    It will automatically open a connection to the database using the QSqlDatabase object
    //    if necessary. Note that a reference to the SqlConnection object is tracked so that
    //    it can be freed later. Alternatively, you could assign the SqlConnection
    m_sqlConnection = new SqlConnection(DATABASENAME);

    // 2. Connect a slot to the SqlConnection objects 'reply' signal which is emitted when the
    //    executed query is complete.
    //    Note: types are fully qualified with namespaces (e,g, bb::data::) when used with all
    //    MOC macros (in this case, SIGNAL and SLOT)
    connect(m_sqlConnection, SIGNAL(reply(const bb::data::DataAccessReply&)),
            this, SLOT(onLoadAsyncResultData(const bb::data::DataAccessReply&)));

    // 3. Create the SQL Query that you wish to execute asynchronously.
    //    NOTE: The below query does not use the SQL statement 'IF NOT EXISTS'
    //    Therefore, it will report an error if the table already exists and you
    //    try to create it.
    const QString createSQL = "CREATE TABLE IF NOT EXISTS events ( "
                              "                eventID INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "                timeStamp VARCHAR, "
                              "                textEvent VARCHAR "
                              ");";

    // 4. Execute the query. Note, that upon function return, the query may not have run yet
    //    You need to wait for the 'reply' signal
    m_sqlConnection->execute(createSQL);

    qDebug() << "Create table started...";
}
//! [3]
// Callback for result of createTableAsync();
void DatabaseIo::onLoadAsyncResultData(const bb::data::DataAccessReply &reply)
{
    qDebug() << "Create table finished.";

    // Check if an error has occurred
    if (reply.hasError()) {
        alert(tr("Error creating table: %1").arg(reply.errorMessage()));
    } else {
        alert(tr("Asynchronous createTable query Succeeded!"));
        /*
        if (reply.id() >= INITIAL_LOAD_ID) {
            QVariantList resultList = reply.result().value<QVariantList>();

            if (resultList.size() > 0) {
                // Insert the data into the model.
                this->insertList(resultList);

                if (reply.id() >= ASYNCH_LOAD_ID) {
                    // If the ID belongs to an asynchronous request, we keep querying until there are no more results.
                    QString query;

                    // Increase the offset by using the reply IDs.
                    QTextStream(&query) << "select * from cities limit " << ASYNCH_BATCH_SIZE
                            << " offset "
                            << +(ASYNCH_BATCH_SIZE * (reply.id() - ASYNCH_LOAD_ID + 2));
                    mSqlConnector->execute(query, (reply.id() + 1));
                }
            }
        }
        */
    }

}

// -----------------------------------------------------------------------------------------------
// Alert Dialog Box Functions
void DatabaseIo::alert(const QString &message)
{
    SystemDialog *dialog; // SystemDialog uses the BB10 native dialog.
    dialog = new SystemDialog(tr("OK"), 0); // New dialog with on 'Ok' button, no 'Cancel' button
    dialog->setTitle(tr("Alert")); // set a title for the message
    dialog->setBody(message); // set the message itself
    dialog->setDismissAutomatically(true); // Hides the dialog when a button is pressed.

    // Setup slot to mark the dialog for deletion in the next event loop after the dialog has been accepted.
    connect(dialog, SIGNAL(accepted()), dialog, SLOT(deleteLater()));
    dialog->show();
}

int DatabaseIo::getCount()
{
	int ret = 0;
    QString query = "select COUNT(*) from events";
    DataAccessReply reply = m_sqlConnection->executeAndWait(query);

    if (reply.hasError()) {
        qWarning() << "onLoadAsyncResultData: " << reply.id() << ", SQL error: " << reply;
    } else {
        QVariantList data = reply.result().value<QVariantList>();

        // Check if the city is already in the list, if its not we add it to the model.
        // The first() function used since in our city data there is only one instance of each city.
        QVariantMap dataItem = data.first().toMap();
        ret = dataItem.value("COUNT(*)").toInt();
    }
    //alert(tr("there are %1 records in database").arg(ret));
    return ret;
}

QString DatabaseIo::getEvent(int eventId)
{
	QString ret = "Error: no item found";
	QString eid = QString::number(eventId+1);
    QString query = "select * from events WHERE eventID='" + eid + "'";
    DataAccessReply reply = m_sqlConnection->executeAndWait(query);

    if (reply.hasError()) {
        qWarning() << "getEvent: " << reply.id() << ", SQL error: " << reply;
    } else {
        QVariantList data = reply.result().value<QVariantList>();

        // Check if the city is already in the list, if its not we add it to the model.
        // The first() function used since in our city data there is only one instance of each city.
        QVariantMap dataItem = data.first().toMap();

        ret = dataItem.value("timeStamp").toString() + ", " + dataItem.value("textEvent").toString();
    }
    return ret;
}
