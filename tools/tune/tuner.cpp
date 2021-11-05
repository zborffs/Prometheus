#include <iostream>
#include <matplot/matplot.h>
#include "../math/math.hpp"


int main(int argc, char** argv) {
    Eigen::Matrix<double, 10, 1> vector; vector << 1,2,3,4,5,6,7,8,9,10;
    std::cout << vector << std::endl;

    auto my_sample_var = zaamath::sample_var(vector);
    auto my_sample_mean = zaamath::sample_mean(vector);
    auto my_sample_std_dev = zaamath::sample_std_dev(vector);

    std::cout << "Variance: " << my_sample_var << std::endl;
    std::cout << "Mean: " << my_sample_mean << std::endl;
    std::cout << "Standard Deviation: " << my_sample_std_dev << std::endl;

    Eigen::Matrix<double, 10, 1> vector2; vector2 << 2,2,3,5,5,5,7,8,8,10;
    std::cout << vector2 << std::endl << std::endl;

    auto k = zaamath::two_sample_t_test(vector, vector2, 0.001);

//    std::cout << k.t_stat << std::endl << std::endl;
//    std::cout << k.dof << std::endl << std::endl;
//    std::cout << k.crit_val << std::endl << std::endl;
//    std::cout << k.alpha << std::endl << std::endl;
    std::cout << k << std::endl << std::endl;

    auto k2 = zaamath::f_test(vector, vector2, 0.01, zaamath::Tail::BOTH);
    std::cout << k2 << std::endl;

    auto vector_range = zaamath::range(vector);
    std::cout << "Range: " << vector_range << std::endl;


    Eigen::Matrix<double, 15, 1> hours_studying;
    hours_studying << 9.0, 4.9, 1.6, 1.9, 7.9, 2.0, 11.5, 3.9, 1.1, 1.6, 5.1, 8.2, 7.3, 10.4, 11.2;
    Eigen::Matrix<double, 15, 1> exam_grad;
    exam_grad << 88.0, 72.3, 66.5, 65.1, 79.5, 60.8, 94.3, 66.7, 65.4, 63.8, 68.4, 82.5, 75.9, 87.8, 85.2;

    zaamath::Adam adam;
    const int NUM_PARAMS = 2;
    Eigen::Matrix<double, NUM_PARAMS, 1> init({50, 1.0});
    zaamath::Parameters<NUM_PARAMS> engine_params(init);
    std::function<double(zaamath::Parameters<NUM_PARAMS>&)> obj_func = [&](zaamath::Parameters<NUM_PARAMS>& p) {
        Eigen::Matrix<double, 15, 2> x_augmented = Eigen::Matrix<double, 15, 2>::Ones();
        x_augmented.col(1) = hours_studying;
        return 1.0/2.0 * (x_augmented * p.eigen_params() - exam_grad).squaredNorm();
    };

    std::function<Eigen::Matrix<double, NUM_PARAMS, 1>(zaamath::Parameters<NUM_PARAMS>&, int)> jacobian = [&](zaamath::Parameters<NUM_PARAMS>& p, int i) {
        assert(hours_studying.rows() == exam_grad.rows());
        int index = i % hours_studying.rows();
        double f_theta = p.eigen_params()(0) + p.eigen_params()(1) * hours_studying(index);
        double h{0.01};
        double di_f_theta0 = ((p.eigen_params()(1) * hours_studying(index) + p.eigen_params()(0) + h) - (p.eigen_params()(1) * hours_studying(index) + p.eigen_params()(0))) / h;
        double di_f_theta1 = (((p.eigen_params()(1) + h) * hours_studying(index) + p.eigen_params()(0)) - (p.eigen_params()(1) * hours_studying(index) + p.eigen_params()(0))) / h;

        Eigen::Matrix<double, NUM_PARAMS, 1> ret;

        ret(0) = (f_theta - exam_grad(index)) * di_f_theta0; // finite diff approx
        ret(1) = (f_theta - exam_grad(index)) * di_f_theta1; // finite diff approx
//        ret(0) = f_theta - y;
//        ret(1) = (f_theta - y) * x;
        return ret;
    };

    adam.optimize(engine_params, obj_func, jacobian);
    std::cout << "Best Parameters: " << engine_params.eigen_params().transpose() << std::endl;
    std::cout << "Score: " << obj_func(engine_params) << std::endl;
    std::cout << "Sample Mean of Time: " << zaamath::sample_mean(adam.times()) << std::endl;

//    matplot::plot(hours_studying, exam_grad, "o");
//    matplot::hold(matplot::on);
//    Eigen::Matrix<double, 100, 1> myX = Eigen::Matrix<double, 100, 1>::LinSpaced(hours_studying.minCoeff(), hours_studying.maxCoeff());
//    Eigen::Matrix<double, 100, 2> myX_augmented = Eigen::Matrix<double, 100, 2>::Ones();
//    myX_augmented.col(1) = myX;
//    Eigen::Matrix<double, 100, 1> myY = myX_augmented * engine_params.eigen_params();
//    matplot::plot(myX, myY);
//    matplot::hold(matplot::off);
//    matplot::show();

//    matplot::plot(adam.times());
//    matplot::hist(adam.times());
//    matplot::show();
//    matplot::plot(adam.f_x_prev());
//    matplot::show();


    H5Easy::File file("../../tools/data/vector.hdf5", H5Easy::File::Overwrite);

    H5Easy::dump(file, "/vector", vector);

    Eigen::Matrix<double, 10, 1> vector123 = H5Easy::load<Eigen::Matrix<double, 10, 1>>(file, "/vector");
    std::cout << vector << std::endl << std::endl;
    std::cout << vector123 << std::endl << std::endl;
    return 0;
}