#include <fmt/base.h>
#include <fmt/format.h>
#include <mpi/mpi.h>
#include <spdlog/spdlog.h>
#include <spdlog/stopwatch.h>

#include <CLI/CLI.hpp>
#include <Eigen/Core>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <map>
#include <string>
#include <vector>

#include "core/dataset.h"
#include "core/message.h"
#include "core/strings.h"
#include "knn/classifier.h"
#include "knn/distance.h"

struct RunConfig {
  ComputeRange range;
  int K;  // NOLINT
  int jobs;
  DistanceType distance;
  std::string dataset;
  int rank;
  int mpiSize;
};

void Run(const RunConfig &runConfig) {
  const char *const EnableVerbose = getenv("ENABLE_VERBOSE");  // NOLINT

  const Dataset train{runConfig.dataset};
  const Dataset test{runConfig.dataset, true};

  const auto &range = runConfig.range;

  KNeighborsClassifierCreateConfig config{};
  config.K = runConfig.K;  // NOLINT
  config.distanceType = runConfig.distance;
  config.jobs = runConfig.jobs;
  config.startLine = range.startLine;
  config.endLine = range.endLine;
  config.outputFile = fmt::format("process_{}.txt", runConfig.rank);

  KNeighborsClassifier knn{config};
  knn.Fit(train.X(), train.Y());

  const spdlog::stopwatch sw;
  knn.Predict(test.X());

  bool dummy = true;
  if (runConfig.rank != 0) {
    MPI_Send(&dummy, 1, MPI_CXX_BOOL, 0, 0, MPI_COMM_WORLD);
  }
  if (runConfig.rank == 0) {
    if (runConfig.mpiSize > 1) {
      MPI_Status status;
      MPI_Recv(&dummy, runConfig.mpiSize, MPI_CXX_BOOL, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
    }

    std::vector<int> predict(static_cast<size_t>(range.totalLines));
    for (int i = 0; i < runConfig.mpiSize; ++i) {
      std::fstream file{fmt::format("process_{}.txt", i), std::ios_base::in};

      std::string line;
      file >> line;
      const auto &splitted = StringsUtils::Split(line, ',');
      const size_t startLine = std::stoul(splitted[0]);
      const size_t endLine = std::stoul(splitted[1]);

      for (size_t j = startLine; j < endLine; ++j) {
        file >> line;
        predict[j] = std::stoi(line);
      }

      file.close();
    }

    const auto &elapsed = sw.elapsed();

    size_t right{0};
    for (size_t i = 0; i < predict.size(); ++i) {
      right = (predict[i] == test.Y()[i]) ? right + 1 : right;
    }

    const float acc = static_cast<float>(right) / static_cast<float>(predict.size());
    if (EnableVerbose != nullptr) {
      spdlog::info("Accuracy: {:.2f}% | Time: {}", acc * 100.F, elapsed.count());  // NOLINT
    } else {
      fmt::println("{:.2f},{}", acc, elapsed.count());
    }
  }
}

ComputeRange GetRank0Range(const std::string &dataset, int mpiSize) {
  const char *const EnableVerbose = getenv("ENABLE_VERBOSE");  // NOLINT
  std::vector<ComputeRange> messages(static_cast<size_t>(mpiSize), ComputeRange{});

  const std::string &filename = fmt::format("{}/X_test.csv", dataset);
  const int totalLines = [&filename]() {
    std::ifstream file{filename};

    std::string line;
    int count{0};

    while (std::getline(file, line)) {
      count++;
    }

    // Ignore first line (header)
    return count - 1;
  }();
  const int linesPerProcess = static_cast<int>(std::floor(totalLines / mpiSize));

  if (EnableVerbose != nullptr) {
    spdlog::info("Lines/Process: {} | Total Lines: {}", linesPerProcess, totalLines);
  }

  int process{0};
  int startLine{0};
  int endLine{0};
  while (endLine < totalLines) {
    endLine = startLine + linesPerProcess;

    if (process == mpiSize - 1) {
      endLine += totalLines - endLine;
    }

    const auto i = static_cast<size_t>(process);
    messages[i] = {startLine, endLine, totalLines};

    if (EnableVerbose != nullptr) {
      spdlog::info("[process = {}] {} - {} ({} lines)", process, startLine, endLine, endLine - startLine);
    }

    process++;
    startLine = process * linesPerProcess + 1;
  }

  for (int i = 1; i < mpiSize; ++i) {
    MPI_Send(messages[static_cast<size_t>(i)].Serialize(), sizeof(ComputeRange), MPI_BYTE, i, 0, MPI_COMM_WORLD);
  }

  return messages[0];
}

ComputeRange GetGeneralRange(int mpiRank) {
  const char *const EnableVerbose = getenv("ENABLE_VERBOSE");  // NOLINT

  MPI_Status status;
  uint8_t buffer[sizeof(ComputeRange)];
  MPI_Recv(&buffer, sizeof(ComputeRange), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);

  ComputeRange data;
  data.ParseFromBuffer(buffer);  // NOLINT

  if (EnableVerbose != nullptr) {
    spdlog::info("[On process = {}]: {} - {}", mpiRank, data.startLine, data.endLine);
  }

  return data;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char **argv) {
  if (const int ret = MPI_Init(&argc, &argv); ret != MPI_SUCCESS) {
    spdlog::error("Error when tried to init MPI. Aborting...");
    MPI_Abort(MPI_COMM_WORLD, ret);
  }

  int mpiRank{0};
  int mpiSize{0};
  MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpiSize);

  CLI::App app{"KNN Algorithm implemented with HPC techniques."};
  argv = app.ensure_utf8(argv);

  std::string dataset{"resources/iris_dataset"};
  app.add_option("-d,--dataset", dataset)->check(CLI::ExistingPath);

  int K{3};                                                        // NOLINT
  app.add_option("-k,--neighbors", K)->check(CLI::Range(3, 100));  // NOLINT

  DistanceType distance{DistanceType::kEuclidian};
  const std::map<std::string, DistanceType> map{{"euclidian", DistanceType::kEuclidian},
                                                {"e", DistanceType::kEuclidian},
                                                {"manhattan", DistanceType::kManhattan},
                                                {"m", DistanceType::kManhattan}};
  app.add_option("-f,--distance", distance)
      ->check(CLI::IsMember({"euclidian", "e", "manhattan", "m"}, CLI::ignore_case))
      ->transform(CLI::CheckedTransformer(map, CLI::ignore_case));

  int jobs{1};
  app.add_option("-j,--jobs", jobs);

  bool verbose{false};
  app.add_flag("-v,--verbose", verbose);

  bool printCalc{false};
  app.add_flag("-c,--print_calc", printCalc, "Needs to be used with -v flag.");

  CLI11_PARSE(app, argc, argv);

  if (verbose) {
    setenv("ENABLE_VERBOSE", "true", 0);  // NOLINT
  }
  if (verbose && printCalc) {
    setenv("ENABLE_PRINT_CALC", "true", 0);  // NOLINT
  }
  if (verbose && mpiRank == 0) {
    int mpiVersion{0};
    int mpiSubVersion{0};
    MPI_Get_version(&mpiVersion, &mpiSubVersion);
    spdlog::info("MPI Version {}.{}", mpiVersion, mpiSubVersion);

    int libVersion{0};
    std::string mpiLibVersion(MPI_MAX_PROCESSOR_NAME, '\0');
    MPI_Get_library_version(mpiLibVersion.data(), &libVersion);  // NOLINT
    mpiLibVersion.resize(static_cast<size_t>(libVersion));
    mpiLibVersion = StringsUtils::Split(mpiLibVersion, ',')[0];
    spdlog::info("MPI Library Version: {}", mpiLibVersion);

    int strLen{0};
    std::string machine(MPI_MAX_PROCESSOR_NAME, '\0');
    MPI_Get_processor_name(machine.data(), &strLen);
    machine.resize(static_cast<size_t>(strLen));
    spdlog::info("MPI Machine Name: {}", machine);

    spdlog::info("MPI Size: {}", mpiSize);
  }

  RunConfig config{};
  config.K = K;
  config.jobs = jobs;
  config.dataset = dataset;
  config.distance = distance;
  config.rank = mpiRank;
  config.mpiSize = mpiSize;
  config.range = (mpiRank == 0) ? GetRank0Range(dataset, mpiSize) : GetGeneralRange(mpiRank);

  Run(config);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
