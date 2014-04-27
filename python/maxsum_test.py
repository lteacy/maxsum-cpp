import maxsum as ms

try:
   # register some variables
   for v in range(30):
      ms.registerVariable(v,4)

   # create a maxsum controller and use it to construct a factor graph
   controller = ms.MaxSumController()
   
   # factor 1
   domain = [1,2]
   data = range(4*4)
   controller.setFactor(1,domain,data)
   
   # factor 2
   domain = [2,3]
   data = range(4*4)
   controller.setFactor(2,domain,data)
   
   # optimise and get result
   x = controller.optimise()
   print 'number of max-sum iterations performed: {0}'.format(x)
   print 'result:'
   print controller.getValues()

except ms.MaxSumException as e:
   print "Oops! a maxsum error occurred: {0}".format(e.what())

