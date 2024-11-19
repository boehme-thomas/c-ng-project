# Results
Different examples were implemented and tested either in cooja or using nRF52840-dk and/or -dongle boards.

To extend the functionality, [modules](/new-code/modules/MODULES.md) were implemented to set, distribute and receive schedules via the network.
In addition, an extension was created to request statistics, received statistics request, gather the statistics on the
node and send them back to the requesting node.

Additionally, the shell functionality of contiki-ng was extended with tsch-set, tsch-send and request-stats, for the
communication with a designated edge node of the network.

Next to the former, which was implemented to run on nRF52840-dk or -dongle boards, a small program was designed to run on 
a server. It requests and receives statistics, via the edge node, of all nodes in the network, creates a graph and plots it.

Experimental set-up:
- one nRF52840-dk board was selected as an edge node and flashed with [final_example.c](/new-code/final_example/final_example.c)
- all other boards (at the time of the experiment: one dk board and 4 dongle boards) where flashed with 
[final_example.c](/new-code/final_example/final_example.c) and [final_example_dongle.c](/new-code/final_example/final_example_dongle.c) respectively
- two out of the four dongle boards were equipped with an external battery

After waiting for all node to join the network and exchange some messages the function in [main.py](/new-code/server/main.py)
was executed twice and the following plots were created:

<img src="/new-code/final_example/plots/nicer_plot.png">

<img src="/new-code/final_example/plots/final_plot.png">