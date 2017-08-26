/*
This source code file is licensed under the GNU GPL Version 2.0 Licence by the following copyright holder:
Crown Copyright Commonwealth of Australia (Geoscience Australia) 2015.
The GNU GPL 2.0 licence is available at: http://www.gnu.org/licenses/gpl-2.0.html. If you require a paper copy of the GNU GPL 2.0 Licence, please write to Free Software Foundation, Inc. 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

Author: Ross C. Brodie, Geoscience Australia.
*/

//Adapted from Matlab code written by Andrew Pethick 2013 - see bottom of file
//http://www.digitalearthlab.com/tutorial/tutorial-1d-mt-forward/

#ifndef _resistivity_medel_H
#define _resistivity_medel_H

#include <random>
#include "file_utils.h"
#include "vector_utils.h"

enum MTDataType { MT_DT_IMPEDANCE, MT_DT_APPRESPHASE };

class ResistivityModel {

public:

	std::vector<double> resistivity;
	std::vector<double> thickness;

	ResistivityModel(const std::string& modelfilename){
		read_model_file(modelfilename);
	}

	ResistivityModel(const std::vector<double>& depths, const std::vector<std::vector<double>>& props){

		size_t nl = depths.size() - 1;
		resistivity.resize(nl);
		thickness.resize(nl);

		for (size_t i = 0; i < nl; i++){
			resistivity[i] = props[i][0];
		}

		for (size_t i = 0; i < nl - 1; i++){
			thickness[i] = depths[i + 1] - depths[i];
		}
		thickness[nl - 1] = 9999999;
	}

	size_t nlayers(){ return resistivity.size(); }

	void model_impedance(
		const std::vector<double>& frequency,
		std::vector<double>& imp_i,
		std::vector<double>& imp_q
		)
	{
		std::vector<std::complex<double>> impedance = cMTModeller1D::model_impedance(resistivity, thickness, frequency);
		imp_i.resize(frequency.size());
		imp_q.resize(frequency.size());
		for (size_t fi = 0; fi < frequency.size(); fi++){
			imp_i[fi] = impedance[fi].real();
			imp_q[fi] = impedance[fi].imag();
		}		
	}

	std::vector<double> model_impedance_fieldunits_as_vector(const std::vector<double>& frequency)
	{
		std::vector<std::complex<double>> impedance = cMTModeller1D::model_impedance(resistivity, thickness, frequency);
		impedance *= MT_MODELLINGTOFIELDUNITS;
		std::vector<double> data(2 * frequency.size());
		for (size_t fi = 0; fi < frequency.size(); fi++){
			data[fi] = impedance[fi].real();
			data[fi  + frequency.size()] = impedance[fi].imag();
		}
		return data;
	}

	void model_appresphase(
		const std::vector<double>& frequency,
		std::vector<double>& app_res,
		std::vector<double>& app_phase
		)
	{
		std::vector<std::complex<double>> impedance = cMTModeller1D::model_impedance(resistivity, thickness, frequency);		
		cMTModeller1D::impedance_to_appresphase(frequency,impedance,app_res,app_phase);		
	}

	std::vector<double> model_appresphase_as_vector(const std::vector<double>& frequency)
	{
		std::vector<double> app_res;
		std::vector<double> app_phase;
		model_appresphase(frequency, app_res, app_phase);
		std::vector<double> data(2 * frequency.size());
		for (size_t fi = 0; fi < frequency.size(); fi++){
			data[fi] = app_res[fi];
			data[fi + frequency.size()] = app_phase[fi];
		}
		return data;
	}

	static void add_gaussian_noise(std::vector<double>& data, const double& relative_err, const double& floor_err){
		
		//auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::mt19937::result_type seed = (unsigned int) std::time(NULL);
		std::mt19937 generator(seed);
		//std::time();
		std::normal_distribution<double> distribution(0.0, 1.0);
		double n;
		for (size_t k = 0; k < data.size(); k++){
			n = distribution(generator);
			data[k] += n*relative_err*std::fabs(data[k]);
			n = distribution(generator);
			data[k] += n*floor_err;	
		}
	}

	void generate_synthetic_data_impedance(
		const std::vector<double> frequency,
		std::vector<double>& imp_i,
		std::vector<double>& imp_q,
		const double imp_i_relative_err,
		const double imp_i_floor_err,
		const double imp_q_relative_err,
		const double imp_q_floor_err		
		){
		
		model_impedance(frequency,imp_i,imp_q);
		add_gaussian_noise(imp_i, imp_i_relative_err, imp_i_floor_err);
		add_gaussian_noise(imp_q, imp_q_relative_err, imp_q_floor_err);		
	}

	void generate_synthetic_data_app_res_phase(
		const std::vector<double> frequency,		
		std::vector<double>& app_res,
		std::vector<double>& app_phase,		
		const double app_res_relative_err,
		const double app_res_floor_err,
		const double app_phase_relative_err,
		const double app_phase_floor_err
		){		
		model_appresphase(frequency, app_res, app_phase);		
		add_gaussian_noise(app_res, app_res_relative_err, app_res_floor_err);
		add_gaussian_noise(app_phase, app_phase_relative_err, app_phase_floor_err);
	}

	void read_model_file(const std::string& modelfilename){

		FILE* fp = fileopen(modelfilename.c_str(), "r");
		std::string s;
		filegetline(fp, s);
		while (filegetline(fp, s)){
			std::vector<std::string> t = tokenize(s);
			double thk = std::atof(t[0].c_str());
			double res = std::atof(t[1].c_str());
			resistivity.push_back(res);
			thickness.push_back(thk);
		}
		thickness.pop_back();
		fclose(fp);
	}

	std::string string(const std::string& lineprefix = std::string()){

		std::string s;
		double tsum = 0.0;
		s = strprint("%sLayer    From       To   Resistivity\n", lineprefix.c_str());
		//           |..xx...xxxx.x...xxxx.x...xxxxxxx.x
		for (size_t i = 0; i < resistivity.size() - 1; i++){
			s += strprint("%s  %2lu   %6.1lf   %6.1lf   %9.1lf\n", lineprefix.c_str(), i + 1, tsum, tsum + thickness[i], resistivity[i]);
			tsum += thickness[i];
		}
		size_t i = resistivity.size() - 1;
		s += strprint("%s  %2lu   %6.1lf      Inf   %9.1lf\n", lineprefix.c_str(), i + 1, tsum, resistivity[i]);
		return s;
	}

	void print(){
		printf(string().c_str());
	}

	void write(const std::string& filename){
		FILE* fp = fileopen(filename.c_str(), "w");
		fprintf(fp, "%s", string().c_str());
		fclose(fp);
	}

	static void write_forward_model(const std::string& filename, const std::vector<double>& frequency, const std::vector<double>& predicted){
		FILE* fp = fileopen(filename.c_str(), "w");
		size_t nf = frequency.size();
		for (size_t i = 0; i < nf; i++){
			fprintf(fp, "%15.6E %15.6E %15.6E\n", frequency[i], predicted[i], predicted[i + nf]);
		}
		fclose(fp);
	}

};

#endif

