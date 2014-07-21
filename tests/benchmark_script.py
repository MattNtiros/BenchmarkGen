#!/usr/bin/env python
#
#	Script to use Benchmark.py class
#	
#
#


from ossie.utils import sb
import Benchmark


## Component to test
upzero_comp = sb.launch('UpZero')
tunefilter_comp = sb.launch('TuneFilterDecimate')
fastfilter_comp = sb.launch('fastfilter')



# Benchmark Parameters
test_comp = upzero_comp  #this is the component you want to test.
t = 5				#time for benchmark to recalcuate confidence interval in seconds
packets = 100		#num of packets in BenchmarkGen component to calculate output rate
size = 1000			#size of packets for BenchmarkGen component to generate
samples_away = 10	#samples away from the first  
plotFlag = 0		#plot results? 
debugFlag = 1		#show debug information



# Benchmark start
bench1 = Benchmark.Benchmark(test_comp, t, packets, size, samples_away, plotFlag, debugFlag)
bench1.run()

print 'script done'
