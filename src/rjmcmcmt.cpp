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
#include <stdexcept>


#define USEGLOBALSTACKTRACE
#ifdef USEGLOBALSTACKTRACE
	#include "stacktrace.h"
	cStackTrace globalstacktrace;
#endif

#include "general_utils.h"
#include "file_utils.h"
#include "vector_utils.h"
#include "blocklanguage.h"
#include "mpi_wrapper.h"

#include "mt.h"
#include "resistivity_model.h"


extern "C"{
#include <rjmcmc/forwardmodel_mpi.h>
#include <rjmcmc/rjmcmc_random.h>
#include <rjmcmc/resultset1dfm.h>
#include <rjmcmc/rjmcmc_util.h>
}

class cRjMcMCMT{

private:
		

	cEDIFile E;
	MTDataType  DataType;	
	std::string OutputDirectory;
	bool PrintBestFits;

	double lowestmisfit = 1e38;	
	std::vector<double> dobs;
	std::vector<double> derr;
	int totalsamples;
	int burninsamples;	
	int min_part;
	int max_part;
	double xmin;
	double xmax;
	int nxsamples;

	double ymin;
	double ymax;
	int nysamples;
	double credible_interval;
	
	double pd;
	int nglobal_parameters = 0;
	int nlocal_parameters  = 0;

public:	

	cRjMcMCMT(const std::string& controlfile, const std::string& edifile){
		cBlock b(controlfile);
		E = cEDIFile(edifile);
		parse_options(b.findblock("Options"));
		write_station_info();		
		get_data_and_noise(b.findblock("Data"));		
		run();
	};

	std::string StationDirectory()
	{		
		return OutputDirectory + E.StationName() + pathseparatorstring();
	};

	void parse_options(const cBlock& b){

		std::string usedatatype;
		if (b.getvalue("UseDataType", usedatatype) == false){
			throw(std::runtime_error("UseDataType was not found"));
		}
		else{
			if (strcasecmp(usedatatype,"ApparentResistivityPhase") == 0){
				DataType = MT_DT_APPRESPHASE;				
			}
			else if(strcasecmp(usedatatype, "Impedance") == 0){
				DataType = MT_DT_IMPEDANCE;
			}
			else{
				throw(std::runtime_error("UseDataType must be either ApparentResistivityPhase or Impedance"));
			}
		}

		if (b.getvalue("TotalSamples", totalsamples) == false){
			throw(std::runtime_error("TotalSamples was not found"));
		}

		if (b.getvalue("BurninSamples", burninsamples) == false){
			throw(std::runtime_error("BurninSamples was not found"));
		}

		min_part = 2;
		int maxlayers;
		if (b.getvalue("MaxLayers", maxlayers) == false){
			throw(std::runtime_error("Maxlayers was not found"));
		}
		max_part = maxlayers + 1;
		xmin = 0;//i.e. minimum depth at 1m

		if (b.getvalue("MaxInterfaceDepth", xmax) == false){
			throw(std::runtime_error("MaxInterfaceDepth was not found"));
		}
		else xmax = std::log10(xmax);

		if (b.getvalue("NumDepthBins", nxsamples) == false){
			throw(std::runtime_error("NumDepthBins was not found"));
		}

		if (b.getvalue("MinResistivity", ymin) == false){
			throw(std::runtime_error("MinResistivity was not found"));
		}
		else ymin = std::log10(ymin);
		
		if (b.getvalue("MaxResistivity", ymax) == false){
			throw(std::runtime_error("MaxResistivity was not found"));
		}
		else ymax = std::log10(ymax);

		if (b.getvalue("NumResistivityBins", nysamples) == false){
			throw(std::runtime_error("NumResistivityBins was not found"));
		}

		if (b.getvalue("CredibleInterval", credible_interval) == false){
			throw(std::runtime_error(std::string("CredibleInterval was not found")));
		}

		if (b.getvalue("OutputDirectory", OutputDirectory) == false){
			throw(std::runtime_error("StationDirectory was not found"));
		}
		else{
			fixseparator(OutputDirectory);
			if (OutputDirectory[OutputDirectory.size()-1] != pathseparator()){
				OutputDirectory.push_back(pathseparator());
			}
			if (cMpiEnv::world_rank() == 0){
				makedirectorydeep(OutputDirectory);
				makedirectorydeep(StationDirectory());
			}
			cMpiComm c = cMpiEnv::world_comm(); c.barrier();
		}
		
		PrintBestFits = b.getboolvalue("PrintBestFits");

	}

