import numpy as np
import matplotlib.pyplot as plt
from scipy.special import spence as dilog
import scipy.integrate as integrate
from scipy.optimize import curve_fit

def tanhAD2(x):
    return 0.5 * (x * (x + 2 * np.log(np.exp(-2 * x) + 1)) - dilog(1 + np.exp(-2 * x)))

def tanhAD2_int(x):
    y = np.copy(x)
    for n in range(len(y)):
        y[n] = integrate.quad(lambda x : np.log(np.cosh(x)), 0, y[n])[0]
    return y

def dilog_approx(x):
    return -np.log(-x + 1)

def tanhAD2_approx(x):
    return 0.5 * (x * (x + 2 * np.log(np.exp(-2 * x) + 1)) - dilog_approx(-np.exp(-2 * x)))

def exp_approx(x, a, b, c, d):
    y = np.copy(x)
    for n in range(len(y)):
        if y[n] >= 0:
            y[n] = c * (1 - d**(a*b*y[n])) / (1 - d**a)
        else:
            y[n] = -c * (1 - d**(-a*b*y[n])) / (1 - d**a)
    return y

LIM = 12
x = np.linspace(-LIM, LIM, num=100)
x2 = np.copy(x)
x3 = np.copy(x)

y = tanhAD2(x)
y2 = tanhAD2_int(x2)
y3 = tanhAD2_approx(x3)

# popt, pcov = curve_fit(exp_approx, x2, y2, bounds=([0, 0, 0, 1], [10, 1, 50, 10]))
# y3 = exp_approx(x3, *popt)
# print(popt)

# plt.plot(x, y)
# plt.plot(x2, y2)
plt.plot(x3, y3)
plt.grid()
plt.show()
