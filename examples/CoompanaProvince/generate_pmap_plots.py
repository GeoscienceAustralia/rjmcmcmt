"""
Script to generate plots from the rjmcmcmt outputs
"""
import os;
import PlotResults;

resultsdir = 'output';
colormap='gray_r';
maxDepth=500;
plotSizeInches='11x8';

for s in os.listdir(resultsdir):
    stationpath = os.path.join(resultsdir, s);
    stationname = os.path.basename(stationpath);
    pdffile     = stationpath + os.sep + stationname + '.pdf';
    print('Creating ' + pdffile);
    r = PlotResults.Results(stationpath,pdffile,plotSizeInches,maxDepth,colormap);
    r.plot();
    #break;
# end
