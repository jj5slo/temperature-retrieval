
#include "PythonSasktran2.h"

#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <string>
#include <csignal>

#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

extern char** environ;// このプログラム実行時の環境変数

PythonSasktran2::PythonSasktran2(
	const std::string& python,
	const std::string& script
){
	int to_child[2];
	int from_child[2];
	if (pipe(to_child) != 0) {
		throw std::runtime_error("pipe failed");
	}
	if (pipe(from_child) != 0) {
		close(to_child[0]);
		close(to_child[1]);
		throw std::runtime_error("pipe failed");
	}

	posix_spawn_file_actions_t actions;
	int err = posix_spawn_file_actions_init(&actions);
	if (err != 0) {
		close(to_child[0]);
		close(to_child[1]);
		close(from_child[0]);
		close(from_child[1]);
		throw std::runtime_error("spawn actions init failed");
	}

	// Python stdin <- C++ pipe
	if (err == 0){
		err = posix_spawn_file_actions_adddup2(&actions, to_child[0], STDIN_FILENO);// 標準入力をpipeに
	}
	// Python stdout -> C++ pipe
	if (err == 0){
		err = posix_spawn_file_actions_adddup2(&actions, from_child[1], STDOUT_FILENO);// 標準出力をpipeに
	}
	if (err == 0){
		err = posix_spawn_file_actions_addclose(&actions, to_child[0]);
	}// 子プロセスで不要なpipeをclose
	if (err == 0){
		err = posix_spawn_file_actions_addclose(&actions, to_child[1]);
	}
	if (err == 0){
		err = posix_spawn_file_actions_addclose(&actions, from_child[0]);
	}
	if (err == 0){
		err = posix_spawn_file_actions_addclose(&actions, from_child[1]);
	}
	if (err != 0) {
		posix_spawn_file_actions_destroy(&actions);
		close(to_child[0]);
		close(to_child[1]);
		close(from_child[0]);
		close(from_child[1]);
		throw std::runtime_error(
			"spawn actions failed: " +
			std::string(std::strerror(err))
		);
	}

	char* argv[] = {
		const_cast<char*>(python.c_str()),
		const_cast<char*>(script.c_str()),
		nullptr
	};

	err = posix_spawn(
		&pid_,
		python.c_str(),
		&actions,
		nullptr,
		argv,
		environ
	);

	posix_spawn_file_actions_destroy(&actions);// 設定を保持していたデータ構造を解放
	// Parent does not use these ends
	close(to_child[0]);
	close(from_child[1]);
	if (err != 0) {
		close(to_child[1]);
		close(from_child[0]);
		pid_ = -1;
		throw std::runtime_error(
			"posix_spawn failed: " +
			std::string(std::strerror(err))
		);
	}

	input_ = fdopen(to_child[1], "w");
	output_ = fdopen(from_child[0], "r");

	if (!input_ || !output_) {
		if (input_){ fclose(input_); }
		else{ close(to_child[1]); }

		if (output_){ fclose(output_); }
		else{ close(from_child[0]); }

		input_ = nullptr;
		output_ = nullptr;

		// Close child's stdin and wait for termination.
		kill(pid_, SIGTERM);

		int status;
		while (waitpid(pid_, &status, 0) == -1 && errno == EINTR) {}

		pid_ = -1;
		throw std::runtime_error("fdopen failed");
	}
}


double PythonSasktran2::calculate(
	const Sasktran2TangentGeometry& geo
){
	if (!std::isfinite(geo.tangent_altitude_m) ||
		!std::isfinite(geo.observer_altitude_m) ||
		!std::isfinite(geo.cos_sza) ||
		!std::isfinite(geo.relative_azimuth_rad)) {
		throw std::invalid_argument("Non-finite geometry");
	}

	// Send one request
	if (std::fprintf(
			input_,
			"%.17g %.17g %.17g %.17g\n",
			geo.tangent_altitude_m,
			geo.observer_altitude_m,
			geo.cos_sza,
			geo.relative_azimuth_rad
		) < 0 || std::fflush(input_) != 0) {
		throw std::runtime_error("Python input pipe failed");
	}

	// Receive one response
	char buffer[256];

	if (!std::fgets(buffer, sizeof(buffer), output_)) {
		throw std::runtime_error(
			"Python output pipe closed unexpectedly"
		);
	}

	if (std::strcmp(buffer, "ERROR\n") == 0) {
		throw std::runtime_error("Python calculation failed");
	}

	char extra;

	double radiance;

	if (std::sscanf(buffer, "%lf %c", &radiance, &extra) != 1 ||
		!std::isfinite(radiance)) {
		throw std::runtime_error(
			std::string("Invalid Python response: ") + buffer
		);
	}

	return radiance;
}


PythonSasktran2::~PythonSasktran2()
{
	if (input_) {
		std::fprintf(input_, "QUIT\n");
		std::fflush(input_);
		std::fclose(input_);
	}

	if (output_) {
		std::fclose(output_);
	}

	if (pid_ > 0) {
		int status;
		while (waitpid(pid_, &status, 0) == -1 &&
			   errno == EINTR) {}
	}
}

