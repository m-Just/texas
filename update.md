**this file records all new importaupdates, please keep it clean and up-to-date**  
**Please edit and show your udpates at the top**  
  

##Update by m.Just (5.28 16:00)
###Updates in constant.h
1. Deletion of action POT, which is now combined with SHOW action; Ps. the value of QUIT was changed to 8.  
2. Deletion of stage POT_WIN, which is now combined with RIVER stage.

###Updates in opponent.cpp
1. In function *updateData()*, change the usage of *num* at SHOW stage: num = best_hand*10+nut_h.  
2. In function *updateData()*, change the usage of *stage*: all stages after RIVER are part of the stage of RIVER.  
3. In function *updateData()*, improved detection of bluff-playing.  
4. In function *jettonPara()*, added correction of the correlation coeffecient between opponents bet and jettons left.  

###Updates in opponent.h
1. In struct ANAOPP, new variable *lastbet* to store the final bet value of each round, for easier invocation.  
2. In struct ANAOPP, new variable *cc* to store the value of *coco()*.  

###Overall
1. Better accuracy in estimation concerning jetton.  
2. Improved interface.  
3. Better bluff-playing detection and soluton.  
4. Main logic in *estHand* improved
