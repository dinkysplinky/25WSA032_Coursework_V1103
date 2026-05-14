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
KPI	               Unopt mean       1st Opt mean	New run	    vs Unopt	    vs 1st Opt
Units delivered	   77.3	            80.0	        87	        +9.7 (+12.5%)	+7.0 (+8.8%)
Weight (kg)	       1155.0	        1193.0	        1286	    +131 (+11.3%)	+93 (+7.8%)
Distance	       5565.97	        5553.20	        5535.80	    −30.2 (−0.5%)	−17.4 (−0.3%)
Energy (kWh)	   5633.3	        5626.0	        5604	    −29.3 (−0.5%)	−22.0 (−0.4%)
kg / dist	       0.21	            0.21	        0.23	    +0.02	        +0.02
Damage events      0	            1	            0	        —	            −1
Idle bots	       0	            2	            0	        —	            −2

These results are brilliant, they show how underutilised the maximum capacity of the bots were and how well my improvements have worked. 

#7- Lowered the state of charge threshold from 0.2 to 0.1 for improved charging efficiency. I optimised for this value by testing many different values, then comparing the KPI results and selecting the best constant. 

KPI 	            Unopt mean	1st Opt mean	2nd Opt mean	New 0.10 threshold      Percentage change from Unoptimised factory
Units delivered	    77.3	    80.0	        87	            89                      +15.1%
Weight (kg)	        1155.0	    1193.0	        1286	        1301                    +12.6%
Distance	        5565.97	    5553.20	        5527.50	        5532.40	                -0.5%
Energy (kWh)	    5633.3	    5626.0	        5604	        5614	                -0.3
kg / dist	        0.21	    0.21	        0.23	        0.24	                N/A
Broken bots	        0	        0.33	        0	            0	                    N/A

This table clearly and quite satisfyingly shows how my results are improving over time. 

#8- Created a new function "try_opportunistic_charge" in which a bot tests if it is within a constant radius of a charger and below a constant threshold charge. If both conditions are met the bot will carry out an opportunistic charge, charging at the charging station before continuing on its contract. I tested different combinations of suitable constants and found the most optimal combination to be charging radius of 10 units, and a charginf threshold of 0.3. 

KPI 	            Unopt mean	1st Opt mean	2nd Opt mean	3rd Opt mean      Final charging Opt    Changed from Unopt 
Units delivered	    77.3	    80.0	        87	            89                92                    19.0%
Weight (kg)	        1155.0	    1193.0	        1286	        1301              1447                  25.3%
Distance	        5565.97	    5553.20	        5527.50	        5532.40	          5491.30               -1.3%
Energy (kWh)	    5633.3	    5626.0	        5604	        5614	          5572                  -1.1%
kg / dist	        0.21	    0.21	        0.23	        0.24	          0.26                  N/A
Broken bots	        0	        0.33	        0	            0	              0                     N/A

#9- Finally I went through the code and added comments to explain it to any other future developers. I also inreased the simulation length to 52 weeks in line with the instructions set in the task 3 brief. 

| KPI               | Run 1     | Run 2     | Run 3     | Mean 
| Units delivered   | 2,249     | 2,215     | 2,252     | 2,238.7 
| Weight (kg)       | 33,938    | 33,172    | 33,966    | 33,692.0 
| Distance          | 142,865.3 | 142,972.8 | 140,780.3 | 142,206.1 
| Energy (kWh)      | 144,968   | 145,102   | 143,580   | 144,550.0 
| Broken bots       | 0         | 0         | 1         | 0.33 
| Active hrs        | 78,624    | 78,624    | 77,903    | 78,383.7 
| kg / dist         | 0.24      | 0.23      | 0.24      | 0.24 
| Units / hr        | 0.03      | 0.03      | 0.03      | 0.03 