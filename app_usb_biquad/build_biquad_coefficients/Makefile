all: 
	cd build_biquad_coefficients;	java MakeCoeffs $(FILTER) -h $(INCLUDEFILE) -xc $(XCFILE) -csv $(CSVFILE)

MakeCoeffs.class: ../../sc_dsp_filters/build_biquad_coefficients/src/MakeCoeffs.java
	javac  ../../sc_dsp_filters/build_biquad_coefficients/src/MakeCoeffs.java
	mv ../../sc_dsp_filters/build_biquad_coefficients/src/MakeCoeffs*.class .
