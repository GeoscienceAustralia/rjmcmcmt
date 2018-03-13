# -*- coding: utf-8 -*-
"""
script to visualise rjmcmcmt output

"""
import os
import PlotMcmcResults

resultsdir = r'C:\Git\rjmcmcmt\examples\CoompanaProvince\output'

for path in os.listdir(resultsdir):
    
    
    r = PlotMcmcResults.Results(os.path.join(resultsdir, path), 
                         os.path.join(resultsdir, 
                                      path + '/%s.pdf'%(os.path.basename(path))), 
                         plotSizeInches='11x8', 
                         maxDepth=500,
                         colormap='jet')
#                          colormap='OrRd')
#                         colormap='gray_r'
#                         colormap='rainbow')
#                         
#      r = Results(path, output_file_name, plotSizeInches=plot_size_inches,
#                maxDepth=max_depth, colormap=colormap)
#    r.plot()

#    return                   
    r.plot()