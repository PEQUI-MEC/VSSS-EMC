#ifndef VSSS_FIELD_H
#define VSSS_FIELD_H

#include "Geometry/Geometry.h"

// A unidade de medida usada aqui é sempre me metros

namespace field {

	// Estas são as contantes definidas pelas regras da competição
	const double field_height = 1.3;
	const double field_width = 1.7;
	const double goal_width = 0.1;
	const double goal_height = 0.4;
	const double area_width = 0.15;
	const double area_height = 0.7;
	const double corner_height = 0.3;
	// -----------------------------------------------------------------------------------------------------------------

	// Enumera todas as localizações do campo de interesse
	enum class Location {
			OurField,
			TheirField,
			UpperField,
			LowerField,
			OurGoal,
			TheirGoal,
			OurBox, // Pequena área
	};
	// -----------------------------------------------------------------------------------------------------------------

	// Namespaces que com os pontos, restas etc que serão avaliados na função at_position()
	namespace center {
		Geometry::Point point( {field_width, field_height/2} );
	}

	namespace our {
		namespace area {
			namespace goalkeeper {
				Geometry::Line line( {goal_width+area_width/2, 0}, {goal_width+area_width/2, field_height} );
				Geometry::Point lower_limit( {goal_width+area_width/2, corner_height+(area_height-goal_height)/2} );
				Geometry::Point upper_limit( {goal_width+area_width/2, lower_limit.x + goal_height} );
			}
			namespace upper {
				Geometry::Point center( {goal_width+area_width/2, area_height+corner_height} );
			}
			namespace lower {
				Geometry::Point center( {goal_width+area_width/2, corner_height} );
			}
			namespace front {
				Geometry::Point center( {goal_width+area_width, field_height/2} );
			}
		}

		namespace goal {
			namespace front {
				Geometry::Line line( {goal_width, 0.0}, {goal_width, field_height});
				Geometry::Point center( {goal_width, field_height/2} );
			}

			namespace back {
				// Adicione aqui as geometrias de interesse do fundo do nosso gol
			}
		}
	}

	namespace their {
		namespace goal {
			namespace front {
				Geometry::Line line( {field_width-goal_width, 0.0}, {field_width-goal_width, field_height} );
				Geometry::Point center( {field_width-goal_width, field_height/2} );
			}
			namespace back {
				// Adicione aqui as geometrias de interesse do fundo do gol adversário
			}
		}
	}
	// -----------------------------------------------------------------------------------------------------------------

	// Função que será chamada pela estratégia para saber a localização dos agentes
	bool at_location( const Geometry::Point& position, const Location location ) {
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
			default:
				return false;
		}
	}
}



#endif //VSSS_FIELD_H
