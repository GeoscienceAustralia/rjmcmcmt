/*
This source code file is licensed under the GNU GPL Version 2.0 Licence by the following copyright holder:
Crown Copyright Commonwealth of Australia (Geoscience Australia) 2015.
The GNU GPL 2.0 licence is available at: http://www.gnu.org/licenses/gpl-2.0.html. If you require a paper copy of the GNU GPL 2.0 Licence, please write to Free Software Foundation, Inc. 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

Author: Ross C. Brodie, Geoscience Australia.
*/

//Adapted from Matlab code written by Andrew Pethick 2013 - see bottom of file
//http://www.digitalearthlab.com/tutorial/tutorial-1d-mt-forward/

#ifndef _mt_H
#define _mt_H

#include <complex>
#include <cmath>

#include "vector_utils.h"
#include "file_utils.h"

//Scales from modelling units E/H(mV/km/nT) to EDI file units E/B(V/m/A/H)
//795.774715459477 = 4.0*PI/10000;
#define MT_MODELLINGTOFIELDUNITS 795.774715459477

class cEDIFile{

public:

	std::string pathname;
	std::vector<double> freq;
	std::vector<double> zxxr;
	std::vector<double> zxyr;
	std::vector<double> zyxr;
	std::vector<double> zyyr;
	std::vector<double> zxxi;
	std::vector<double> zxyi;
	std::vector<double> zyxi;
	std::vector<double> zyyi;

	std::vector<double> rhoxx;
	std::vector<double> rhoxy;
	std::vector<double> rhoyx;
	std::vector<double> rhoyy;

	std::vector<double> phsxx;
	std::vector<double> phsxy;
	std::vector<double> phsyx;
	std::vector<double> phsyy;

	cEDIFile(){};

	cEDIFile(const std::string& path){
		pathname = path;
		fixseparator(pathname);

		freq = read_token("FREQ");		
		zxxr = read_token("ZXXR");
		zxyr = read_token("ZXYR");
		zyxr = read_token("ZYXR");
		zyyr = read_token("ZYYR");

		zxxi = read_token("ZXXI");		
		zxyi = read_token("ZXYI");
		zyxi = read_token("ZYXI");		
		zyyi = read_token("ZYYI");
				
		rhoxx = read_token("RHOXX");
		rhoxy = read_token("RHOXY");
		rhoyx = read_token("RHOYX");
		rhoyy = read_token("RHOYY");

		phsxx = read_token("PHSXX");
		phsxy = read_token("PHSXY");
		phsyx = read_token("PHSYX");
		phsyy = read_token("PHSYY");		
	};

	cEDIFile(const std::vector<double>& frequency,
		const std::vector<double>& apparent_resistivity,
		const std::vector<double>& apparent_phase)
	{		
		freq  = frequency;
		rhoxy = apparent_resistivity;
		rhoyx = apparent_resistivity;
		phsxy = apparent_phase;
		phsyx = apparent_phase - 180.0;		
	};

	void write(const std::string& path, const std::string& header){
		
		pathname = path;
		fixseparator(pathname);		
		FILE* fp = fileopen(pathname.c_str(), "w");
		fprintf(fp, header.c_str());
		write_token(fp, "FREQ", freq);
		write_token(fp, "RHOXX", rhoxx);
		write_token(fp, "RHOXY", rhoxy);
		write_token(fp, "RHOYX", rhoyx);
		write_token(fp, "RHOYY", rhoyy);
		write_token(fp, "PHSXX", phsxx);
		write_token(fp, "PHSXY", phsxy);
		write_token(fp, "PHSYX", phsyx);
		write_token(fp, "PHSYY", phsyy);
		fprintf(fp, "\n");
		fclose(fp);
	};

	void write_token(FILE* fp, const std::string& token, std::vector<double>& values) const {
		
		if (values.size() == 0)return;
		fprintf(fp, ">%s //%zu\n", token.c_str(),values.size());
		for (size_t i = 0; i < values.size(); i++){
			fprintf(fp, " %13.6le ", values[i]);
			if (i == values.size() - 1 || (i + 1) % 6 == 0){
				fprintf(fp, "\n");
			}
		}

	};

	std::vector<double> read_token(std::string token){
		token = ">" + token;
		std::vector<double> v;

		FILE* fp = fileopen(pathname, "r");
		std::string s;
		int k = 0;
		while (filegetline(fp, s)){
			k++;
			std::vector<std::string> t = tokenize(s);
			if (t.size()>0 && t[0] == token){
				int num;
				int nread;
				nread = sscanf(t.back().c_str(), "%d", &num);
				if (nread == 0){
					nread = sscanf(t.back().c_str(), "//%d", &num);
				}
				while (filegetline(fp, s)) {
					t = tokenize(s);
					for (size_t i = 0; i < t.size(); i++){
						double val = std::atof(t[i].c_str());
						v.push_back(val);
						if (v.size() == (size_t)num){
							fclose(fp);
							return v;
						}
					}
				}
				break;
			}
		}
		fclose(fp);
		return v;
	};
	
