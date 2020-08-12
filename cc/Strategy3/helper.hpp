#ifndef VSSS_HELPER_HPP
#define VSSS_HELPER_HPP

#include "Robot3.hpp"
#include "Ball.hpp"

bool has_ball(const Robot3* robot, const Ball& ball);

bool is_ball_behind(const Geometry::Point& point, const Ball& ball);

bool is_ball_behind(const Robot3 *robot, const Ball& ball);

bool is_ball_est_ahead(const Geometry::Point& point, const Ball& ball, double offset);

#endif //VSSS_HELPER_HPP
