//
// Based on "Accuracy evaluation project,
// created by Ashok K. Pant on 3/17/16.
// https://github.com/ashokpant/accuracy-evaluation-cpp
//
// Changes by Pawel Andruszkiewicz
// Verision 1.7 ( 22.01.2018)
//
// MED, Project 2017Z
// Warsaw University of Technology
//

#ifndef ACCURACY_EVALUATION_CPP_CONFUSION_HPP
#define ACCURACY_EVALUATION_CPP_CONFUSION_HPP

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sstream>  // std::stringstream, std::stringbuf
#include <algorithm>
#include <array>
#include <numeric>

using namespace std;

class Confusion {
	enum Statistics
	{
		RECALL=0,
		SPECIFITY,
		PRECISION,
		FMEASURE
	};

public:
	int _classes;
	int _samples;
	double _c;
	vector<vector<double>> _per; //per[classes][4]
	vector<vector<string>> _ind; //ind[classes][classes]
	vector<vector<int>> _cm; //cm[classes][classes]

	std::unordered_map<unsigned short int, std::string> _labels;

	Confusion(int classes, int samples) : _classes(classes), _samples(samples),
		_per(classes, vector<double>(4)), _ind(classes, vector<string>(classes)),
		_cm(classes, vector<int>(classes)) { }

	Confusion(vector<vector<double>> targets, vector<vector<double>> outputs) 
	{
		confusion(targets, outputs);
	}

	Confusion(vector<int> targets, vector<int> outputs, std::unordered_map<unsigned short int, std::string> labels) : Confusion(targets, outputs)
	{
		_labels = labels;
	}

	Confusion(vector<int> targets, vector<int> outputs)
	{
		vector<vector<double>> tar;
		vector<vector<double>> out;
		convertToBooleanMatrix(targets, outputs, tar, out);
		confusion(tar, out);
	}

	void convertToBooleanMatrix(vector<int> targets, vector<int> outputs, vector<vector<double>> &tar,
		vector<vector<double>> &out) {
		int max_target = *max_element(targets.begin(), targets.end());
		int min_target = *min_element(targets.begin(), targets.end());
		int max_output = *max_element(outputs.begin(), outputs.end());
		int min_output = *min_element(outputs.begin(), outputs.end());
		int numClasses = max(max_target, max_output) - min(min_target, min_output) + 1;
		//int numClasses =
			//*max_element(targets.begin(), targets.end()) - *min_element(targets.begin(), targets.end()) + 1;
		int numSamples = targets.size();
		vector<vector<double>> t(numClasses, vector<double>(numSamples));
		vector<vector<double>> o(numClasses, vector<double>(numSamples));
		for (int i = 0; i < numSamples; ++i) {
			t[targets.at(i)][i] = 1;
			o[outputs.at(i)][i] = 1;
		}
		tar = t;
		out = o;
	}

