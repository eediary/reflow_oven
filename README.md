# reflow_oven
Code for EEDIARY reflow oven project (https://eediary.home.blog/2019/09/08/reflow-oven-intro/)

reflow_oven_code.ino: main file

oven_interp.cpp: contains linear interpolation code

oven_UI.cpp: contains state machine used for user interface

PMODTC1.cpp: contains code to get binary from thermocouple module, convert it to Celcius, filter data and get error flags
