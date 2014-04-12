Judges - FYI!

The goal of the game is to select a map, and defend headquarters (HQ) against 50 waves of attackers.
You win by successfully protecting HQ until the 50th wave has initiated. You lose if HQ is destroyed
before the 50th wave.

The main menu allows user to...
* Scroll left/right to select 1 of four maps.
* Select beginner or advanced.
* Exit program.

Click defensive unit icon on control panel to add defensive weapons (if enough credits exist). Once
placed, they may be upgraded (left-click) and/or repaired (right-click).  There is no selling of defensive
weapons.

If you exist a map level, and re-enter, the level starts all over.

In any map level, click the ? icon to display help. Should be enough to figure out game.

I've limited effects and projectile rendering in order to minimize the 
lowering of framerate below an acceptable level. However, it may still
be possible to bring it to a near halt.

There is no collision detection between attackers and defenders on land. Ran out of time.
Headerquarters should show explosions...again ran out of time.


//****************************** tips for the geeky game player
//*****************************************************************************
The program runs in windowed mode. Fullscreen works unless you tab away
from it in which case it goes off to la-la land. I lost this sometime after
version 0.015.  Fullscreen may be selected in the config.ini by changing
the value from 0 to 1.

The subfolder assets\data\ contains four level files.  Each level is
tweaked from these CSV files. Things that may be tweaked are:

//balancing factors
wavecountdivisor, 25  // attackers become stronger with each wave
                      // lower to increase damage of successive waves
					  // factor = 1 + (wave number/wavecountdivisor)
advancedfactor, 1.5   // attacker damage is multiplied by 1.5
beginnerfactor, 1     // attacker damage is multiplied by 1.0
creditmultiplier, 100 // health of killed attacker is multiplied by
                      // this multiplier and factor above
scoremultiplier, 25   // health of killed attacker is multiplied by
                      // this multiplier and factor above
upgrademultiplier1, 2 // defensive unit damage is multiplied by this amount
                      // when upgraded the 1st time, similar for 2 & 3
upgrademultiplier2, 3
upgrademultiplier3, 4
upgradecostmultiplier1, 0.75 //cost to upgrade to 1 is cost multiplied by multiplier
upgradecostmultiplier2, 1.5
upgradecostmultiplier3, 3
repaircostmultiplier1, 0.50 //def unit may be repaired. Cost is cost multiplied by multiplier.
repaircostmultiplier2, 1.0
repaircostmultiplier3, 1.5

The gc14_tool.xls contains a CSV tab which is used to manage all the
object parameters.  The button saves this to the assets folder.  The data
may be copied from here to the objects.dat file and saved.

