#ifndef VSSS_FIELD_H
#define VSSS_FIELD_H

#include <Strategy3/Robot3.hpp>
#include "Geometry/Geometry.h"
#include "Strategy2/Robot2.h"

// A unidade de medida usada aqui é sempre me metros

namespace field {

	// Estas são as contantes definidas pelas regras da competição
	const double field_height = 1.3;
	const double field_width = 1.7;
	const double field_height_2019 = 1.26;
	const double field_width_2019 = 1.62;
	const double goal_width = 0.08;
	const double goal_height = 0.35;
	const double area_width = 0.15;
	const double area_height = 0.65;
	const double corner_height = 0.3; //distancia entre a lateral do campo e a area do gol
	const double free_ball_dist = 0.2; // distância de cobrança de falta entre bola e robô
	const double free_ball_height = 0.25; // distância entre a lateral do campo em y e o ponto de falta
	const double gk_line_x = 0.17;
	const double reach_ball_distance = (0.08 * std::sqrt(2))/2 + 0.04/2;
	// -----------------------------------------------------------------------------------------------------------------

	// Enumera todas as localizações do campo de interesse
	enum class Location {
			OurField,
			TheirField,
			UpperField,
			LowerField,
			OurGoal,
			OurLowerCorner,
			OurUpperCorner,
			TheirGoal,
			OurBox, // Pequena área
			TheirBox,
			TheirCornerAny, // Qualquer um dos cantos do time oponente
			WideDangerZone, // região do nosso campo a esquerda da linha onde os robôs oponentes cobram falta
			TheirAreaSideAny,  // laterais da área adversária (regiões da área que não ficam em frente ao gol)
			AnySide,//qualquer lateral do campo
			AnyGoal,
			OurArea,
			GoalkeeperCorner
	};
	// -----------------------------------------------------------------------------------------------------------------

	// Namespaces que com os pontos, retas etc que serão avaliados na função at_position()
	namespace center {
		const Geometry::Point point( {field_width/2, field_height/2} );
	}

	namespace our {
		namespace area {
			namespace box {
				const Geometry::Line center_line({goal_width+area_width/2, 0}, {goal_width+area_width/2, field_height});
				const Geometry::Point lower_limit({gk_line_x, corner_height+(area_height-goal_height)/2});
				const Geometry::Point upper_limit({gk_line_x, field_height-corner_height-(area_height-goal_height)/2});
				const Geometry::Point center_line_point({goal_width+area_width/2, field_height/2});
			}
			namespace upper {
				const Geometry::Point center({gk_line_x, field_height/2+area_height/2});
			}
			namespace lower {
				const Geometry::Point center({gk_line_x, field_height/2-area_height/2});
			}
			namespace front {
				const Geometry::Point center({goal_width+area_width, field_height/2});
			}
		}

		namespace free_ball {
			namespace upper {
				const Geometry::Point point({field_width/4+free_ball_dist, field_height-free_ball_height});
			}
			namespace lower {
				const Geometry::Point point({field_width/4+free_ball_dist, free_ball_height});
			}
		}

		namespace corner {
			namespace upper {
				namespace attacker {
					const Geometry::Point point({goal_width+area_width/2, corner_height+area_height+0.08});
				}
			}
			namespace lower {
				namespace attacker {
					const Geometry::Point point({goal_width+area_width/2, corner_height-0.08});
				}
			}
		}

		namespace goal {
			namespace front {
                const Geometry::Point upper_limit({goal_width, field_height/2+goal_height/2});
				const Geometry::Point lower_limit({goal_width, field_height/2-goal_height/2});
				const Geometry::Line line( {goal_width, 0.0}, {goal_width, field_height});
				const Geometry::Point center( {goal_width, field_height/2} );
			}

			namespace back {
				// Adicione aqui as geometrias de interesse do fundo do nosso gol
                const Geometry::Point center( {0.0, field_height/2} );
			}
		}
	}

	namespace their {
		namespace goal {
			namespace front {
				const Geometry::Line line( {field_width-goal_width, 0.0}, {field_width-goal_width, field_height} );
				const Geometry::Point center( {field_width-goal_width, field_height/2} );
				const Geometry::Point upper_limit({field_width-goal_width, field_height/2+goal_height/2});
				const Geometry::Point lower_limit({field_width-goal_width, field_height/2-goal_height/2});
			}
			namespace back {
				// Adicione aqui as geometrias de interesse do fundo do gol adversário
				const Geometry::Point center( {field_width, field_height/2} );
			}
		}
		namespace area {
			namespace front {
				const Geometry::Point center( {field_width-goal_width-area_width, field_height/2} );
			}
			namespace lower {
				const Geometry::Point center({field_width-goal_width-area_width/2, corner_height});
			}
			namespace upper {
				const Geometry::Point center({field_width-goal_width-area_width/2, area_height+corner_height});
			}
		}
	}

	namespace defender {
		namespace back {
			const Geometry::Point upper_limit({goal_width+area_width+0.09, field_height/2 + goal_height/2});
			const Geometry::Point lower_limit({goal_width+area_width+0.09, field_height/2 - goal_height/2});
			const Geometry::Line line(lower_limit, upper_limit);
		}
		namespace front {
			namespace upper {
				const Geometry::Point wait_point({0.62 * field_width, field_height/2+area_height/2});
			}
			namespace lower {
				const Geometry::Point wait_point({0.62 * field_width, field_height/2-area_height/2});
			}
		}
		namespace middle {
			const Geometry::Point wait_point(0.25 * field_width, field_height/2 );
		}
	}
	namespace goalkeeper {
		const Geometry::Point upper_limit({gk_line_x, field_height/2 + area_height/2 - 0.08/2});
		const Geometry::Point lower_limit({gk_line_x, field_height/2 - area_height/2 + 0.08/2});
		const Geometry::Point goal_upper_limit({gk_line_x, field_height/2 + goal_height/2});
		const Geometry::Point goal_lower_limit({gk_line_x, field_height/2 - goal_height/2});
		const Geometry::Point center({gk_line_x, field_height/2});
		const Geometry::Point reach_line({gk_line_x+reach_ball_distance, field_height/2});
	}
	// -----------------------------------------------------------------------------------------------------------------

	// Função que será chamada pela estratégia para saber a localização dos agentes
	bool at_location(const Geometry::Point& position, Location location);
	bool at_location(const Robot2& robot, Location location);
	bool at_location(const Robot3& robot, Location location);
	bool match_y(const Geometry::Point &position, const Location &location);
}



#endif //VSSS_FIELD_H
