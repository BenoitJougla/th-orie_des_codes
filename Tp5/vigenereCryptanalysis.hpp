#ifndef _VIGENERE_CRYPTANALYSIS_HPP_
#define _VIGENERE_CRYPTANALYSIS_HPP_

#include <array>
#include <string>
#include <vector>

class VigenereCryptanalysis
{	
	private:
		std::array<double, 26> targets;
		std::array<double, 26> sortedTargets;
	 
	public:
		VigenereCryptanalysis(const std::array<double, 26>& targetFreqs);
		std::pair<std::string, std::string> analyze(std::string input);
		
	private :
		/**************************Find period**************************/
		/*
		 * http://www.practicalcryptography.com/cryptanalysis/text-characterisation/index-coincidence/
		 * http://www.practicalcryptography.com/cryptanalysis/stochastic-searching/cryptanalysis-vigenere-cipher/
		 */
		 
		std::vector<int> findPeriod(const std::string& input);
		double indexOfXoincidence(const std::string& input, int keyLength);

		/**************************Find key**************************/
		//http://www.practicalcryptography.com/cryptanalysis/text-characterisation/chi-squared-statistic/
		
		double chiSquared(const std::string& input);
		std::string findKey(const std::string& input, int keyLength);
};

#endif
