import numpy as np


def getOD(a_ldr, a_laser):
    # resistance in series with LDR - in Ohm
    r_series = 10000.0

    if isinstance(a_ldr,(list,np.ndarray)):
        a_ldr[a_ldr == 0] = 1
    elif a_ldr == 0:
        a_ldr = 1

    # OD conversion
    #   get resistance
    r_ldr = r_series * (1023 - a_ldr) / a_ldr
    r_laser = r_series * (1023 - a_laser) / a_laser

    #   get logIntensity
    # logR = k*logI + C
    k = -0.87
    C = 8
    logI = (np.log10(r_ldr) - C ) / k
    logI_laser = (np.log10(r_laser) - C ) / k

    #   get OD
    OD = logI_laser - logI

    return OD