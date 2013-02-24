// Tabbed pane project template
#include "DWriter.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include "AddEvent.hpp"
#include "eventdatamodel.hpp"

using namespace bb::cascades;

DWriter::DWriter(bb::cascades::Application *app)
: QObject(app)
{
    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    DatabaseIo *dataio = new DatabaseIo();
    // Make the AddEvent object available to the UI as context property
    qml->setContextProperty("_addevent", new AddEvent(app, dataio));
    qml->setContextProperty("_model", new EventDataModel(app, dataio));

    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // set created root object as a scene
    app->setScene(root);
}
