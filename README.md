# CPR_E_288_Final
Final Project for my CPR E 288 class. We control a roomba, with PING and IR sensors, and move it to a location while avoiding obstacles.

We used python libraries to listen to the port that the roomba broadcasted to the desktop in the lab. The putty on the desktop(where we had written the original roomba code, now on the roomba) received the information that the roomba got from its Ping, IR, bump, and light sensors. This allowed us to create a 2D map of the floor and move our roomba towards a final location.
