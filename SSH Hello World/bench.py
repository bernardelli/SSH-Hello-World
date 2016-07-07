import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import UnivariateSpline
import glob, os
def do_plot(file, label, color):
  raw_data = np.genfromtxt(file, dtype=float, skip_header =2);

  raw_x = raw_data[:,1];
  raw_y = raw_data[:,0];

  x = np.unique(raw_x);
  y = np.zeros( x.size);
  y_err = np.zeros(x.size);
  for i in range(x.size):
      y[i] = np.average(raw_y[raw_x==x[i]])
      y_err[i] = 100*np.std(raw_y[raw_x==x[i]])/np.sqrt(np.sum(raw_x==x[i]));


  spl = UnivariateSpline(x, y)
  x_spl = np.linspace(np.min(x), np.max(x), 100)

  plt.plot(x_spl, spl(x_spl), color='b')
  plt.hold(True);
  plt.errorbar(x, y, yerr=y_err, fmt='o', color='b', label='512 img')
  plt.hold(True);
  plt.title('bla')
  plt.xlabel('Kernel_xy size')
  plt.ylabel('Runtime (ms)')

plt.figure()
for file in glob.glob('image_size_*__kernel_eps_size_21.txt'):
  
  print(file)

  do_plot(file, "lol", 'b')
plt.show()