	void get_data_and_noise(const cBlock& b){
		
		std::string freq_t   = b.getstringvalue("Frequency.Token");
		
		//double period_min = b.getdoublevalue("Frequency.MinimumPeriod");
		//double period_max = b.getdoublevalue("Frequency.MaximumPeriod");
		double freq_min = b.getdoublevalue("Frequency.Minimum");
		double freq_max = b.getdoublevalue("Frequency.Maximum");

		if (freq_min == cBlock::ud_double())freq_min = 0.0;
		if (freq_max == cBlock::ud_double())freq_max = 1e16;
				
		double zr_nr = b.getdoublevalue("ImpedanceReal.ErrorRelative");
		double zr_nf = b.getdoublevalue("ImpedanceReal.ErrorFloor");		
		double zi_nr = b.getdoublevalue("ImpedanceImaginary.ErrorRelative");
		double zi_nf = b.getdoublevalue("ImpedanceImaginary.ErrorFloor");

		std::string rhoxy_t = b.getstringvalue("ApparentResistivity.TokenXY");
		std::string rhoyx_t = b.getstringvalue("ApparentResistivity.TokenYX");
		double rho_nr     = b.getdoublevalue("ApparentResistivity.ErrorRelative");
		double rho_nf     = b.getdoublevalue("ApparentResistivity.ErrorFloor");

		std::string phsxy_t = b.getstringvalue("ApparentPhase.TokenXY");
		std::string phsyx_t = b.getstringvalue("ApparentPhase.TokenYX");
		double phs_nr     = b.getdoublevalue("ApparentPhase.ErrorRelative");
		double phs_nf     = b.getdoublevalue("ApparentPhase.ErrorFloor");

		E.limit_frequency_range(freq_min, freq_max);		
		size_t nf = E.freq.size();
		if (DataType == MT_DT_IMPEDANCE){			
			std::vector<double> zr(nf);
			std::vector<double> zi(nf);
			std::vector<double> zr_err(nf);
			std::vector<double> zi_err(nf);
			for (size_t i = 0; i < nf; i++){				
				std::complex<double> zxx(E.zxxr[i], E.zxxi[i]);
				std::complex<double> zxy(E.zxyr[i], E.zxyi[i]);
				std::complex<double> zyx(E.zyxr[i], E.zyxi[i]);
				std::complex<double> zyy(E.zyyr[i], E.zyyi[i]);
				std::complex<double> zdet = std::sqrt(zxx*zyy - zxy*zyx);
				zr[i] = zdet.real();
				zi[i] = zdet.imag();
			
				zr_err[i] = std::sqrt(std::pow(zr[i] * zr_nr, 2.0) + std::pow(zr_nf, 2.0));
				zi_err[i] = std::sqrt(std::pow(zi[i] * zi_nr, 2.0) + std::pow(zi_nf, 2.0));
			}
			dobs = concaternate(zr, zi);
			derr = concaternate(zr_err, zi_err);
		}
		else if (DataType == MT_DT_APPRESPHASE){
			E.phsyx = 180.0 + E.phsyx;
			//std::vector<double> rho = pow10(0.5*(log10(E.rhoxy) + log10(E.rhoyx)));
			std::vector<double> rho(nf);
			std::vector<double> phs(nf);
			std::vector<double> rho_err(nf);
			std::vector<double> phs_err(nf);
			for (size_t i = 0; i < rho.size(); i++){				
				rho[i] = std::sqrt(E.rhoxy[i] * E.rhoyx[i]);//Geometric mean
				phs[i] = 0.5*(E.phsxy[i] + E.phsyx[i]); //Arithemitic mean
				rho_err[i] = std::sqrt(std::pow(rho[i] * rho_nr, 2.0) + std::pow(rho_nf, 2.0));
				phs_err[i] = std::sqrt(std::pow(phs[i] * phs_nr, 2.0) + std::pow(phs_nf, 2.0));
			}
			dobs = concaternate(rho, phs);
			derr = concaternate(rho_err, phs_err);
		}

		ResistivityModel::write_forward_model(StationDirectory()+"data.txt", E.freq, dobs);
		ResistivityModel::write_forward_model(StationDirectory()+"noise.txt", E.freq, derr);
	}
	
