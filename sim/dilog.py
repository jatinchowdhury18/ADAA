import numpy as np
from scipy.special import spence as dilog
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

def dilog_approx(x, a):
    return -1 * np.log(np.where(np.abs(np.power(x,a)) < 1.0e-15, 1.0e-15, np.power(x,a)))

x = np.linspace(-10, 1, num=200)
x2 = np.copy(x)

y = dilog(1 - x)
y2 = dilog_approx(1 - x2, 1.4)

plt.plot(x, y)
plt.plot(x2, y2, '--')
plt.grid()
plt.show()