	void confusion(vector<vector<double>> targets, vector<vector<double>> outputs) {
		/* confusion takes an SxQ (S:Classes; Q:Samples)target and output matrices
		T and Y, where each column of T is all zeros with one 1 indicating the target
		class, and where the columns of Y have values in the range [0,1], the largest
		Y indicating the models output class.
		*/

		int numClasses = targets.size(); //row= classes , col=samples

		if (numClasses == 1) {
			cout << "Number of classes must be greater than 1." << endl;
			return;
		}

		int numSamples = targets[0].size();

		_classes = numClasses;
		_samples = numSamples;

		//Transform outputs   (maximum value is set to 1 and other values to 0, column-wise)
		for (int col = 0; col < numSamples; col++) {
			double max = outputs[0][col];
			int ind = 0;

			for (int row = 1; row < numClasses; row++) {
				if (outputs[row][col] > max) {
					max = outputs[row][col];
					ind = row;
				}
				outputs[row][col] = 0.0;
			}
			outputs[0][col] = 0.0;
			outputs[ind][col] = 1;
		}

		//Confusion value
		int count = 0;
		for (int row = 0; row < numClasses; row++) {
			for (int col = 0; col < numSamples; col++) {
				if (targets[row][col] != outputs[row][col])
					count++;
			}
		}
		double c = (double)count / (double)(2 * numSamples);

		// Confusion matrix
		vector<vector<int>> cm(numClasses+1, vector<int>(numClasses+1));
		for (int row = 0; row < numClasses; row++) {
			for (int col = 0; col < numClasses; col++) {
				cm[row][col] = 0;
			}
		}

		vector<int> i(numSamples);
		vector<int> j(numSamples);

		for (int col = 0; col < numSamples; col++) {
			for (int row = 0; row < numClasses; row++) {
				if (targets[row][col] == 1.0) {
					i[col] = row;
					break;
				}
			}
		}

		for (int col = 0; col < numSamples; col++) {
			for (int row = 0; row < numClasses; row++) {
				if (outputs[row][col] == 1.0) {
					j[col] = row;
					break;
				}
			}
		}

		for (int col = 0; col < numSamples; col++) {
			cm[i[col]][j[col]] = cm[i[col]][j[col]] + 1;
			cm[numClasses][j[col]] = cm[numClasses][j[col]] + 1;
			cm[i[col]][numClasses] = cm[i[col]][numClasses] + 1;
		}


		// Percentages
		int num_of_stats = 4;
		vector<vector<double>> stats(numClasses + 2, vector<double>(num_of_stats));
		vector<vector<double>> col(num_of_stats, vector<double>(numClasses));

		for (int row = 0; row < numClasses; row++) {
			for (int col = 0; col < num_of_stats; col++) {
				stats[row][col] = 0.0;
			}
		}

		for (int row = 0; row < numClasses; row++) 
		{
			vector<int> yi(numSamples);
			vector<int> ti(numSamples);
			for (int col = 0; col < numSamples; col++) 
			{
				yi[col] = outputs[row][col];
				ti[col] = targets[row][col];

			}

			int TP = 0;
			int FP = 0;
			int TN = 0;
			int FN = 0;

			for (int col = 0; col < numSamples; col++)
			{
				if (yi[col] != 1 && ti[col] == 1)
				{
					FN += 1;
				}

				if (yi[col] == 1 && ti[col] != 1)
				{
					FP += 1;
				}

				if (yi[col] == 1 && ti[col] == 1)
				{
					TP += 1;
				}

				if (yi[col] != 1 && ti[col] != 1)
				{
					TN += 1;
				}
			}

			float recall = float(TP) / (TP + FN);
			float specifity = float(TN) / (TN + FP);
			float precison = float(TP) / (TP+FP);
			float fmeasure = 2 * precison * recall / (precison + recall);

			stats[row][Statistics::RECALL] = recall;
			stats[row][Statistics::SPECIFITY] = specifity;
			stats[row][Statistics::PRECISION] = precison;
			stats[row][Statistics::FMEASURE] = fmeasure;

			col[Statistics::RECALL][row] = recall;
			col[Statistics::SPECIFITY][row] = specifity;
			col[Statistics::PRECISION][row] = precison;
			col[Statistics::FMEASURE][row] = fmeasure;
		}

		//NAN handling
		for (int row = 0; row < numClasses; row++)
		{
			for (int c = 0; c < 4; c++)
			{
				if (isnan(stats[row][c]))
				{
					stats[row][c] = 0.0;
					col[c][row] = 0.0;
				}
					
			}
		}

		// Mean and stdev
		for (unsigned int i = 0; i < num_of_stats; i++)
		{
			double sum = std::accumulate(col[i].begin(), col[i].end(), 0.0);
			double mean = sum / col[i].size();

			std::vector<double> diff(col[i].size());
			std::transform(col[i].begin(), col[i].end(), diff.begin(), [mean](double x) {return x - mean; });
			double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
			double stdev = std::sqrt(sq_sum / col[i].size());

			stats[numClasses][i] = mean;
			stats[numClasses + 1][i] = stdev;
		}
		

		//NAN handling
		for (int row = 0; row < numClasses; row++) 
		{
			for (int col = 0; col < 4; col++) 
			{
				if (isnan(stats[row][col]))
					stats[row][col] = 0.0;
			}
		}

		_c = c;
		_cm = cm;
		_per = stats;
	}

