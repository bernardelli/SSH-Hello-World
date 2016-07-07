import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import UnivariateSpline
raw_data = np.fromfile('image_size  512, kernel_eps_size  21.txt', dtype=float, count=-1, sep=' ');

raw_x = raw_data[1::2];
raw_y = raw_data[::2];

x = np.unique(raw_x);
y = np.zeros( x.size);
y_err = np.zeros(x.size);
for i in range(x.size):
    y[i] = np.average(raw_y[raw_x==x[i]])
    y_err[i] = 100*np.std(raw_y[raw_x==x[i]])/np.sqrt(np.sum(raw_x==x[i]));


print(y_err)
spl = UnivariateSpline(x, y)
x_spl = np.linspace(np.min(x), np.max(x), 100)
plt.figure()
plt.plot(x_spl, spl(x_spl), color='b')
plt.hold(True);
plt.errorbar(x, y, yerr=y_err, fmt='o', color='b', label='512 img')

plt.title('bla')
plt.xlabel('Kernel_xy size')
plt.ylabel('Runtime (ms)')
plt.show()
#y = np.mean(raw_y[])
