#include "Types.hpp"

std::string role_to_str(Role role) {
	switch (role) {
		case Role::Attacker:
			return "Attacker";
		case Role::Goalkeeper:
			return "Goalkeeper";
		case Role::Defender:
			return "Defender";
		default:
			return "None";
	}
}
