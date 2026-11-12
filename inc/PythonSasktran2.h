
#ifndef PYTHON_SASKTRAN2_H
#define PYTHON_SASKTRAN2_H

#include <cstdio>
#include <string>
#include <sys/types.h>

#include "Sasktran2Geometry.h"

class PythonSasktran2 {
private:
	pid_t pid_ = -1;
	FILE* input_ = nullptr;
	FILE* output_ = nullptr;

public:
	PythonSasktran2(
		const std::string& python,
		const std::string& script
	);

	~PythonSasktran2();

	PythonSasktran2(const PythonSasktran2&) = delete;
	PythonSasktran2& operator=(const PythonSasktran2&) = delete;

	double calculate(const Sasktran2TangentGeometry& geo);
};

#endif

