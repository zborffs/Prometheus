#include "tune.hpp"

double FirstOrderNumDiff::diff(Eigen::VectorXd &params, double h) {
    return 0;
}

int FirstOrderNumDiff::diff(Eigen::VectorXi &params, int h) {
    return 0;
}

double SecondOrderNumDiff::diff(Eigen::VectorXd &params, double h) {
    return 0;
}

int SecondOrderNumDiff::diff(Eigen::VectorXi &params, int h) {
    return 0;
}

std::pair<Eigen::VectorXd, Eigen::VectorXi> Parameters::diff() {
    return std::pair<Eigen::VectorXd, Eigen::VectorXi>();
}

int Parameters::dimensionality() const {
    return 0;
}

Parameters ParameterParser::load(const std::string &filename) {
    return Parameters();
}

void ParameterParser::save(const std::string &filename, const Parameters &params) {

}

Parameters Tuner::tune(Parameters &params, double max_time, int max_iter) {
    return Parameters();
}
