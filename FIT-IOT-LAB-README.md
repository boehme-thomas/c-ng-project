# FIT Iot-Lab
This gives a short overview of the FIT Iot-Lab and how to do an experiment. For more details visit 
https://www.iot-lab.info/ or https://www.iot-lab.info/docs/.

## Info
- FIT IoT-Lab is a remote testbed where on five different sites 18 different boards are available.
- Registration and usage is generally free of charge, but an academic or professional email address is required, also
FIT IoT-Lab has to be cited, if results are found using the lab.
- For this project the nRF52840 DK nodes are of particular interest:
  - There are 10 different nodes available
  - All of them placed in Saclay in two lines of 5 perpendicular to each other and on the same height
  - The boards are equipped with a ST X-NUCLEO-IKS01A2 shield, enabling the usage of different sensors

## Deployment of an experiment
Different ways of creating an experiment are available. In general, it is possible to create experiments via the User
Interface on the FIT IoT-Lab website or via CLI or even a mix of both.
### GUI
It can be accessed on https://www.iot-lab.info/testbed/dashboard.

#### Firmware and Monitoring profiles (My resources tab)
Before creating an experiment, it can be advantageous to first create a firmware and if necessary a monitoring profile.
Firmware can be build locally and then be uploaded. There is a possibility to give them architecture and OS infos as well as 
description for a better overview.

A Monitoring profile describes which parameters should be monitored while the experiment is running. Different profiles 
for different architectures can be created. Consumption with current, power and voltage and/or radio on different
channels can be monitored. The monitoring is saved in the ~\.iot-lab\<experiment_id> folder.

#### Creating experiment (New experiment tab)
To create a new experiment, a name, the duration and the start time has to be chosen. After node selection, which can be
done via the node properties, host name / map or via the node id, the firmware and a monitoring profile can be selected/created.
Important to note is, that when the nodes are chosen via id or map it is not given that the nodes are in range of each other, 
especially for m3 or m3-a8 nodes which are deployed at a large scale.
After the submission the experiment will be scheduled and executed accordingly.

The firmware can also be uploaded and monitoring profiles be selected via CLI, when the experiment is already running.

### CLI
Next to the GUI way it is also possible to start an experiment via an CLI.

At first the user has to login to the testing site. Then the firmware needs to be downloaded (e.g. via wget).
After that an experiment can be submitted with parameters to choose the name, timeslot, duration, number and architecture of 
nodes, the firmware and monitoring profile.

For example with this command:

&emsp; _iotlab-experiment submit -n first-exp -d 20 -l 2,archi=m3:at86rf231+site=grenoble,tutorial_m3.elf_

After the submission, the experiment is scheduled respectively. It is also possible to create an experiment and flash the
firmware to the node afterwards. It is also possible to create, select and delete monitoring profiles via the CLI.

For more information about CLI functionality and the exact commands visit: https://www.iot-lab.info/docs/tools/cli/

## Running experiment
After the experiment is submitted the scheduler will select suitable node and start the experiment in the selected timeslot.
The status of the experiment will change to _running_.
- A running experiment can always be stopped, e.g. when the deployment to the nodes was unsuccessful. 
- When no firmware was selected before the submission of an experiment, it can be done afterwards.
- Nodes:
  - can be stopped, started, or restarted
  - a debugger can be started and stopped
  - and monitoring configurations can be updated

While an experiment is running there is the possibility to connect to a selected node:

&emsp; _nc nrf52840dk-\<node_id\> 20000_

Now the sent and received messages are shown and it is possible to perform certain actions like sending radio packets or read sensors.

## Evaluation after an experiment
When a monitoring profile is selected for a node, the collected data is stored in the home directory ~/.iot-lab/last/ and
~/.iot-lab/<exp_id>/ as an OML file, after the experiment has successfully finished or was stopped.
It can be viewed on the CLI, but for a better overview the lab also provides plotting tools (login with _ssh -X ..._ 
beforehand to view the plot on your local machine):
- For consumption:

&emsp; &emsp; _plot_oml_consum -p -i ~/.iot-lab/\<exp_id\>/consumption/\<node\>.oml_
- For radio: 

&emsp; &emsp; _plot_oml_radio -a -i ~/.iot-lab/\<exp_id\>/radio/\<node\>.oml_

## Useful commands
Here a short list of useful CLI commands are given. For more information or more details visit: https://www.iot-lab.info/docs/tools/cli/.

login to a specific test site (with _\<site\>_ in {lille, paris, saclay, strasbourg, toulouse}): 

&emsp; _ssh \<login\>@\<site\>.iot-lab.info_

submit experiment (example):

&emsp; _iotlab-experiment submit -n first-exp -d 20 -l 2,archi=m3:at86rf231+site=grenoble,tutorial_m3.elf_

flash firmware of nodes of running experiment (if not already selected):

&emsp; _iotlab-node -- update_

to get a status of nodes; can be filtered by site, architecture, ...:

&emsp; _iotlab-status --nodes --archi nrf52840dk --site saclay_

to get information about the sites and the available nodes:

&emsp; _iotlab-status --sites_

plot consumption monitoring data: 

&emsp; _plot_oml_consum -p -i ~/.iot-lab/\<exp_id\>/consumption/\<node_id\>.oml_ 

plot radio monitoring data:

&emsp; _plot_oml_radio -a -i ~/.iot-lab/\<exp_id\>/radio/\<node_id\>.oml_
