# CAMAC Source Codes at B103 in YNU Engineering Lab
These source codes can operate CAMAC modules at B103 in YNU Engineering laboratory building.
We can operate CAMAC modules even in other places if changing a few parts of each code.  

## CAMAC Operation Codes
We have CAMAC modules written below.
This README tells us how to use and explains source codes in details.

### General process
- Change directory to ~/daq/src
- Use a source code with the same file name as module(s) type you use.
- Change station number and channel number list if you need. (These numbers are around the top of the source file)
- Enter "make (file name without an extension)" command in terminal.
- Object file and executable file are made then.
- Execute the executable file.
- Command to execute is "./(executable file name) (# of events) (data file name).dat"
- A data file is made in other directory ~/daq/data

### ADC: Analog Digital Convertor
#### 2249W(LeCroy)
- It is a general type of ADC modules
- You need to initialize data array to put data into
- You will get data value 1 if you forget this process

#### C009(HOSHIN)
- This module does not have F=26(Enable LAM) function

### TDC: Time Digital Convertor
#### C-TS105(Technoland)
- This module is NOT general type in my opinion
- This sends X value 0 when using F=8(LAM) function. However, that does not say it is breaking
- You need to make logic that stop signal(s) do NOT enter before start signal enters
- This module can measure multi hits' time
- You can choose single hit mode or multi hits mode in this source code
- Do NOT use channels of odd numbers
- You need to read instruction of this module or its source code in details if you change this source code

#### RPC-061(REPIC)
- This module might freeze for a long time
- This was made sure of correct work in Hayashi-REPIC on December, 2018

### Combine ADC with TDC
- Use a source file with two modules' type

### Other Modules
- Output Register (C004(HOSHIN))  
- Input Register (C005(HOSHIN))  
- Scaler (?)

## Test Codes
- Source codes for tests are in test-src directory
- inhibit_test: Make sure that the crate controller connects PC correctly
- lam_test: Make sure of modules' LAM

## Setting
Check these points when you use source codes in places except for B103.
- Check crate number
- Check station number
