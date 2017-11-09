#ifndef CONSTANTS_HPP_
#define CONSTANTS_HPP_

#include <math.h>

namespace CONST {

    //class Const {
        static bool GOAL_DANGER_ZONE,
            ADV_NA_AREA;
    
        static int height, width,
            ABS_PLAYING_FIELD_WIDTH,
            ABS_GOAL_TO_GOAL_WIDTH,
            COORD_MID_FIELD_X,
            ABS_FIELD_HEIGHT,
            ABS_GOAL_SIZE_Y,
            ABS_GOAL_SIZE_X,
            COORD_GOAL_DEF_FRONT_X,
            COORD_GOAL_ATK_FRONT_X,
            COORD_GOAL_MID_Y,
            COORD_GOAL_UP_Y,
            COORD_GOAL_DWN_Y,
            ABS_BOX_SIZE_Y,
            ABS_BOX_SIZE_X,
            COORD_BOX_DEF_X,
            COORD_BOX_ATK_X,
            COORD_BOX_UP_Y,
            COORD_BOX_DWN_Y,
            ABS_BALL_SIZE,
            ABS_ROBOT_SIZE,

            COMPRIMENTO_CAMPO_TOTAL,
            COMPRIMENTO_PISTA,
            LARGURA_CAMPO,
            TAMANHO_GOL,
            TAMANHO_AREA,
            BANHEIRA,
            DIVISAO_AREAS,
            OFFSET_BANHEIRA,
            MEIO_GOL_X,
            MEIO_GOL_Y,
            MAX_GOL_Y,
            MIN_GOL_Y,
            LINHA_ZAGA,
            LIMITE_AREA_X,
            // Parametros para atacante sem bola
            OFFSET_RATIO,
            CONE_RATIO,
            COUNT_DECISION,
            MAX_DECISION_COUNT,
            TARGET_LOCK_COUNT,
            MAX_TARGET_LOCK_COUNT,
            // Parametros do Defensor na defesa
            DESLOCAMENTO_ZAGA_ATAQUE,
            BALL_RADIUS,

            corner_atk_limit,
            def_corner_line,
            def_line,
            possession_distance,
            max_approach,
            fixed_pos_distance,
            collision_radius,
            max_collision_count,
            goalie_line,
            goalie_offset,
            transition_back_radius,
            transition_y_distance,
            acceleration;
    //};
}

#endif
