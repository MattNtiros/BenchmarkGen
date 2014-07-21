#!/usr/bin/env python
######################################################
#		Benchmark Utility for Redhawk Components
# ---------------------------------------------------
# Script to test a redhawk component using the 
# BenchmarkGen component in redhawk.
# 
# Will calcuate 95% confidence interval based on sample
# mean and sample variance. Will keep running until that
# 95% confidence interval is within +-1% away from the 
# calcuated mean
#
#
# Input Parameters:
# 	testComponent 	- this is the component you want to benchmark.
# 					ex. testComponent = sb.launch('component_name')
#
#	time			- time for benchmark to run to calculate confidence
#
#	packets			- the number of packets benchmark should push out 
#					before calculating the average time
#					default value: 100
#
#	packetSize		- size of the packets pushed out by BenchmarkGen
#					default value: 1000
#
#	firstSample		- Samples to skip from beginning of benchmark
#					Change to a higher value if results are higher than
#					expected in the beginning
#					default value: 10
#
#	showPlot		- boolean to show the result in a plot or not.
#					default value: 0 (off)
#
#	debug			- shows debug statistics
#					default value: 0 (off)
#
#
# Author: Yadu Raghu
# 07/02/2014

from ossie.utils import sb
from time import sleep
from multiprocessing import Process
from optparse import OptionParser
import sys
import matplotlib.pyplot as plt
import numpy

class Benchmark:
	def __init__(self, testComponent, time=10, packets=100, packetSize=1000, firstSample=10, showPlot=0, debug=0):
		# initilizing the benchmark generator component
		self.bc = sb.launch('BenchmarkGen')
		self.bc.packets_per_time_avg = packets 
		self.bc.packet_size = packetSize
	
		# test component loaded up
		self.bc.connect(testComponent)
		
		# class variables
		self.bmTime = time
		self.showPlot = showPlot
		self.debug = debug
		self.samplesAway = firstSample
		self.samples = []
		self.outputRates = []
		self.times = []

	def run(self):
		print 'starting benchmark'
		sb.start()
		
		#first sample recorded should be a couple away from the start of BenchmarkGen
		#due to the high output rate to fill up the queue
		lastTP = self.bc.packets_per_time_avg*self.samplesAway
		
		#loop counter
		loopCount = 0
		#run until less than 1%
		while 1:
			timer = 0
			#run until time for benchmark runs out
			while timer <= self.bmTime: 
				#run until next value can be collected
				while self.bc.totalPackets <= lastTP: 
					sleep(.1)
					timer = timer + .1;
				#collect data from benchmark component
				self.samples.append(float(timer + loopCount*self.bmTime))
				self.outputRates.append(float(self.bc.avg_output_rate))
				self.times.append(float(self.bc.time))
				lastTP = lastTP + self.bc.packets_per_time_avg
			#calculating statistics
			N = len(self.samples)
			s = numpy.std(self.outputRates)
			x = numpy.mean(self.outputRates)
			value = 1.96*(s/numpy.sqrt(N))
			lower = x - value
			upper = x + value
			percent = (value / x) * 100
			
			#debug
			if self.debug==1:
				print 'N is: ' + str(int(N))
				print 's is: ' + str(float(s))
				print 'x is: ' + str(float(x))
				print 'value is: ' + str(float(value))
				print '95% confidence level: ' + str(float(lower)) + ' - ' + str(float(upper))
				print 'percent away is: ' + str(float(percent))
			loopCount = loopCount + 1
			if(percent <= 1):
				break;
		
		self.printStats()
		if self.showPlot==1:
			self.plot()
		
		sb.reset()
		sb.stop()

	# plots the final result
	def plot(self):
		plt.subplot(2, 1, 1)
		plt.plot(self.samples, self.outputRates, 'k.-')
		plt.plot(self.samples, [numpy.mean(self.outputRates)]*len(self.samples), 'r-')
		plt.title('Output Rate and Time (' + str(len(self.samples)) + ' samples taken)')
		plt.ylabel('Output Rate')

		plt.subplot(2, 1, 2)
		plt.plot(self.samples, self.times, 'k.-')
		plt.plot(self.samples, [numpy.mean(self.times)]*len(self.samples), 'r-')
		plt.xlabel('Time for Benchmark')
		plt.ylabel('Time (sec)')
		print 'close plot to continue...'
		plt.show()
		

	# prints the final average and standard deviation
	def printStats(self):
		print '---------------------- STAT --------------------------'
		print 'Average output rate: ' + str(numpy.mean(self.outputRates))
		print 'Standard Deviation: ' + str(numpy.std(self.outputRates))
		print 'Max output rate: ' + str(numpy.max(self.outputRates))
		print 'Min output rate: ' + str(numpy.min(self.outputRates))
		print 'Average time it took for ' + str(int(self.bc.packets_per_time_avg)) + ' packets to push: ' + str(numpy.mean(self.times))
		print '------------------------------------------------------'
		print ''
		



#Can be run alone, give it a component to test. 
#Will use default component values
#	Usage:
#	./Benchmark [options] componentName
#	will use default benchmark parameters	
#	
#		
def main(argv):
	usage="usage: %prog [options] componentName"
	parser = OptionParser(usage, description="Benchmark a specific redhawk component")
	parser.add_option("-t", "--time", action="store", type="int", dest="time", default=10,
			help="Time for benchmark to calculate confidence", metavar="NUM")
	parser.add_option("-p", "--packets", action="store", type="int", dest="packets", default=100,
			help="# of packets BenchmarkGen pushes out to calculate output rate", metavar="NUM")
	parser.add_option("-s", "--size", action="store", type="int", dest="packetSize", default=1000,
			help="size of the packets pushed out by BenchmarkGen component in redhawk", metavar="NUM")
	parser.add_option("--shift", action="store", type="int", dest="firstSample", default=10,
			help="skips NUM amount of samples before benchmark starts recording values", metavar="NUM")
	parser.add_option("-g", "--showPlot", action="store_true", dest="plotFlag", default=False,
			help="Shows plot at the end of benchmark")
	parser.add_option("--debug", action="store_true", dest="debugFlag", default=False,
			help="Set debug flag on")
	(options, args) = parser.parse_args();
	if len(args) < 1:
		parser.print_help()
		exit()
	try:
		testComponent = sb.launch(args[0])
	except ValueError:
		print 'Component cannot be found.'
		print '	Make sure the component is within your SDR root or'
		print '	give the full path to the xml file of the component'
	except:
		print 'Unknown error: Program exiting'
		exit()
	else:
		print "Testing component: " + args[0]
		bm = Benchmark(testComponent, options.time, options.packets, options.packetSize, options.firstSample, options.plotFlag, options.debugFlag)
		bm.run()





if __name__ == '__main__':
	main(sys.argv)
