import matplotlib as mpl
import numpy as np
import matplotlib.pyplot as plt
# python 2.7
data = np.genfromtxt('outdegree.csv', delimiter=',', skip_header=1,
                     names=['degree', 'count'])

data2 = np.genfromtxt('indegree.csv', delimiter=',', skip_header=1,
                     names=['degree', 'count'])
# print data['count']
plt.title('Log-Log Plot (Base 10) of Out Degree')
plt.loglog(data['degree'] ,data['count'], 'r', basex=10, basey=10)
plt.ylabel('Count')
plt.xlabel('Out Degree')

plt.show()



# print data['degree']
plt.title('Log-Log Plot (Base 10) of In Degree')
plt.loglog(data2['degree'] ,data2['count'],'b', basex=10, basey=10)
plt.show()

memsize = [209715200, 104857600, 52428800, 13107200, 6553600, 3276800]
#memsize = np.log(memsize)
runningtime = [46.40, 47.65, 42.91, 35.48, 41.51, 66.87]

plt.title('Total Memory Size vs Running Time')
plt.plot(memsize, runningtime)
plt.ylabel('Running Time (in Seconds)')
plt.xlabel('Log of Total Memory Size')
plt.show()
