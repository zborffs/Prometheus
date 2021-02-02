#ifndef CLOAK_TUNE_HPP
#define CLOAK_TUNE_HPP

#include <Eigen/Core>
#include "eval.hpp"

class NumericalDifferentiator {
public:
    virtual ~NumericalDifferentiator() = default;
    virtual double diff(Eigen::VectorXd& params, double h) = 0;
    virtual int diff(Eigen::VectorXi& params, int h) = 0;
};

class FirstOrderNumDiff : public NumericalDifferentiator {
public:
    double diff(Eigen::VectorXd& params, double h) override; // evaluates evaluation function
    int diff(Eigen::VectorXi& params, int h) override; // evaluates evaluation function
};

class SecondOrderNumDiff : public NumericalDifferentiator {
public:
    double diff(Eigen::VectorXd& params, double h) override; // evaluates evaluation function
    int diff(Eigen::VectorXi& params, int h) override; // evaluates evaluation function
};

// assume parameters are locally linear
class Parameters {
    Eigen::VectorXd double_params_;
    Eigen::VectorXi int_params_;
    double double_h_{0.01};
    int double_int_{1}; // 1
    int dimensionality_; // sum of two params vectors

public:
    std::pair<Eigen::VectorXd, Eigen::VectorXi> diff(); // differentiates both
    int dimensionality() const; // returns dimensionality of parameters
    // RetType serialize() const; // returns serialization of the Parameters object
};

/**
 * Reads and writes to a file containing serialization (or something) of Parameters object
 */
class ParameterParser {
public:
    static Parameters load(const std::string& filename); // read and deserialize
    static void save(const std::string& filename, const Parameters& params); // serialize and write
};

class Tuner {
    Parameters params_;
    double alpha_{0.25};
    double gamma_{0.25};
    double lambda_{0.7};

public:
    Parameters tune(Parameters& params, double max_time, int max_iter);
};

#endif // CLOAK_TUNE_HPP
