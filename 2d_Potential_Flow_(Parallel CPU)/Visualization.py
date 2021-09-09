import numpy as np
import matplotlib.pyplot as plt
import scipy.interpolate as intpol
import os

# Plotting Vertexes
vertex_x = []
vertex_y = []
fp = open('./data/geometry_set/vertex.txt', 'r')
while True:
    line = fp.readline()
    if not line : break
    line = line.split(' ')
    vertex_x.append(float(line[0]))
    vertex_y.append(float(line[1]))
fp.close()
plt.scatter(vertex_x, vertex_y)

# Plotting Triangle
tri_vertex1_x = []
tri_vertex1_y = []
tri_vertex2_x = []
tri_vertex2_y = []
tri_vertex3_x = []
tri_vertex3_y = []
fp = open('./data/geometry_set/triangle.txt', 'r')
while True:
    line = fp.readline()
    if not line : break
    line = line.split(' ')
    tri_vertex1_x.append(float(line[0]))
    tri_vertex1_y.append(float(line[1]))
    tri_vertex2_x.append(float(line[2]))
    tri_vertex2_y.append(float(line[3]))
    tri_vertex3_x.append(float(line[4]))
    tri_vertex3_y.append(float(line[5]))
fp.close()


for i in range(len(tri_vertex1_x)):
    plt.plot([tri_vertex1_x[i], tri_vertex2_x[i]],
             [tri_vertex1_y[i], tri_vertex2_y[i]], color='green')
    plt.plot([tri_vertex2_x[i], tri_vertex3_x[i]],
             [tri_vertex2_y[i], tri_vertex3_y[i]], color='green')
    plt.plot([tri_vertex3_x[i], tri_vertex1_x[i]],
             [tri_vertex3_y[i], tri_vertex1_y[i]], color='green')

# Plotting Result
result_x = []
result_y = []
result = []
fp = open('./data/result/result.txt', 'r')
while True:
    line = fp.readline()
    if not line : break
    line = line.split(' ')
    result_x.append(float(line[0]))
    result_y.append(float(line[1]))
    result.append(float(line[2]))
fp.close()

# Updating domain data
fp = open('./data/domain_set/domain.txt', 'r')
line = fp.readline()
line = line.split(' ')
width = (float(line[0]))
height = (float(line[1]))

npts = 500
# define grid.
xi = np.linspace(0 - 0.1, width + 0.1, npts)
yi = np.linspace(0 - 0.1,height + 0.1, npts)
# grid the data.
zi = intpol.griddata((result_x, result_y), result, (xi[None, :], yi[:, None]), method='cubic')
# contour the gridded data, plotting dots at the randomly spaced data points.
plt.contour(xi, yi, zi, 50, linewidths=0.5, colors='k')
plt.contourf(xi, yi, zi, 50, cmap=plt.cm.jet)
plt.show()