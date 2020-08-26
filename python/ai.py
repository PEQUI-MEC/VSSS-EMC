import gym
import math
import random
import numpy as np

from copy import deepcopy

from stable_baselines.common.policies import FeedForwardPolicy
from MultiAgent import PPO2

MAX = 7.42723067783*360*3.14159265359/180 # radiandos por segundo, para que o robô atinja 1,4m/s


class AI:
    def __init__(self, our_goal=[-0.8,0], their_goal=[0.8,0], game_time=600, path="/home/thiago/Workspace/VSSS-EMC/python/models/model.zip", num_frames=8):

        # game_time: game time in seconds (suggestion: use half the match as you can reset after halftime)

        self.OUR_GOAL = our_goal # position [x,y] (suggestion: set it somewhere between the goalie line and the goal back "wall")
        self.THEIR_GOAL = their_goal # position [x,y]
        self.OUR_PLAYERS = 3 # number of players on "our" Team
        self.THEIR_PLAYERS = 3 # number of players on "their" Team
        self.DEFAULT_PLAYERS = 3
        self.num_frames = num_frames
        print("Our goal (x, y) is " + str(self.OUR_GOAL[0]) + ", " + str(self.OUR_GOAL[1]))
        print("Adv goal (x, y) is " + str(self.THEIR_GOAL[0]) + ", " + str(self.THEIR_GOAL[1]))
        self.game_time = game_time*10
        self.time = game_time

        self.state = []
        for i in range(self.OUR_PLAYERS):
            self.state.append(np.zeros((0),'float32'))

        self.last_actions = np.array([0.0 for i in range(self.OUR_PLAYERS*2)])

        try:
            self.model = PPO2.load(path)
        except:
            raise Exception("Please provide a valid path to the model.zip file in AI(path='path')")

    def reset_timer(self, game_time=None):
        # Resets the timer to the chosen game_time

        if game_time:
            self.time = game_time
        else:
            self.time = self.game_time


    def distance(self, a,b):
        # Cartesian distance between two points
        return ((a[0] - b[0])**2 + (a[1] - b[1])**2)**(1/2)

    def run_strategy(self, robot0, robot1, robot2, ball, adv0, adv1, adv2, is_start=False):
        #print("Robot 0 (x, y, theta) is " + str(robot0[0]) + ", " + str(robot0[1]) + ", " + str(robot0[2]))
        #print("Robot 1 (x, y, theta) is " + str(robot1[0]) + ", " + str(robot1[1]) + ", " + str(robot1[2]))
        #print("Robot 2 (x, y, theta) is " + str(robot2[0]) + ", " + str(robot2[1]) + ", " + str(robot2[2]))
        #print("Adv 0 (x, y) is " + str(adv0[0]) + ", " + str(adv0[1]))
        #print("Adv 1 (x, y) is " + str(adv1[0]) + ", " + str(adv1[1]))
        #print("Adv 2 (x, y) is " + str(adv2[0]) + ", " + str(adv2[1]))
        #print("Ball is (x, y) " + str(ball[0]) + ", " + str(ball[1]))
        #print("Is start: " + str(is_start))
        if (is_start):
            print("Play pressed!!!");

        # The Following assumptions are made about the data
        # The vision system acquires positions in meters and in relation to the lower left corner of the field
        # The center of the field is located at the mark (0.75, 0.65) meters
        # The positions and orientations are treated to make the left side ALWAYS "our side"
        # Orientation data comes in the range of [-pi, pi] according to the X axis that points towards the adversary's goal
        # No orientation data is acquired for the adversary players

        robot0, robot1, robot2, ball, adv0, adv1, adv2 = self.cartesian_translate([robot0, robot1, robot2, ball, adv0, adv1, adv2])

        self.mount_inputs(robot0, robot1, robot2, ball, adv0, adv1, adv2, is_start) # Gera inputs [1,2,3] na variável self.state

        actions, _states = self.model.predict(np.asarray(self.state), deterministic=True)

        actions = ( 2 * actions.reshape( (3,2) ) -1.0 ) * MAX # rescaling
        actions = np.clip(actions, -MAX, MAX)

        self.last_actions = actions.flatten() # this operation does not affect 'actions'
        self.time = self.time-1

        # Velocidades em m/s dos robos 1, 2 e 3
        # return ((0.2, 0.3), (0.5, 0.1), (0.8, 1.2))
        return tuple(tuple(sub) for sub in actions)

    def cartesian_translate(self, elements):
        # Change the origin of a cartesian system for all points in the list
        new_origin = [0.85, 0.65]
        for obj in elements:
            obj[0] = obj[0] - new_origin[0]
            obj[1] = obj[1] - new_origin[1]

        return tuple(elements)


    def mount_inputs(self, robot0, robot1, robot2, ball, adv0, adv1, adv2, is_start):

        Y_frame_xy, Y_frame_rot, Specifics, Specifics_dists = self.preprocess_info(robot0, robot1, robot2, ball, adv0, adv1, adv2)
        self.mount(Y_frame_xy, Y_frame_rot, Specifics, Specifics_dists, is_start=is_start)


    def preprocess_info(self, robot0, robot1, robot2, ball, adv0, adv1, adv2):
        # Use the information to genereta the normalized input data by calculating distances, sin and cos

        our_robots = [robot0, robot1, robot2]
        their_robots = [adv0, adv1, adv2]
        # Entradas cartesianas em metros com origem no centro do campo

        Y_frame_pos = np.zeros((0),'float32')
        Y_frame_rot = np.zeros((0),'float32')
        B_frame_pos = np.zeros((0),'float32')
        B_frame_rot = np.zeros((0),'float32')
        ball_pos = np.zeros((0),'float32')
        Specifics = []
        Specifics_dists = []
        for i in range(self.OUR_PLAYERS):
            Specifics.append(np.zeros((0),'float32'))
            Specifics_dists.append(np.zeros((0),'float32'))
        Robots = [[[0,0,0] for r in range(self.OUR_PLAYERS)]for _ in range(2)]
        Ball = [ball[0], ball[1]]
        # if field[0]:
        #     Goal_B = GOAL_RIGHT
        #     Goal_Y = GOAL_LEFT
        # else:
        #     Goal_B = GOAL_LEFT
        #     Goal_Y = GOAL_RIGHT

        ball_pos = np.append(ball_pos, (Ball[0])/0.85)
        ball_pos = np.append(ball_pos, (Ball[1])/0.65)
        sin_BG_Y = math.sin(math.atan2(Ball[1] - self.OUR_GOAL[1], Ball[0] - self.OUR_GOAL[0]))
        cos_BG_Y = math.cos(math.atan2(Ball[1] - self.OUR_GOAL[1], Ball[0] - self.OUR_GOAL[0]))
        Dist_BG_Y = self.distance(Ball,self.OUR_GOAL)/1.82
        sin_BG_B = math.sin(math.atan2(Ball[1] - self.THEIR_GOAL[1], Ball[0] - self.THEIR_GOAL[0]))
        cos_BG_B = math.cos(math.atan2(Ball[1] - self.THEIR_GOAL[1], Ball[0] - self.THEIR_GOAL[0]))
        Dist_BG_B = self.distance(Ball,self.THEIR_GOAL)/1.82


        for r in range(self.OUR_PLAYERS):
            Robots[0][r] = deepcopy(our_robots[r])
            Y_frame_pos = np.append(Y_frame_pos, (Robots[0][r][0])/0.85) #normalizado com metade do campo uma vez que a posição ja pode ser negativa
            Y_frame_pos = np.append(Y_frame_pos, (Robots[0][r][1])/0.65) #normalizado com metade do campo


        for r in range(self.OUR_PLAYERS):
            Y_frame_rot = np.append(Y_frame_rot, math.sin(Robots[0][r][2]))
            Y_frame_rot = np.append(Y_frame_rot, math.cos(Robots[0][r][2]))
            Y_frame_rot = np.append(Y_frame_rot, (math.atan2(Y_frame_rot[-2:][0],Y_frame_rot[-2:][1]))/math.pi)



        for r in range(self.THEIR_PLAYERS):
            Robots[1][r] = deepcopy(their_robots[r])
            B_frame_pos = np.append(B_frame_pos, (Robots[1][r][0])/0.85) #normalizado com metade do campo uma vez que a posição ja pode ser negativa
            B_frame_pos = np.append(B_frame_pos, (Robots[1][r][1])/0.65) #normalizado com metade do campo


        # for r in range(self.THEIR_PLAYERS):
        #     B_frame_rot = np.append(B_frame_rot, math.sin(Robots[1][r][2]))
        #     B_frame_rot = np.append(B_frame_rot, math.cos(Robots[1][r][2]))
        #     B_frame_rot = np.append(B_frame_rot, (math.atan2(B_frame_rot[-2:][0],B_frame_rot[-2:][1]))/math.pi)



        for r in range(self.OUR_PLAYERS):
            Specifics[r] = np.append(Specifics[r], math.sin(math.atan2(Ball[1] - Robots[0][r][1], Ball[0] - Robots[0][r][0])) )
            Specifics[r] = np.append(Specifics[r], math.cos(math.atan2(Ball[1] - Robots[0][r][1], Ball[0] - Robots[0][r][0])) )
            Specifics[r] = np.append(Specifics[r], (math.atan2(Specifics[r][-2:][0], Specifics[r][-2:][1])) /math.pi)
            Specifics[r] = np.append(Specifics[r], sin_BG_Y )
            Specifics[r] = np.append(Specifics[r], cos_BG_Y )
            Specifics[r] = np.append(Specifics[r], (math.atan2(Specifics[r][-2:][0], Specifics[r][-2:][1])) /math.pi)
            Specifics[r] = np.append(Specifics[r], sin_BG_B )
            Specifics[r] = np.append(Specifics[r], cos_BG_B )
            Specifics[r] = np.append(Specifics[r], (math.atan2(Specifics[r][-2:][0], Specifics[r][-2:][1])) /math.pi)
            Specifics_dists[r] = np.append(Specifics_dists[r], self.distance(Ball,Robots[0][r])/2.14 ) #((1.7)**2 + (1.3)**2)**(1/2) ~ 2.14
            Specifics_dists[r] = np.append(Specifics_dists[r], Dist_BG_Y ) #((1.7)**2 + (1.3/2)**2)**(1/2) ~ 1.82 || uma vez que o gol se encontra na metade do eixo Y, sua distância máxima é diferente
            Specifics_dists[r] = np.append(Specifics_dists[r], Dist_BG_B ) #((1.7)**2 + (1.3/2)**2)**(1/2) ~ 1.82 || uma vez que o gol se encontra na metade do eixo Y, sua distância máxima é diferente
            for a in range(self.DEFAULT_PLAYERS):
                if a >= self.OUR_PLAYERS: #Desta forma, o espaço de estados sempre mantém o mesmo tamanho, zerando os sinais de jogadores que não estão em jogo
                    Specifics[r] = np.append(Specifics[r], 0.0 )
                    Specifics[r] = np.append(Specifics[r], 0.0 )
                    Specifics[r] = np.append(Specifics[r], (math.atan2(Specifics[r][-2:][0], Specifics[r][-2:][1])) /math.pi)
                    Specifics_dists[r] = np.append(Specifics_dists[r], 0.0 )
                elif a != r:
                    # print("before",Specifics[r])
                    Specifics[r] = np.append(Specifics[r], math.sin(math.atan2(Robots[0][a][1] - Robots[0][r][1], Robots[0][a][0] - Robots[0][r][0])) )
                    Specifics[r] = np.append(Specifics[r], math.cos(math.atan2(Robots[0][a][1] - Robots[0][r][1], Robots[0][a][0] - Robots[0][r][0])) )
                    Specifics[r] = np.append(Specifics[r], (math.atan2(Specifics[r][-2:][0], Specifics[r][-2:][1])) /math.pi)
                    Specifics_dists[r] = np.append(Specifics_dists[r], self.distance(Robots[0][a],Robots[0][r])/2.14 ) #((1.7)**2 + (1.3)**2)**(1/2) ~ 2.14
                    # print("after",Specifics[r])
            for b in range(self.DEFAULT_PLAYERS):
                if b >= self.THEIR_PLAYERS:
                    Specifics[r] = np.append(Specifics[r], 0.0 )
                    Specifics[r] = np.append(Specifics[r], 0.0 )
                    Specifics[r] = np.append(Specifics[r], (math.atan2(Specifics[r][-2:][0], Specifics[r][-2:][1])) /math.pi)
                    Specifics_dists[r] = np.append(Specifics_dists[r], 0.0 )
                else:
                    Specifics[r] = np.append(Specifics[r], math.sin(math.atan2(Robots[1][b][1] - Robots[0][r][1], Robots[1][b][0] - Robots[0][r][0])) )
                    Specifics[r] = np.append(Specifics[r], math.cos(math.atan2(Robots[1][b][1] - Robots[0][r][1], Robots[1][b][0] - Robots[0][r][0])) )
                    Specifics[r] = np.append(Specifics[r], (math.atan2(Specifics[r][-2:][0], Specifics[r][-2:][1])) /math.pi)
                    Specifics_dists[r] = np.append(Specifics_dists[r], self.distance(Robots[1][b],Robots[0][r])/2.14 ) #((1.7)**2 + (1.3)**2)**(1/2) ~ 2.14


        # Y_frame_pos = np.concatenate((Y_frame_pos, ball_pos), axis=0)
        # B_frame_pos = np.concatenate((B_frame_pos, ball_pos), axis=0)
        Y_frame_xy = np.concatenate((Y_frame_pos, B_frame_pos, ball_pos),axis=0)
        # B_frame_xy = np.concatenate((B_frame_pos, Y_frame_pos, ball_pos),axis=0)

        return Y_frame_xy, Y_frame_rot, Specifics, Specifics_dists


    def mount(self, frame_pos, frame_rot, Specifics, Specifics_dists, is_start=False):
        # Gathers the processed information to mount the input and pile up readings

        # is_start: shows if it is or not a starting state, if it is, the previous readings
        # are discarted and the new reading is repeated. Otherwise the new reading is piled
        # on top of the old ones.

        if is_start:
            for l_ac in self.last_actions:
                l_ac = 0.0

        Specifics = np.asarray(Specifics, 'float32')
        Specifics_dists = np.asarray(Specifics_dists, 'float32')

        for i in range(self.OUR_PLAYERS):
            # s = np.concatenate((frame_pos,frame_rot), axis=0)
            # print(Specifics)
            # print(i)
            # print("0 %i"%len(frame_pos[i*2:(i*2)+2]))
            # print("1 %i"%len(np.concatenate((frame_pos[:2*i], frame_pos[(2*i)+2:]))))
            # print("2 %i"%len(frame_pos[self.Players[team]*2:]))
            # print('--------------------------------------')
            # print(frame_pos[i*2:(i*2)+2])
            # print(np.concatenate((frame_pos[:2*i], frame_pos[(2*i)+2:])))
            # print(frame_pos[self.Players[team]*2:])
            # print("3 %i"%len(frame_rot[i*3:(i*3)+3]))
            # print("4 %i"%len(Specifics[i]))
            # print("5 %i"%len(Specifics_dists[i]))
            # print("6 %i"%len(np.concatenate((Specifics_dists[...,0][:i],Specifics_dists[...,0][i+1:]))))
            # print(self.sim.data.ctrl)
            s = np.concatenate((frame_pos[i*2:(i*2)+2], frame_pos[:2*i], frame_pos[(2*i)+2:],
                                frame_rot[i*3:(i*3)+3],
                                Specifics[i],
                                Specifics_dists[...,0][:i], Specifics_dists[...,0][i+1:], Specifics_dists[i],
                                self.last_actions[(i*2):(i*2)+2]/MAX, self.last_actions[:(i*2)]/MAX, self.last_actions[(i*2)+2:]/MAX,
                                np.array([self.time/self.game_time])
                                ), axis=0)
            # print("s %i"%len(s))
            # print(s)
            # caso seja o início de um novo jogo, ou falta, gol etc o mesmo frame é copiado
            # if is_start:
            #     print(s)
            #     print("--------------------------------------------------------------------\n")
            if is_start:
                self.state[i] = np.delete(self.state[i], range(len(self.state[i])))
                for _ in range(self.num_frames):
                    self.state[i] = np.concatenate((self.state[i], s), axis=0)
            else:
                self.state[i] = np.delete(self.state[i], range(len(s)))
                self.state[i] = np.concatenate((self.state[i], s), axis=0)

    def random_test_AI(self):
        # Test the AI with random positions and orientations

        # Generate positions and orientations
        robots = [[random.uniform(0.0, 1.7), random.uniform(0.0, 1.3), random.uniform(-math.pi, math.pi)] for _ in range(self.OUR_PLAYERS)]
        advs = [[random.uniform(0.0, 1.7), random.uniform(0.0, 1.3)] for _ in range(self.THEIR_PLAYERS)]
        ball = [random.uniform(0.0, 1.7), random.uniform(0.0, 1.3)]

        actions = self.run_strategy(robot0 = robots[0], robot1 = robots[1], robot2 = robots[2], ball = ball, adv0 = advs[0], adv1 = advs[1], adv2 = advs[2], is_start=True)
        print(actions)

        return actions
