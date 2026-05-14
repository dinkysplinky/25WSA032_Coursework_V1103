# READ ME

This file is in the 'root of your project.
You can use this to provide top-level documentation of your project

Commit documentation 
Robots 

#1-  Initial Commit, pasted example code into a new file and tested the tabulate function. 

#2- Changed the position of the charger to the home position [40,20]. Changed the simulation length to 52 weeks. 

#3- Added KPI functions (collection, table layout and printing functions), printed an info table for each bot at the end of the programme in a vertical format to fit in the terminal. 

#4- Added charger distance and comparison functions to return the closest charger for every bot. Haven't implemented to main sequence for KPI comparison. Below are the results of the unoptimised simulation, simulated for 2 weeks in which each bot must return to the charging station in the centre of the factory to charge. This will serve as a control set of KPIs which I'll use to validate and measure my optimisations' effectiveness.

NOTE: throughout this document I will compare optimisation results to their predecessors, not back to the original each time. While this may slightly reduce accuracy of improvement measurements, it will avoid compounding improvement errors and allow for a more direct comparison, as well as confirm the two optimisations work together in the code. 

UNOPTIMISED RESULTS:
KPI 	           Run 1	Run 2	Run 3	Mean
Units delivered	   80	    76	    76	    77.3
Weight (kg)	       1196	    1088	1181	1155.0
Distance	       5559.7	5571.6	5566.6	5565.97
Energy (kWh)	   5624	    5636	5640	5633.3
Broken bots	       0	    0	    0	    0
Active hrs	       3024	    3024	3024	3024
kg / dist	       0.22	    0.20	0.21	0.21
Units / hr	       0.03     0.03	0.03	0.03


Droids 
KPI 	           Run 1	Run 2	Run 3	Mean
Units delivered	   29	    26	    25	    26.7
Weight (kg)	       430	    395	    390	    405.0
Distance	       ~1879	~1892	~1892	~1888
Energy (kWh)	   1880	    1892	1892	1888.0

Drones
KPI 	           Run 1	Run 2	Run 3	Mean
Units delivered	   37	    36	    39	    37.3
Weight (kg)	       572	    503	    611	    562.0
Distance	       2711.98	2712.21	2700.64	2708.28
Energy (kWh)	   1808	    1808	1800	1805.3

Robots 
KPI	               Run 1	Run 2	Run 3	Mean
Units delivered	   14	    14	    12	    13.3
Weight (kg)	       194	    190	    180	    188.0
Distance	       968.05	967.83	973.92	969.93
Energy (kWh)	   1936	    1936	1948	1940.0

From this dataset I noted the following:
1: Drones make the highest mean deliveries at 37.3 units and 562 kg, they also cover the most distance at lowest energy cost.
2: Robots don't utilise their strengths enough. They have a very high carrying capacity (125kg) but average just ~14kg per delivery. 

#5- Implemented the charging station selection function into the main operation sequence so that bots always choose the closest of the four chargers to them. 

CHARGER LOCATION OPTIMISED RESULTS: (overview and comparison)
KPI	               Run 1	Run 2	Run 3	Mean	vs unopt  Change(%)
Units delivered	   83	    78	    79	    80.0	+2.7      +3.5
Weight (kg)	       1229	    1156	1194	1193.0	+38.0     +3.3
Distance	       5527.50	5575.50	5556.60	5553.20	−12.8     -0.2
Energy (kWh)	   5610	    5636	5632	5626.0	−7.3      -0.1

These results show the efficacy of my charging optimisations, since the mean number of deliveries has increased, as well as the total weight delivered increasing and the distane travelled and energy consumed decreasing. I would predict that adding even more chargers to the factory floor would increase the performance further (most likely tending towards a final value). However, in the spirit of keeping the simulation as applicable to a real world factory floor as possible, I shall not be adding a charging station to every single coordinate point on the grid. 

#6- Removed the break after bot.deliver() so bots keep loading pizzas until they're close to their max payload. This improves delivery efficiency since bots (especially robots with their high max capacity) can deliver multiple pizzas in the same trip, reduing time wasted travelling back and forth for each trip individually. 

BOT LOADING OPTIMISED RESULTS: (overview and comparison)
KPI	               Original mean    Previous mean	New run	    vs original	    vs previous
Units delivered	   77.3	            80.0	        87	        +9.7 (+12.5%)	+7.0 (+8.8%)
Weight (kg)	       1155.0	        1193.0	        1286	    +131 (+11.3%)	+93 (+7.8%)
Distance	       5565.97	        5553.20	        5535.80	    −30.2 (−0.5%)	−17.4 (−0.3%)
Energy (kWh)	   5633.3	        5626.0	        5604	    −29.3 (−0.5%)	−22.0 (−0.4%)
kg / dist	       0.21	            0.21	        0.23	    +0.02	        +0.02
Damage events      0	            1	            0	        —	            −1
Idle bots	       0	            2	            0	        —	            −2

These results are brilliant, they show how underutilised the maximum capacity of the bots were and how well my improvements have worked. 

