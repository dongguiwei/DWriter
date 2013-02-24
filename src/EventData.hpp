/*
 * EventData.hpp
 *
 *  Created on: Feb 14, 2013
 *      Author: daviddong
 */

#ifndef EVENTDATA_HPP_
#define EVENTDATA_HPP_

namespace bb { namespace cascades { class Application; }}

typedef struct Position_t
{
	int longitude;
	int latitude;
} Position;

typedef struct EventData_t
{
//	time_t time_stamp;
//	QString  text;
	char*  picture;
	char*  video;
	char*  voice;
	Position pos;
} EventData;


#endif /* EVENTDATA_HPP_ */
