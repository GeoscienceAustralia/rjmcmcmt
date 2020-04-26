/*
This source code file is licensed under the GNU GPL Version 2.0 Licence by the following copyright holder:
Crown Copyright Commonwealth of Australia (Geoscience Australia) 2015.
The GNU GPL 2.0 licence is available at: http://www.gnu.org/licenses/gpl-2.0.html. If you require a paper copy of the GNU GPL 2.0 Licence, please write to Free Software Foundation, Inc. 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

Author: Ross C. Brodie, Geoscience Australia.
*/

#include <iostream>
#include <cstdio>
#include <vector>
#include <limits>
#include <cmath>
#include <fstream>

#include <vector>
#include "general_utils.h"
#include "logger.h"
#include "mt.h"
#include "resistivity_model.h"

class cLogger glog; //The global instance of the log file manager

int generate_synthetic_data(int argc, const char** argv)
{
	
	//model_1.txt model_1.edi 54 1.040001e+04 4.300000e-01 0.05 0.1 0.02 1

	std::string modelfile = std::string(argv[1]);
	std::string edifile = std::string(argv[2]);
	size_t nfrequencies = (size_t) atoi(argv[3]);
	double flow = atof(argv[4]);
	double fhigh = atof(argv[5]);
	double appres_relative_err = atof(argv[6]);;
	double appres_floor_err = atof(argv[7]);;
	double appphase_relative_err = atof(argv[8]);;
	double appphase_floor_err = atof(argv[9]);;

	ResistivityModel m(modelfile);
	m.print();
	std::vector<double> frequency = log10space(flow, fhigh, nfrequencies);
	std::vector<double> app_res;
	std::vector<double> app_phase;	
	m.generate_synthetic_data_app_res_phase(frequency, app_res, app_phase, appres_relative_err, appres_floor_err, appphase_relative_err, appphase_floor_err);	
	cEDIFile E(frequency, app_res, app_phase);
	std::string s = "// Synthetic model\n" + m.string("// ");
	E.write(edifile,s);
	return 0;
};


int main(int argc, const char** argv)
{		
	_GSTITEM_
	if (argc != 10){
		printf("Usage: %s modelfile edifile nfrequencies frequency_low frequency_high appres_relative_err appres_floor_err appphase_relative_err appphase_floor_err\n", argv[0]);
		printf("e.g.\n");
		printf("%s model_1.txt model_1.edi 54 1 10000 0.05 0.1 0.02 1\n",argv[0]);
		return 1;
	}

	try
	{		
		_GSTITEM_
		generate_synthetic_data(argc, argv);
		return 0;
	}		
	catch (std::string& e)
	{
		_GSTPRINT_
		std::cout << e << std::endl;
		return 1;
	}
	catch (std::exception& e)
	{
		_GSTPRINT_
		std::cout << e.what() << std::endl;
		return 1;
	}
	std::printf("Success\n");
	return 0;

};