	static double forward_model_function(void* userarg,
		int npartitions, const double* partition_boundaries,
		int nglobalparameters, const double* global_parameters,
		part1d_fm_likelihood_state_t* state,
		part1d_fm_value_at_t value_at,
		part1d_fm_value_at_t gradient_at)
	{
		static int nit = 0;
		nit++;

		cRjMcMCMT* me = (cRjMcMCMT*)userarg;
		size_t nprops = (size_t)me->nlocal_parameters;
		std::vector<double> depths(partition_boundaries, partition_boundaries + npartitions);
		size_t nlayers = depths.size() - 1;
		std::vector<std::vector<double>> props(nlayers);
		for (size_t li = 0; li < nlayers; li++){
			const double* p = value_at(state, depths[li]);
			props[li]    = std::vector<double>(p, p + nprops);
			props[li][0] = std::pow(10.0, props[li][0]);
		}

		//Convert to linear depth
		depths[0] = 0;//Account for minimum partition depth at 1m (xmin=0)
		for (size_t li = 1; li < depths.size(); li++){
			depths[li]   = std::pow(10.0,depths[li]);
		}
		
		ResistivityModel m(depths, props);
		std::vector<double> dpre;
		if(me->DataType == MT_DT_IMPEDANCE){
			dpre = m.model_impedance_fieldunits_as_vector(me->E.freq);
		}
		else if (me->DataType == MT_DT_APPRESPHASE){
			dpre = m.model_appresphase_as_vector(me->E.freq);
		}				
		std::vector<double> nr   = (dpre - me->dobs) / me->derr;

		double misfit = 0.0;
		for (size_t i = 0; i < nr.size(); i++){
			misfit += nr[i] * nr[i];
		}
		
		if (misfit < me->lowestmisfit){
			me->lowestmisfit = misfit;
			std::string bmf = me->StationDirectory() + "best_model" + strprint(".%03d", cMpiEnv::world_rank()) + ".txt";
			m.write(bmf);

			std::string bmff = me->StationDirectory() + "best_model_forward" + strprint(".%03d", cMpiEnv::world_rank()) + ".txt";						
			m.write_forward_model(bmff, me->E.freq, dpre);

			if (me->PrintBestFits){
				printf("It=%d misfit=%10.1lf\n", nit, misfit);
				m.print();
				printf("\n");
			}
		}

		return 0.5*misfit;
	}

	int run(){

		nglobal_parameters = 0;
		forwardmodelparameter_t* global_parameters = NULL;

		nlocal_parameters = 1;
		forwardmodelparameter_t properties[1];

		//set the standard deviation on depth perturbation to 5% of the range
		pd = 0.05 * (xmax - xmin);

		//Resistivity
		size_t k = 0;
		properties[k].fmin = ymin;
		properties[k].fmax = ymax;
		//set the standard deviation on property perturbation to 5% of the range
		properties[k].fstd_value = 0.05*(properties[k].fmax - properties[k].fmin);
		properties[k].fstd_bd = 2.0 * properties[k].fstd_value;

		void* user_data = (void*)this;
		int mpiroot = 0;//My guiess is that this the process where the results are merged
		resultset1dfm_t* results = MPI_part1d_forwardmodel(burninsamples,
			totalsamples, min_part, max_part, xmin, xmax,
			nxsamples, nysamples, credible_interval, pd,
			rjmcmc_uniform, rjmcmc_normal,
			nglobal_parameters, global_parameters,
			nlocal_parameters, properties,
			forward_model_function, user_data,
			RESULTSET1DFM_MEAN | RESULTSET1DFM_MEDIAN | RESULTSET1DFM_MODE | RESULTSET1DFM_CREDIBLE,
			cMpiEnv::world_size(), cMpiEnv::world_rank(), mpiroot, cMpiEnv::world_comm());


		if (results == NULL){
			fprintf(stderr, "error: failed to run functionfit\n");
			return -1;
		}

		save_convergences(results);
		if (cMpiEnv::world_rank() == mpiroot) save_results(results);
		
		return 0;
	}

	bool write_station_info(){
		if (cMpiEnv::world_rank() == 0){
			std::string statfile = StationDirectory() + "station_info.txt";
			std::ofstream o(statfile);
			o << E.StationName() << std::endl;
			if (DataType == MT_DT_IMPEDANCE){
				o << "Impedance" << std::endl;
			}
			else{
				o << "ApparentResistivityPhase" << std::endl;
			}			
			o << cMpiEnv::world_size() << std::endl;
			o << burninsamples << std::endl;
			o << totalsamples  << std::endl;
		}
		cMpiComm c = cMpiEnv::world_comm(); c.barrier();
		return true;
	}

	bool save_convergences(resultset1dfm_t* results){
		const double* v = resultset1dfm_get_misfit(results);		
		std::string filename = StationDirectory() + "misfit" + strprint(".%03d", cMpiEnv::world_rank()) + ".txt";
		//rjmcmc_save_vector(filename.c_str(), v, totalsamples);

		//Only save every 100th sample to limit file size, but save sample number as well
		FILE* fp=fileopen(filename,"w");
		for(int i=0; i<totalsamples; i=i+100){
			fprintf(fp,"%d %lf\n",i+1,v[i]);
		}
		fclose(fp);
		return true;
	}
	
