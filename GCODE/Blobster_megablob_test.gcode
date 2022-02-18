;*************************************************
;* BLOBSTER megablob test script for Prusa MK3S+ *
;*************************************************
;
;Avoid running it without appropriate calibrated setup of Blobster - you can hurt your printer or Blobster!
;
;Credits goes to https://www.prusa3d.com and https://www.bigbrain3d.com for the original scripts from which this one is derived

M73 P0 R710
M73 Q0 S712
M201 X1000 Y1000 Z200 E5000 ; sets maximum accelerations, mm/sec^2
M203 X200 Y200 Z12 E120 ; sets maximum feedrates, mm/sec
M204 P1250 R1250 T1250 ; sets acceleration (P, T) and retract acceleration (R), mm/sec^2
M205 X8.00 Y8.00 Z0.40 E4.50 ; sets the jerk limits, mm/sec
M205 S0 T0 ; sets the minimum extruding and travel feed rate, mm/sec
M107
;TYPE:Custom
M862.3 P "MK3S" ; printer model check
M862.1 P0.4 ; nozzle diameter check
G90 ; use absolute coordinates
M83 ; extruder relative mode
M104 S215 ; set extruder temp
M109 S215 ; wait for extruder temp
G28 W ; home all without mesh bed level
G1 X200 Z100 F12000
G4 S0

;BEGIN: COPY FROM HERE if you want to produce more blobs
G1 X254 F12000 ;Change this line during calibration 
G4 S1
M107
G92 E0
G1 E0.8 F2100
G92 E0
G1 E160 F350 ;Purge
M106 S255 ;Blob cooling
G4 S31 ;Blob cooling
M107 ;Turn off fan
G92 E0
G1 E-0.8 F2100
G1 X200 F12000
G4 S3; Wait before next blob
;END: COPY TO HERE if you want to produce more blobs

;PASTE here the appropriate amount of blocks marked BEGIN - END above for more megablobs in test 

M221 S100 ; reset flow
M900 K0 ; reset LA
M907 E538 ; reset extruder motor current
M104 S0 ; turn off temperature
M140 S0 ; turn off heatbed
M107 ; turn off fan
G1 Z94.4 ; Move print head up
G1 X0 Y200 F3000 ; home X axis
M84 ; disable motors
M73 P100 R0
M73 Q100 S0
