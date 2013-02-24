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

#ifndef EVENTDATAMODEL_HPP
#define EVENTDATAMODEL_HPP

#include "databaseio.hpp"
#include <bb/cascades/DataModel>

//! [0]
class EventDataModel : public bb::cascades::DataModel
{
    Q_OBJECT
public:
    EventDataModel(QObject *parent = 0, DatabaseIo *dataio = 0);

    // Required interface implementation
    virtual int childCount(const QVariantList& indexPath);
    virtual bool hasChildren(const QVariantList& indexPath);
    virtual QVariant data(const QVariantList& indexPath);
    virtual QString itemType(const QVariantList& indexPath);

private:
    DatabaseIo *m_dataIo;
};
//! [0]

#endif