	bool save_results(resultset1dfm_t* results){
				
		std::string filename;

		double* ycoords = rjmcmc_create_array_1d(nysamples);		
		double* xcoords = rjmcmc_create_array_1d(nxsamples);

		resultset1dfm_fill_xcoord_vector(results, xcoords, &nxsamples);
		filename = StationDirectory() + "depth_bins.txt";
		rjmcmc_save_vector(filename.c_str(), xcoords, nxsamples);
		
		const int* iv = resultset1dfm_get_partitions(results);
		filename = StationDirectory() + "partitions.txt";
		rjmcmc_save_int_vector(filename.c_str(), iv, totalsamples);
		filename = StationDirectory() + "partitions_hist.txt";
		rjmcmc_save_int_vector_as_histogram(filename.c_str(), 2, max_part, iv, totalsamples);
		
		iv = resultset1dfm_get_partition_x_histogram(results);		
		filename = StationDirectory() + "partitions_depth_hist.txt";
		rjmcmc_save_int_coords(filename.c_str(), xcoords, iv, nxsamples);
		
		save_property_histograms(results);
		
		//Property bins

		for (int k = 0; k < nlocal_parameters; k++){
			int pi = k + 1;
			resultset1dfm_fill_ycoord_vector(results, k, ycoords, &nysamples);
			filename = StationDirectory() + strprint("prop_%d_bins.txt", pi);			
			rjmcmc_save_vector(filename.c_str(), ycoords, nysamples);
		}

		//Mean
		for (int k = 0; k < nlocal_parameters; k++){
			int pi = k + 1;
			const double* v = resultset1dfm_get_local_parameter_mean(results, k);			
			filename = StationDirectory() + strprint("prop_%d_mean.txt", pi);
			rjmcmc_save_coords(filename.c_str(), xcoords, v, nxsamples);
		}

		//Median
		for (int k = 0; k < nlocal_parameters; k++){
			int pi = k + 1;
			const double* v = resultset1dfm_get_local_parameter_median(results, k);
			filename = StationDirectory() + strprint("prop_%d_median.txt", pi);
			rjmcmc_save_coords(filename.c_str(), xcoords, v, nxsamples);
		}

		//Mode
		for (int k = 0; k < nlocal_parameters; k++){
			int pi = k + 1;
			const double* v = resultset1dfm_get_local_parameter_mode(results, k);			
			filename = StationDirectory() + strprint("prop_%d_mode.txt", pi);
			rjmcmc_save_coords(filename.c_str(), xcoords, v, nxsamples);
		}

		//Credible min
		for (int k = 0; k < nlocal_parameters; k++){
			int pi = k + 1;
			const double* v = resultset1dfm_get_local_parameter_credible_min(results, k);
			filename = StationDirectory() + strprint("prop_%d_credmin.txt", pi);
			rjmcmc_save_coords(filename.c_str(), xcoords, v, nxsamples);
		}

		//Credible max
		for (int k = 0; k < nlocal_parameters; k++){
			int pi = k + 1;
			const double* v = resultset1dfm_get_local_parameter_credible_max(results, k);
			filename = StationDirectory() + strprint("prop_%d_credmax.txt", pi);
			rjmcmc_save_coords(filename.c_str(), xcoords, v, nxsamples);
		}
		rjmcmc_destroy_array_1d(xcoords);
		rjmcmc_destroy_array_1d(ycoords);
		resultset1dfm_destroy(results);
		return 0;
	}

	int save_property_histograms(resultset1dfm_t* results){

		for (int k = 0; k < nlocal_parameters; k++){
			int pi = k + 1;
			std::string filename = StationDirectory()+strprint("prop_%d_hist.txt", pi);
			FILE* fp = fileopen(filename, "w");
			const int** h = resultset1dfm_get_local_parameter_histogram(results, k);
			for (int i = 0; i < nxsamples; i++){
				for (int j = 0; j < nysamples; j++){
					fprintf(fp, "%d ", h[i][j]);
				}
				fprintf(fp, "\n");
			}
			fclose(fp);
		}
		return 0;
	}
};

int main(int argc, char** argv)
{		
	_GSTITEM_
	if (argc != 2){
		printf("Usage: %s control_file_name\n", argv[0]);
		return 1;
	}

	try
	{		
		_GSTITEM_		
		cMpiEnv mpienv(argc,argv);
		int seed = (int) time(NULL) + mpienv.world_rank();
		rjmcmc_seed(seed);

		cBlock b(argv[1]);
		std::string edifiledir = b.getstringvalue("Data.EdiFileDir");
		std::vector<std::string> f=getfilelist(edifiledir,"edi");
		for (size_t i = 0; i < f.size(); i++){
			if (mpienv.world_rank() == 0){
				printf("Inverting station %d of %d (%s)\n", (int)i+1, (int)f.size(), f[i].c_str());
				fflush(stdout);
			}
			cRjMcMCMT I(argv[1],f[i]);
		}		
		return 0;
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
