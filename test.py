import numpy as np
import scipy.stats as sps

r = 0.02
T = 1
sigma = 0.3
S0 = 100
K = 90

d1 = (np.log(S0/K) + (r + sigma*sigma/2)*T)/sigma/np.sqrt(T)
d2 = d1 - sigma*np.sqrt(T)

C = sps.norm.cdf(d1)*S0 - sps.norm.cdf(d2)*K*np.exp(-r*T)

print(C)