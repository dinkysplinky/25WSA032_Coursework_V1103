TASK FOUR DISCUSSION (also present in python file)
# Discussion of Findings
#
# Time-domain behaviour:
# The recorded temperature remained essentially stable at approximately
# 18.22 C throughout the 3-minute measurement period, with no sudden
# rises or drops corresponding to a real thermal event. The signal does,
# however, appear noisy: all readings fall into just three discrete
# values (18.14, 18.22, and 18.30 C), which reflects the quantisation
# step of the ADC rather than genuine temperature variation. The
# moving-average overlay closely tracks the raw signal, confirming that
# the underlying temperature is effectively stationary and that the
# apparent fluctuations are noise rather than physical change.
#
# Frequency-domain behaviour:
# The DFT is dominated by the DC component, consistent with a
# near-constant signal. A minor peak appears around 1.3 Hz, but its
# magnitude is comparable to the surrounding noise floor and it is not
# accompanied by harmonics, so it cannot be attributed to a genuine
# periodic fluctuation. The remainder of the spectrum is broadly flat,
# indicating that higher-frequency content is essentially white noise
# introduced by the ADC and sensor rather than any structured
# oscillation in the measurand.
#
# System behaviour:
# The sampling strategy operated as intended and produced a complete
# 180-sample record at the expected interval. Because the measured
# temperature was effectively constant, the data does not strongly
# exercise an adaptive sampling or power-mode-switching scheme, and the
# system spent the recording in what would effectively be a low-activity
# state. A useful improvement would be to validate the adaptive
# behaviour against a deliberately varying input, for example by
# introducing a controlled heat source so that transitions between
# sampling rates or power modes can be observed.
#
# Data quality:
# The 3-minute recording duration and 1 Hz sampling rate were
# appropriate for monitoring slowly varying ambient temperature, and the
# captured dataset is complete and well-formed. The main limitation is
# the resolution of the measurement chain: the ADC quantisation step
# (~0.08 C) is large relative to the natural variation in the room, so
# genuine fluctuations smaller than one step are invisible. A more
# informative experiment would either increase the effective resolution
# (e.g. through oversampling and averaging) or expose the sensor to a
# wider temperature range so that real signal dominates over
# quantisation noise.