	string itos(int i) { // convert int to string
		stringstream s;
		s << i;
		return s.str();
	}

	float round(float valueToRound, int numberOfDecimalPlaces) {
		float multiplicationFactor = pow(10, numberOfDecimalPlaces);
		float interestedInZeroDPs = valueToRound * multiplicationFactor;
		return roundf(interestedInZeroDPs) / multiplicationFactor;
	}

	void printC() {
		cout << "\tConfusion value\n\t\tc = " << round(_c, 2) << endl;
	}

	void printAccuracy()
	{
		cout << "\tAccuracy:\n\nacc = " << round(1 - _c, 2) << endl;
	}

	template<typename T> void printElement(T t, const int& width)
	{
		cout << left << setw(width) << setfill(' ') << t;
	}

	int name_width = 20;
	int num_width = 15;


	void printCM() {
		cout << "\tConfusion Matrix:" << endl << endl;

		//stringstream ss;
		//ss << "\t\torg\\dec\t";

		printElement("org\\dec", name_width);
		for (unsigned int i = 0; i < _classes; i++)
		{
			if (_labels.empty())
			{
				//ss << i << "\t";
				printElement(i, num_width);
			}
			else
			{
				//ss << _labels[i] << "\t";
				printElement(_labels[i], num_width);
			}
			
		}
		//ss << "suma\t" << endl;
		printElement("sum", num_width);
		cout << endl;
		cout << "\t\t=======================================" << endl;
		//cout << ss.str();
		for (int row = 0; row <= _classes; row++) {
			if (row < _classes)
			{
				if (_labels.empty())
				{
					//cout << "\t\t" << row << ":\t";
					printElement(row, name_width);

				}
				else
				{
					//cout << "\t\t" << _labels[row] << ":\t";
					printElement(_labels[row], name_width);
				}
				
			}
			else
			{
				cout << endl << "\t\t--------------------------------------" << endl;
				//cout << "\t\tsuma:\t";
				printElement("sum", name_width);
			}
			for (int col = 0; col <= _classes; col++) {
				//cout << _cm[row][col] << "\t";
				printElement(_cm[row][col], num_width);
			}
			cout << endl;
		}
	}

	void printPer() {
		cout << "\tStatistics:" << endl << endl;
		printElement("Positive class", name_width);
		printElement("Recall", num_width);
		printElement("Specifity", num_width);
		printElement("Precision", num_width);
		printElement("F-measure", num_width);
		cout << endl;
		cout << "===========================================================================" << endl;
		//cout << "\t\tPositive\tRecall\tSpecifity\tPrecision\tF-Meaasure" << endl;
		for (int row = 0; row < _classes + 2; row++) 
		{
			if (row < _classes)
			{
				if (_labels.empty())
				{
					//cout << "\t\t" << row << ":\t\t";
					printElement(row, name_width);
				}
				else
				{
					//cout << "\t\t" << _labels[row] << ":\t\t";
					printElement(_labels[row], name_width);
				}
			}
			else if (row == _classes)
			{
				//cout << "\t\tMean:\t\t";
				printElement("mean", name_width);
			}
			else
			{
				//cout << "\t\tStdev:\t\t";
				printElement("StDev", name_width);
			}
			for (int col = 0; col < 4; col++) {
				//cout << round(_per[row][col], 2) << "\t\t";
				printElement(round(_per[row][col], 2), num_width);
			}
			cout << endl;
		}  
	}

	void print() {
		cout << "Testing Results" << endl;
		cout << "=======================================" << endl;
		cout << std::endl;
		printCM();
		cout << std::endl;
		printAccuracy();
		cout << std::endl;
		printPer();
	}

	void print(vector<double> vec) {
		for (double d : vec) {
			cout << d << "\t\t";
		}
		cout << endl;
	}

	void print(vector<vector<double>> vec) {
		for (int i = 0; i < vec.size(); ++i) {
			for (int j = 0; j < vec[0].size(); ++j) {
				cout << vec[i][j] << "\t\t";
			}
			cout << endl;
		}
		cout << endl;
	}
};

#endif //ACCURACY_EVALUATION_CPP_CONFUSION_HPP