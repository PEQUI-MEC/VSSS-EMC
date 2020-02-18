#ifndef VSSS_WRAPPER
#define VSSS_WRAPPER

#include "LS.h"
#include "../RobotControl/SimuRobotControl.h"
#include "Attacker.h"
#include "Defender.hpp"
#include "Goalkeeper.hpp"
#include "Strategy2.hpp"
#include "../Geometry/Geometry.h"

static Attacker attacker;
static Defender defender;
static Goalkeeper goalkeeper;
static LS ls_x, ls_y;
static Geometry::Point ball;
static Geometry::Point ball_est;
static Strategy2 strategy(attacker, defender, goalkeeper, ball, ball_est);
static std::array<Robot2 *, 3> robots{&attacker, &defender, &goalkeeper};

struct Velocities {
		Control::WheelVelocity vel1;
		Control::WheelVelocity vel2;
		Control::WheelVelocity vel3;
};

void init();
Robot2 * get_robot(int tag);
void update_ball_est();
void run(float robot1data[5], float robot2data[5], float robot3data[5], float ballpos[2], float time, float out[6]);


#endif //VSSS_WRAPPER