	void limit_frequency_range(const double freq_min, const double freq_max){
		
		for (size_t i = 0; i < freq.size(); i++){
			//Remove the null flagged data (e.g. 1e+32) and frequencies out of range
			//if (rhoxy[i] >= 1e30 || rhoyx[i] >= 1e30 ||
			//	phsxy[i] >= 1e30 || phsyx[i] >= 1e30 ||
			//	freq[i] < freq_min ||
			//	freq[i] > freq_max){
			if (freq[i] < freq_min || freq[i] > freq_max){

				freq.erase(freq.begin() + i);

				zxxr.erase(zxxr.begin() + i);
				zxyr.erase(zxyr.begin() + i);
				zyxr.erase(zyxr.begin() + i);
				zyyr.erase(zyyr.begin() + i);

				zxxi.erase(zxxi.begin() + i);
				zxyi.erase(zxyi.begin() + i);
				zyxi.erase(zyxi.begin() + i);
				zyyi.erase(zyyi.begin() + i);

				if (rhoxx.size()>0) rhoxx.erase(rhoxx.begin() + i);
				if (rhoxy.size()>0) rhoxy.erase(rhoxy.begin() + i);
				if (rhoyx.size()>0) rhoyx.erase(rhoyx.begin() + i);
				if (rhoyy.size()>0) rhoyy.erase(rhoyy.begin() + i);
				
				if (phsxx.size()>0) phsxx.erase(phsxx.begin() + i);
				if (phsxy.size()>0) phsxy.erase(phsxy.begin() + i);
				if (phsyx.size()>0) phsyx.erase(phsyx.begin() + i);
				if (phsyy.size()>0) phsyy.erase(phsyy.begin() + i);

				i--;
			}
		}
	}

	std::string StationName()
	{
		sFilePathParts fpp = getfilepathparts(pathname);
		return fpp.prefix;
	};

};

class cMTModeller1D{

private:
	

public:

	cMTModeller1D(){ };

	static std::vector<std::complex<double>> model_impedance(		
		const std::vector<double>& resistivity,
		const std::vector<double>& thickness,
		const std::vector<double>& frequency	
		){

		/*
		This C++ function was converted and adapted from MAATLAB code on the web
		Digital Earth Lab
		www.DigitalEarthLab.com
		Written by Andrew Pethick 2013
		Last Updated October 29th 2013
		Licensed under WTFPL
		*/

		//Returns result in Z(Ohm) = E/H(V/m/A/m)
		size_t nlayers = resistivity.size();
		std::vector<std::complex<double>> impedance(frequency.size());		
		std::vector<std::complex<double>> layerimpedance(nlayers);
		for (size_t fi = 0; fi < frequency.size(); fi++){			
			const double muzeroomega = MUZERO*2.0*PI*frequency[fi];			
			size_t j = nlayers - 1;//bottom layer
			layerimpedance[j] = std::sqrt(std::complex<double>(0.0, muzeroomega*resistivity[j]));
			while(j>0){
				j--;
				std::complex<double> dj = std::sqrt(std::complex<double>(0.0,muzeroomega/resistivity[j]));
				std::complex<double> wj = dj * resistivity[j];
				std::complex<double> rj = (wj - layerimpedance[j + 1]) / (wj + layerimpedance[j + 1]);				
				std::complex<double> re = rj * std::exp(-2.0 * thickness[j] * dj);;
				layerimpedance[j] = wj * ((1.0 - re) / (1.0 + re));				
			}			
			impedance[fi] = layerimpedance[0];			
		}
		return impedance;
	}

	static void impedance_to_appresphase(
		const std::vector<double>& frequency,
		const std::vector<std::complex<double>>& impedance,
		std::vector<double>& app_res,
		std::vector<double>& app_phase
		){
		
		app_res.resize(frequency.size());
		app_phase.resize(frequency.size());
		for (size_t fi = 0; fi < frequency.size(); fi++){
			const double muzeroomega = MUZERO*2.0*PI*frequency[fi];
			app_res[fi] = std::pow(std::abs(impedance[fi]), 2.0) / muzeroomega;
			app_phase[fi] = R2D * std::atan2(impedance[fi].imag(), impedance[fi].real());
		}

	}
};

#endif

