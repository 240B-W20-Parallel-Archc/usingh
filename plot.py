import matplotlib.pyplot as plt
import csv
import matplotlib.pylab as pylab
from matplotlib.ticker import ScalarFormatter
import matplotlib.ticker as ticker
import numpy as np

params = {'legend.fontsize': 'xx-large',
		 'font.family' : 'sans-serif',
         'axes.labelsize': 'medium',
         'axes.titlesize':'xx-large',
         'xtick.labelsize':'medium',
         'ytick.labelsize':'medium'}
pylab.rcParams.update(params)
plt.rcParams["figure.figsize"] = (22,7)
#plt.rcParams.update({'font.size': 18, 'font.family': 'STIXGeneral', 'mathtext.fontset': 'stix'})
x = []
y = []

with open('./read_1_random.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    next(plots)
    for row in plots:
        x.append(int(row[0]))

cols = ['b', 'g', 'r', 'c', 'm', 'y', 'k', 'w']
mark = ['o', '^', 's', 'D', 'p', 'h']
tick = [64, 128, 256, 512, '1K', '2K', '4K', '8K', '16K', '32K', '64K', '128K', '256K', '512K', '1M', '2M', '4M', '8M', '16M', '32M', '64M']
stride_list = [0, 64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024]

def plot_stat(rw, stride, single):
	i = 0
	print(stride)
	if single == 'true':
		thread_list = [1]
	else:
		thread_list = [1, 2, 4, 8, 12, 16]

	for thread in thread_list:
		val = []
		file_name = rw+ "_" + str(thread) + "_" + stride + ".csv"
		fig_name = rw+ "_" + str(thread) + "_" + stride
		y.append([])
		print(file_name)
		with open(str(file_name),'r') as csvfile:
			plots = csv.reader(csvfile, delimiter=',')
			next(plots)
			for row in plots:
				val.append(float(row[1]))
		y[i].extend(val)
		label_name = str(thread) + " " + "Thread"
		#fig, ax = plt.subplots()
		print(stride)
		print(x)
		print(tick)
		#plt.xticks(x, tick, rotation=45)
		if stride == "random":
			plt.plot(x, y[i], label=label_name, marker=mark[i], c=cols[i], linewidth=3.0)
			
			plt.xscale('log')
			plt.xlabel("MEMORY SIZES (BYTES)", fontweight='bold')
		else:
			plt.plot(stride_list, y[i], label=label_name, marker=mark[i], c=cols[i], linewidth=3.0)
			plt.xticks(stride_list, rotation=45)
			plt.xlabel("STRIDE (BYTES)", fontweight='bold')

		#plt.style.use('seaborn-pastel')
		plt.yticks(list(range(0, 120, 20)))
		plt.legend(loc='upper left', frameon=True,framealpha=1,shadow=True, borderpad=1)
		#plt.xticks(x, tick, rotation=45)
		if stride =="random" and rw == "read":
			plt.title('READ LATENCY VS MEMORY SIZE', fontweight='bold')
			plt.ylabel("READ LATENCY (ns)", fontweight='bold')
		elif stride =="random" and rw == "rw":
			plt.title('READ-WRITE LATENCY VS MEMORY SIZE', fontweight='bold')
			plt.ylabel("READ-WRITE LATENCY (ns)", fontweight='bold')	
		elif stride == "stride" and rw == "read":
			plt.title('READ LATENCY VS STRIDE', fontweight='bold')
			plt.ylabel("READ LATENCY (ns)", fontweight='bold')
		elif stride == "stride" and rw == "rw":
			plt.title('READ-WRITE LATENCY VS STRIDE', fontweight='bold')
			plt.ylabel("READ-WRITE LATENCY (ns)", fontweight='bold')

		if single == 'true':
			plt.savefig(fig_name+".png")
		if thread == 16:
			plt.savefig(fig_name+".png")
		i+=1

plot_stat('read', 'stride', 'true')




