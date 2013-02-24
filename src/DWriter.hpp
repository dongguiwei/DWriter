// Tabbed pane project template
#ifndef DWriter_HPP_
#define DWriter_HPP_

#include <QObject>

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class DWriter : public QObject
{
    Q_OBJECT
public:
    DWriter(bb::cascades::Application *app);
    virtual ~DWriter() {}
};

#endif /* DWriter_HPP_ */