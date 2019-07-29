<h2>Heart-Monitor-Simulator</h2>

Embedded systems software that simulates a medical monitoring device written for a TI Stellaris Board. This was developed through IAR development environment. 

<hr>
<h4>Introduction:</h4>
The goal of this project was to create a simulated medical device in C. For this project we are using a Texas Instruments Stellaris EKI-LM3S8962 development board with a ARM Cortex-M3 v7M microcomputer and coding in the IAR programming environment (IDE) (version 8.210). Our team also used Microsoft Visual Studio 2017 for source control management with Github. With some of the hardware related methods, our team will be using  a combination of the tasks that are used in the example stellaris projects to access specific pins, ports, LCD screen, and other available hardware on the Stellaris board.. 
<img src="/README_images/usecase.png" width="400">
<hr>

<h3>Features</h3>

<strong>Name: Measure </strong><br>
<strong>Description:</strong> Gets temperature, blood pressure ,and pulse rate information from the patient and brings it into the medical device system. <br>
<strong>Exception: </strong>If an external source interferes with the device, this task could possibly not trigger or function incorrectly by reading corrupted data.<br>

<img src="/README_images/blood-pressure.png" width="400">
<img src="/README_images/pulse-sensor.png" width="400">
<img src="/README_images/temp-sensor.png" width="400">

<strong>Name: EKG </strong><br>
<strong>Description: </strong>This task collects Electrocardiography (EKG) data over a given period of time and processes it and stored in the medical device. <br>
<strong>Exceptions:</strong> If an external source interferes with the device, this task could possibly not trigger or function incorrectly by reading corrupted data.<br>

<strong>Name: Compute Data</strong><br>
<strong>Description:</strong> Gets raw information from the Measure Data task and converts/formats it for purposes of displaying the information and sending the data to the remote site if needed.<br>
<strong>Exception: </strong> Because this task relies on the measure data task, this event will not trigger without the the measure task event not triggering.<br>

<strong>Name: Display</strong><br>
<strong>Description:</strong> After the patient chooses a mode from Mode Select, It will display information of their current health status on a LED screen.<br>
<strong>Exception:</strong> The data could display garbage or no data if the measurement data gets corrupted from either the initial readings from the measurement task or at the EKG task. Data can also be corrupted if it is converted incorrectly from the compute task. <br>
<img src="/README_images/mode-1.png" width="400">
<img src="/README_images/mode-2.png" width="400">

<strong>Name: Warnings</strong> <br>
<strong>Description:</strong> Triggers an alarm if: Temperature is outside of normal range (36.1 C to 37.8 C), Systolic Pressure is outside of normal range(120 mmHg to 130 mmHg), Diastolic Pressure is outside of normal range (70 mmHg to 80 mmHg), pulse rate is outside of normal range (60 BPM to 100 BPM), and or Battery is under 20% remaining charge.<br>
<strong>Exception: </strong>Given that this task triggers based on the readings from the measure task, this task could trigger at inappropriate times or not trigger at all depending if the measure task has functioned correctly.<br>
<img src="/README_images/annunciation.png" width="400">


<strong>Name: Serial Communication</strong><br>
<strong>Description:</strong>Will send all corrected data and current battery status to a remote location <br>
<strong>Exception: </strong>Only Triggers if alarm is triggered. Otherwise, this task will not execute, therefore no data will be sent. If data was incorrectly converted at the compute task or read incorrectly at the EKG task, the data being sent could show incorrect information about the patient.<br>
<img src="/README_images/serial-comm.png" width="400">

<strong>Name: Remote Network Communication</strong><br>
<strong>Description: </strong>Measurement data is sent to a remote website after the raw data has been processed from the compute task. This allows the caregiver or a doctor to view the current status of the patient. The caregiver or doctor can also send commands to the medical device and request the immediate status of the patient or even command the medical device to stop or start collecting data from the patient. <br>
<strong>Exceptions:</strong> Like other events or task, this task is dependant on if the data received has been uncorrupted or not. So for this task to function correctly, it relies on if the measure and compute task functioned correctly in order to send back correct updates of the patient’s status to the remote website. <br>

<strong>Name: Command Handling</strong><br>
<strong>Description:</strong>This event handles managing commands that are sent from the remote website.  These commands are checked if they are valid or not. If the command is valid, it will set the medical device system to the appropriate state depending on the command sent out.<br>
<strong>Exceptions:</strong>In terms of this task functioning correctly and not triggering any actions to the system, if the command that was sent from the remote site was not valid, this task will then not send a command to the medical device system. If the remote site is not operational, this task will not trigger because there are no commands being sent out for this task to manage. <br>

<hr>

<h3>Architecture Diagram</h3>
<img src="/README_images/sequence.png" width="400">
<img src="/README_images/stateflow-1.png" width="400">
<img src="/README_images/stateflow-2.png" width="400">
<img src="/README_images/dataflow.png" width="400">
<img src="/README_images/activity.png" width="400">
