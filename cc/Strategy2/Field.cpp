#include "Field.h"

using namespace field;

// Função que será chamada pela estratégia para saber a localização dos agentes
bool field::at_location( const Geometry::Point& position, const Location location ) {
	switch( location )
	{
		case Location::OurField:
			return position.x < center::point.x;
		case Location::TheirField:
			return position.x >= center::point.x;
		case Location::UpperField:
			return position.y >= center::point.y;
		case Location::LowerField:
			return position.y < center::point.y;
		case Location::OurGoal:
			return position.x < our::goal::front::center.x;
		case Location::TheirGoal:
			return position.x > their::goal::front::center.x;
		case Location::OurBox:
			return position.x >= our::goal::front::center.x && position.x <= our::area::front::center.x
				   && position.y >= our::area::lower::center.y && position.y <= our::area::upper::center.y;
		case Location::TheirBox:
			return position.x <= their::goal::front::center.x && position.x >= their::area::front::center.x
				   && position.y >= their::area::lower::center.y && position.x >= their::area::lower::center.y;
		case Location::TheirCornerAny:
			return position.x >= their::area::front::center.x && (position.y >= their::area::upper::center.y
																  || position.y <= their::area::lower::center.y);
		case Location::WideDangerZone:
			return position.x < our::free_ball::upper::point.x;

		default:
			return false;
	}
}

bool field::at_location(const Robot2& robot, const Location location)
{
	return at_location(robot.get_position(), location);
}

