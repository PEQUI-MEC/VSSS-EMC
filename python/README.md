# **AI module**

The AI module runs models trained and packed with the [Stable Baselines](https://stable-baselines.readthedocs.io/en/master/) Reinforcement Learning library. However, we currently use our [altered version](https://github.com/BrunoBSM/stable-baselines/tree/Beta-Dist) of such library that allows the use of Beta Distributions. It is outdated and we are working on updating everything as soon as possible.

To install it, please download the `Beta-Dist` branch in [this repository](https://github.com/BrunoBSM/stable-baselines/tree/Beta-Dist) and follow the steps to install the dependencies first, and finally the whole library with `pip install -e .`

---

This module receives data acquired by other systems, therefore some assumptions are made about it before it can be processed.

### The following assumptions are made about the data
* The vision system acquires positions in meters and in relation to the lower left corner of the field
* The center of the field is located at the mark (0.75, 0.65) meters
* The positions and orientations are treated to make the left side ALWAYS "our side"
* Orientation data comes in the range of [-pi, pi] according to the X axis that points towards the adversary's goal. Positive counterclockwise and negative clockwise.
* No orientation data is acquired for the adversary players
