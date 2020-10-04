import numpy as np
import scipy.signal as signal
import matplotlib.pyplot as plt
import audio_dspy as adsp
from scipy.special import spence as dilog

def plot_fft(x, fs, sm=1.0/24.0):
    fft = 20 * np.log10(np.abs(np.fft.rfft(x) + 1.0e-9))
    freqs = np.fft.rfftfreq(len(x), 1.0 / fs)
    return freqs, fft

def process_nonlin(fc, FS, nonlin, gain=10):
    N = int(FS*0.75)
    sin = np.sin(2 * np.pi * fc / FS * np.arange(N))
    y = nonlin(gain * sin)
    freqs, fft = plot_fft(y, FS)
    # return np.arange(N) / FS, y
    return freqs, fft

FC = 1244.5
FS = 44100

freqs, fft = process_nonlin(FC, FS, np.tanh)
plt.plot(freqs, fft)

# freqs, fft = process_nonlin(FC, 8*FS, np.tanh)
# plt.plot(freqs, fft)

def adaaTanh(x_):
    x = np.copy(x_)
    x1 = 0.0
    for n in range(len(x)):
        if np.abs(x[n] - x1) < 1.0e-5:
            y = np.tanh((x[n] + x1) / 2)
        else:
            y = (np.log(np.cosh(x[n])) - np.log(np.cosh(x1))) / (x[n] - x1)
        x1 = x[n]
        x[n] = y

    return x

freqs, fft = process_nonlin(FC, 2*FS, adaaTanh)
plt.plot(freqs, fft, '--')

def adaaTanh2(x_):
    x = np.copy(x_)
    x1 = 0.0
    x2 = 0.0

    F0 = np.tanh
    F1 = lambda x : np.log(np.cosh(x))
    F2 = lambda x : 0.5 * (x * (x + 2 * np.log(np.exp(-2 * x) + 1)) - dilog(1 + np.exp(-2 * x)))

    def calcD(x0, x1):
        if np.abs(x0 - x1) < 1.0e-10:
            return F1(0.5 * (x0 + x1))
        else:
            return (F2(x0) - F2(x1)) / (x0 - x1)

    for n in range(len(x)):
        if np.abs(x[n] - x2) < 1.0e-10:
            x_bar = 0.5 * (x[n] + x2)
            delta = x_bar - x[n]

            if np.abs(delta) < 1.0e-10:
                y = F0(0.5 * (x_bar + x[n]))
            else:
                y = (2.0 / delta) * (F1(x_bar) + (F2(x[n]) - F2(x_bar)) / delta)

        else:
            D1 = calcD(x[n], x1)
            D2 = calcD(x1, x2)
            y = (2.0 / (x[n] - x2)) * (D1 - D2)

        x2 = x1
        x1 = x[n]
        x[n] = y

    return x

freqs, fft = process_nonlin(FC, 2*FS, adaaTanh2)
plt.plot(freqs, fft, '--')

# plt.xlim(0, 0.005)
plt.xlim(50, 22000)
plt.show()
