
#include "Sasktran2Geometry.h"

#include <cerrno>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>

extern char** environ;// このプログラム実行時の環境変数

std::string to_string_double(double value)
{
	if (!std::isfinite(value)) {
		throw std::invalid_argument("Non-finite argument");
	}

	std::ostringstream ss;
	ss << std::setprecision(17) << value;
	return ss.str();
}



std::string run_python_sasktran2(
	const std::string& python,
	const std::string& script,
	const Sasktran2TangentGeometry& geo
){
	std::string values[4] = {
		to_string_double(geo.tangent_altitude_m),
		to_string_double(geo.observer_altitude_m),
		to_string_double(geo.cos_sza),
		to_string_double(geo.relative_azimuth_rad)
	};

	char* argv[] = {
		const_cast<char*>(python.c_str()),
		const_cast<char*>(script.c_str()),
		values[0].data(),
		values[1].data(),
		values[2].data(),
		values[3].data(),
		nullptr
	};

	// Pipe for Python stdout
	int fd[2];
	if (pipe(fd) != 0) {
		throw std::runtime_error("pipe failed");
	}
	
	posix_spawn_file_actions_t actions;
	int error = posix_spawn_file_actions_init(&actions);
	if (error != 0) {
		close(fd[0]);
		close(fd[1]);
		throw std::runtime_error(
			"posix_spawn_file_actions_init failed: " +
			std::string(std::strerror(error))
		);
	}
	if (error == 0)
		error = posix_spawn_file_actions_adddup2(
			&actions, fd[1], STDOUT_FILENO
		);// 標準出力をfd[1]に
	if (error == 0)
		error = posix_spawn_file_actions_addclose(
			&actions, fd[0]
		);// 子プロセスでは不要
	if (error == 0)
		error = posix_spawn_file_actions_addclose(
			&actions, fd[1]
		);// 標準出力にしたので子プロセスでは不要
	if (error != 0) {
		posix_spawn_file_actions_destroy(&actions);
		close(fd[0]);
		close(fd[1]);
		throw std::runtime_error(
			"posix_spawn file actions failed: " +
			std::string(std::strerror(error))
		);
	}

	pid_t pid;
	error = posix_spawn(
		&pid,
		python.c_str(),
		&actions,
		nullptr,
		argv,
		environ
	);
	posix_spawn_file_actions_destroy(&actions);// 設定を保持していたデータ構造を解放
	close(fd[1]);// 親プロセスからは書き込まない
	if (error != 0) {
		close(fd[0]);
		throw std::runtime_error(
			"posix_spawn failed: " +
			std::string(std::strerror(error))
		);
	}

	// Read Python stdout
	std::string output;
	char buffer[4096];
	while (true) {
		ssize_t n = read(fd[0], buffer, sizeof(buffer));
		if (n > 0) {
			output.append(buffer, static_cast<size_t>(n));
		} else if (n == 0) {
			break;
		} else if (errno != EINTR) {
			close(fd[0]);
			throw std::runtime_error("read failed");
		}
	}
	close(fd[0]);

	int status = 0;
	pid_t result;
	do {
		result = waitpid(pid, &status, 0);// 待ち
	} while (result == -1 && errno == EINTR);
	if (result == -1) {
		throw std::runtime_error("waitpid failed");
	}

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
		throw std::runtime_error(
			"Python exited abnormally"
		);
	}

	return output;
}

