from pylab import *

file = open('sample_test_timespan7.txt');
data = []
i = 0;
while i < 6:
    j = 0
    line = file.readline()
    tmpData = []
    line = file.readline()
    line = line.strip().split()
    first = float(line[3])
    while j < 29:
        line = file.readline()
        line = line.strip().split()
        if j < 10:
            tmpData.append(float(line[3])/first)
        j += 1
    data.append(tmpData)
    i += 1

i = 0
x = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11]
base = [6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5, 6.5]
xlabel('#Active neighors')
ylabel('T=1/T=0')
title('Result of Randomized test')
while i < 6:
    plot(x, data[i])
    i += 1
plot(x, base)
savefig('test.png')
show()
# plot(trainResult + testResult, linewidth=1.0)
# plot(annTrainOutput + annTestOutput, linewidth=1.0)
# #xlabel('time (s)')
# #ylabel('voltage (mV)')
# title('testError:'+str(testError/509)+'\n'+'trainError:'+str(trainError/509))
# #grid(True)
# #savefig("test.png")
# show()

# t = arange(0.0, 2.0, 0.01)
# s = sin(2*pi*t)
# plot(s, linewidth=1.0)

# xlabel('time (s)')
# ylabel('voltage (mV)')
# title('About as simple as it gets, folks')
# #grid(True)
# savefig("test.png")
# show()
