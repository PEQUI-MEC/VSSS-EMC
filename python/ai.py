#import gym
#from stable_baselines.common.policies import MlpPolicy
#from stable_baselines.common.vec_env import DummyVecEnv
#from stable_baselines import PPO2


class AI:
    def __init__(self, our_goal, their_goal):
        self.our_goal = our_goal
        self.their_goal = their_goal
        print("Our goal (x, y) is " + str(self.our_goal[0]) + ", " + str(self.our_goal[1]))
        print("Adv goal (x, y) is " + str(self.their_goal[0]) + ", " + str(self.their_goal[1]))
    
    def run_strategy(self, robot0, robot1, robot2, ball, adv0, adv1, adv2):
        print("Robot 0 (x, y, theta) is " + str(robot0[0]) + ", " + str(robot0[1]) + ", " + str(robot0[2]))
        print("Robot 1 (x, y, theta) is " + str(robot1[0]) + ", " + str(robot1[1]) + ", " + str(robot1[2]))
        print("Robot 2 (x, y, theta) is " + str(robot2[0]) + ", " + str(robot2[1]) + ", " + str(robot2[2]))
        print("Adv 0 (x, y) is " + str(adv0[0]) + ", " + str(adv0[1]))
        print("Adv 1 (x, y) is " + str(adv1[0]) + ", " + str(adv1[1]))
        print("Adv 2 (x, y) is " + str(adv2[0]) + ", " + str(adv2[1]))
        print("Ball is (x, y) " + str(ball[0]) + ", " + str(ball[1]))
        # Velocidades em m/s dos robos 1, 2 e 3
        return ((0.2, 0.3), (0.5, 0.1), (0.8, 1.2))
