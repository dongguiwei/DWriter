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

#include "eventdatamodel.hpp"

/**
 * The data of the EventDataModel have the following form:
 *
 * - Green
 *   + Cucumber
 *   + Peas
 *   + Salad
 * - Red
 *   + Tomato
 *   + Red Radish
 *   + Carrot
 * - Yellow
 *   + Corn
 *   + Paprika
 */
//! [0]
EventDataModel::EventDataModel(QObject *parent, DatabaseIo *dataio)
    : bb::cascades::DataModel(parent)
	, m_dataIo(dataio)
{
}
//! [0]

//! [1]
int EventDataModel::childCount(const QVariantList& indexPath)
{
    /**
     * In this very simple data model, the first two headers has 3 children
     * and the third one has 2.
     */
    const int level = indexPath.size();
    if (level == 0) { // The number of top-level items is requested
        return m_dataIo->getCount();
    }

    // The number of child items for 2nd level items is requested -> always 0
    return 0;
}
//! [1]

//! [2]
bool EventDataModel::hasChildren(const QVariantList& indexPath)
{
    // Performance is not an issue with this data model.
    // So just let childCount tell us if we have children.
    return childCount(indexPath) > 0;
}
//! [2]

/*
 * Return data as a string QVariant for any requested indexPath.
 * We could add defensive code to ensure that the data is not
 * out of range, but ListView honours the results of hasChildren
 * and childCount.
 */
//! [3]
QVariant EventDataModel::data(const QVariantList& indexPath)
{
    QString value;

    if (indexPath.size() == 1) { // Header requested
    	value = m_dataIo->getEvent(indexPath[0].toInt());
    }
/*
    if (indexPath.size() == 2) { // 2nd-level item requested
        const int header = indexPath[0].toInt();
        const int childItem = indexPath[1].toInt();

        switch (header) {
            case 0: // "Green"
                switch (childItem) {
                    case 0:
                        value = tr("Cucumber");
                        break;
                    case 1:
                        value = tr("Peas");
                        break;
                    case 2:
                        value = tr("Salad");
                        break;
                }
                break;
            case 1: // "Red"
                switch (childItem) {
                    case 0:
                        value = tr("Tomato");
                        break;
                    case 1:
                        value = tr("Red Radish");
                        break;
                    case 2:
                        value = tr("Carrot");
                        break;
                }
                break;
            case 2: // "Yellow"
                switch (childItem) {
                    case 0:
                        value = tr("Corn");
                        break;
                    case 1:
                        value = tr("Paprika");
                        break;
                }
                break;
        }
    }
*/
    qDebug() << "Data for " << indexPath << " is " << value;

    return QVariant(value);
}
//! [3]

//! [4]
QString EventDataModel::itemType(const QVariantList& indexPath)
{
    switch (indexPath.size()) {
        case 0:
            return QString();
            break;
        default:
            return QString();
            break;
    }
}
//! [4]
