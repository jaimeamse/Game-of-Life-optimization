x = 100000.0
y = 100000.0
i = 1000.0
NT = 6.0
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # necesario para 3D
import numpy as np

def CP( m, p, n):
  return ((m+2*n+14)*(p+2*n) +
          (m+2*(n-1)+14)*(p+2*(n-1)) +
          (m+2*(n-2)+14)*(p+2*(n-2)) +
          (m+14)*(p)
      )

def CP2( m, p, n):
  return (
          (m+2*(n-1)+14)*(p+2*(n-1)) +
          (m+2*(n-2)+14)*(p+2*(n-2))
      )

#L3, cuando
#L2_size < CP2 y
#CP2*NT < L3_size
def bytesTransferidosCaso1(m, p, n):
  totalBlocsTransferWrite = (((x+7)*y)/((m+14)*p))*(i/n)

  personalBlocksBytes = 0
  for it in range(1, n-1):
      personalBlocksBytes += 3*(m+2*it+14)*(p+2*it)

  return totalBlocsTransferWrite*(
      (m+2*n+14)*(p+2*n) +
      2*(m+14)*p +
      personalBlocksBytes
  )

#L3, cuando
#CP2 < L2_size < CP
def bytesTransferidosCaso2(m, p, n):
  totalBlocsTransferWrite = (((x+7)*y)/((m+14)*p))*(i/n)
  return totalBlocsTransferWrite*(
      (m+2*n+14)*(p+2*n) +
      2*(m+14)*p +
      (m+2*(n-1)+14)*(p+2*(n-1))*2 +
      (m+2*(n-2)+14)*(p+2*(n-2))*2
  )

#L3, cuando: CP < L2 size
def bytesTransferidosCaso3(m, p, n):
  totalBlocsTransferWrite = (((x+7)*y)/((m+14)*p))*(i/n)
  
  return totalBlocsTransferWrite*(
     2*(m+14)*p +
     2*(m+14+2*n)*(p+14+2*n) 
  )

#L1, cuando
#CP2 < L1_size < CP
def bytesTransferidosCaso4L1(m, p, n):
  totalBlocsTransferWrite = (((x+7)*y)/((m+14)*p))*(i/n)
  
  personalBlocksBytes = 0
  for it in range(1, n-1):
      personalBlocksBytes += 2*(m+2*it+14)*(p+2*it)
    
  return totalBlocsTransferWrite*(
      (m+2*n+14)*(p+2*n) +
      2*(m+14)*p +
      (m+2*(n-1)+14)*(p+2*(n-1))*2 +
      (m+2*(n-2)+14)*(p+2*(n-2))*2 +
      personalBlocksBytes
  )

#L1, cuando: CP < L1 size
def bytesTransferidosCaso5L1(m, p, n):
  totalBlocsTransferWrite = (((x+7)*y)/((m+14)*p))*(i/n)
  personalBlocksBytes = 0
  for it in range(1, n-1):
      personalBlocksBytes += 2*(m+2*it+14)*(p+2*it)

  return totalBlocsTransferWrite*(
      (m+2*n+14)*(p+2*n) +
      2*(m+14)*p +
      personalBlocksBytes
  )

def getBytesL1(m, p, n):
  CP_val = CP(m, p, n)
  CP2_val = CP2(m, p, n)
  L1_size = 49152.0 
  if CP_val < L1_size:
      return bytesTransferidosCaso5L1(m, p, n)
  elif CP2_val < L1_size < CP_val:
      return bytesTransferidosCaso4L1(m, p, n)
  else:
      return bytesTransferidosCaso1(m, p, n)

def getBytesL2(m, p, n):
  CP_val = CP(m, p, n)
  CP2_val = CP2(m, p, n)
  L1_size = 49152.0
  if CP_val < L1_size:
      return bytesTransferidosCaso3(m, p, n)
  elif CP2_val < L1_size < CP_val:
      return bytesTransferidosCaso2(m, p, n)
  else:
      return bytesTransferidosCaso1(m, p, n)
    
def getBytesL3(m, p, n):
  CP_val = CP(m, p, n)
  CP2_val = CP2(m, p, n)
  L2_size = 1310720.0
  if CP_val < L2_size:
      return bytesTransferidosCaso3(m, p, n)
  elif CP2_val < L2_size < CP_val:
      return bytesTransferidosCaso2(m, p, n)
  else:
      return bytesTransferidosCaso1(m, p, n)

def getBytesRAM(m, p, n):
  CP_val = CP(m, p, n)
  L3_size = 18874368.0
  if CP_val*NT < L3_size:
      return bytesTransferidosCaso3(m, p, n)
  else:
      return bytesTransferidosCaso2(m, p, n)
  
def getTimeL1(m, p, n):
  bytes = getBytesL1(m, p, n)
  bandwidth = 660.0*(2**30) #bytes/segundo
  return bytes/bandwidth

def getTimeL2(m, p, n):
  bytes = getBytesL2(m, p, n)
  bandwidth = 348.0*(2**30) #bytes/segundo
  return bytes/bandwidth
  
def getTimeL3(m, p, n):
  bytes = getBytesL3(m, p, n)
  bandwidth = 194.0*(2**30) #bytes/segundo
  return bytes/bandwidth
  
def getTimeRAM(m, p, n):
  bytes = getBytesRAM(m, p, n)
  bandwidth = 30.0*(2**30) #bytes/segundo
  return bytes/bandwidth


def computeBound(m, p, n):
   totalBlocsTransferWrite = ((x*y)/(m*p))*(i/(n*32.0))


   sumAcc = 0.0
   for it in range(0, n-1):
      sumAcc += (m+2*it)*(p+2*it)


   return (totalBlocsTransferWrite * sumAcc * (4.67/6))/((4.4*10**9))

def getArrayTimes(m, p, n):
  return [
          getTimeL1(m, p, n),
          getTimeL2(m, p, n),
          getTimeL3(m, p, n),
          getTimeRAM(m, p, n),
          computeBound(m, p, n)
      ]

def getTotalTime(m, p, n):
  return max (getArrayTimes(m, p, n))

x_vals = []
y_vals = []
z_vals = []

for p in range(1, 1000):
    for n in range(3, 1000):
        for m in range(1, 1000):
            CP2_val = CP2(m, p, n)
            L3_size = 18874368.0
            if CP2_val*NT > L3_size:
                print("Nos hemos pasado de la cache L3 :(, la solucion no es valida")
                print(p)
                print(n)
                print(m)
                break
            time = getTotalTime(m, p, n)
            if time < 100.0:
                x_vals.append(m)
                z_vals.append(n)
                y_vals.append(time)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.scatter(x_vals, z_vals, y_vals)

ax.set_xlabel("m")
ax.set_ylabel("n")
ax.set_zlabel("Tiempo total")

posMinTime = np.argmin(y_vals)
print("m del minimo:" + str(x_vals[posMinTime]))
print("n del minimo:" + str(z_vals[posMinTime]))
print("Tiempo minimo:" + str(y_vals[posMinTime]))
m = x_vals[posMinTime]
n = z_vals[posMinTime]
p = m
print("Tiempos individuales para cada nivel de memoria:")
arrayTimesMin = getArrayTimes(m, p, n)
print(arrayTimesMin[0:-1])
print("Tiempo por computo teorico:")
print(arrayTimesMin[-1])
plt.show()
