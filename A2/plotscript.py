import matplotlib as mpl
import numpy as np
import matplotlib.pyplot as plt

data = np.genfromtxt('outdegree.csv', delimiter=',', skip_header=1,
                     names=['degree', 'count'])

data2 = np.genfromtxt('indegree.csv', delimiter=',', skip_header=1,
                     names=['degree', 'count'])
print data['degree']
plt.title('LogLog Plot (Base 10) of In Degree')
plt.loglog(data2['degree'] ,data2['count'], 'b', basex=10, basey=10)
# plt.loglog(data['degree'] ,data['count'], 'r', basex=10, basey=10)
plt.ylabel('Count')
plt.xlabel('Out Degree')

plt.show()





print data['degree']
plt.title('In Degree')
plt.loglog(data['degree'] ,data['count'],'r', basex=10, basey=10)
plt.show()
