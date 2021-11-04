#include <iostream>
#include <matplot/matplot.h>
#include "../math/math.hpp"


int main(int argc, char** argv) {
    std::cout << "Hello World!" << std::endl;

//    using namespace matplot;
//    std::vector<double> x = linspace(0, 2 * pi);
//    std::vector<double> y = transform(x, [](auto x) { return sin(x); });
//
//    plot(x, y, "-o");
//    hold(on);
//    plot(x, transform(y, [](auto y) { return -y; }), "--xr");
//    plot(x, transform(x, [](auto x) { return x / pi - 1.; }), "-:gs");
//    plot({1.0, 0.7, 0.4, 0.0, -0.4, -0.7, -1}, "k");
//
//    show();

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

    // input
    // output
    // model (maps input to expected output)
    // - model parameters

    // ------
    // error function (maps expected output and actual output to number)
    // error function jacobian (probably requires model jacobian via chain rule) (maps model, input, and output to model parameter type)

    // objective function, objective function jacobian, meta-parameters for algorithm
    // objective function:





    Eigen::Matrix<double,2,1> theta0; theta0 << 50, 1;
    std::function<double(Eigen::Matrix<double,2,1>&, Eigen::Matrix<double, 15, 1>&, Eigen::Matrix<double, 15, 1>&)> J = [&](Eigen::Matrix<double,2,1>& theta, Eigen::Matrix<double, 15, 1>& x, Eigen::Matrix<double, 15, 1>& y) {
        Eigen::Matrix<double, 15, 2> x_augmented = Eigen::Matrix<double, 15, 2>::Ones();
        x_augmented.col(1) = x;
        return 1/2 * (x_augmented * theta - y).squaredNorm();
    };



    std::function<Eigen::Matrix<double, 2, 1>(Eigen::Matrix<double,2,1>&, double, double)> grad_J = [&](Eigen::Matrix<double,2,1>& theta, double x, double y) {
        double f_theta = theta(0) + theta(1) * x;
        double h{0.1};
        double di_f_theta0 = ((theta(1) * x + theta(0) + h) - (theta(1) * x + theta(0))) / h;
        double di_f_theta1 = (((theta(1) + h) * x + theta(0)) - (theta(1) * x + theta(0))) / h;

        Eigen::Matrix<double, 2, 1> ret;

        ret(0) = (f_theta - y) * di_f_theta0; // finite diff approx
        ret(1) = (f_theta - y) * di_f_theta1; // finite diff approx
//        ret(0) = f_theta - y;
//        ret(1) = (f_theta - y) * x;
        return ret;
    };

    Eigen::Matrix<double, 2, 1> my_grad_J = grad_J(theta0, hours_studying(0), exam_grad(0));
    Eigen::Matrix<double, 2, 1> theta = theta0;
    Eigen::Matrix<double, 2, 1> mt = Eigen::Matrix<double, 2, 1>::Zero();
    Eigen::Matrix<double, 2, 1> vt = Eigen::Matrix<double, 2, 1>::Zero();
    Eigen::Matrix<double, 2, 1> mt_hat = Eigen::Matrix<double, 2, 1>::Zero();
    Eigen::Matrix<double, 2, 1> vt_hat = Eigen::Matrix<double, 2, 1>::Zero();
    double beta1 = 0.9;
    double beta2 = 0.999;
    double alpha = 0.1;
    double eps = 1e-8;
    for (int i = 1; i < 10000; ++i) {
        my_grad_J = grad_J(theta, hours_studying((i-1)%15), exam_grad((i-1)%15));
        mt = beta1 * mt + (1-beta1) * my_grad_J;
        vt = beta2 * vt + (1-beta2) * my_grad_J.array().square().matrix();

        mt_hat = mt * 1 / (1 + std::pow(beta1, i));
        vt_hat = vt * 1 / (1 + std::pow(beta2, i));

        theta = theta - (alpha * mt_hat.array() * (vt_hat.array().sqrt() + eps).inverse()).matrix();
        std::cout << "theta: " << theta.transpose() << std::endl;
        std::cout << "Error: " << J(theta, hours_studying, exam_grad) << std::endl;
    }

//    matplot::plot(hours_studying, exam_grad, "o");
//    matplot::hold(matplot::on);
//    Eigen::Matrix<double, 100, 1> myX = Eigen::Matrix<double, 100, 1>::LinSpaced(hours_studying.minCoeff(), hours_studying.maxCoeff());
//    Eigen::Matrix<double, 100, 2> myX_augmented = Eigen::Matrix<double, 100, 2>::Ones();
//    myX_augmented.col(1) = myX;
//    Eigen::Matrix<double, 100, 1> myY = myX_augmented * theta;
//    matplot::plot(myX, myY);
//    matplot::hold(matplot::off);
//    matplot::show();

    zaamath::Adam adam;
    const int NUM_PARAMS = 2;
    Eigen::Matrix<double, NUM_PARAMS, 1> init({50, 1.0});
    zaamath::EngineParameters<NUM_PARAMS> engine_params(init);
    std::function<double(zaamath::EngineParameters<NUM_PARAMS>&)> obj_func = [&](zaamath::EngineParameters<NUM_PARAMS>& p) {
        Eigen::Matrix<double, 15, 2> x_augmented = Eigen::Matrix<double, 15, 2>::Ones();
        x_augmented.col(1) = hours_studying;
        return 1.0/2.0 * (x_augmented * p.eigen_params() - exam_grad).squaredNorm();
    };

    std::function<Eigen::Matrix<double, NUM_PARAMS, 1>(zaamath::EngineParameters<NUM_PARAMS>&, int)> jacobian = [&](zaamath::EngineParameters<NUM_PARAMS>& p, int i) {
        assert(hours_studying.rows() == exam_grad.rows());
        int index = i % hours_studying.rows();
        double f_theta = p.eigen_params()(0) + p.eigen_params()(1) * hours_studying(index);
        double h{0.1};
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

    matplot::plot(hours_studying, exam_grad, "o");
    matplot::hold(matplot::on);
    Eigen::Matrix<double, 100, 1> myX = Eigen::Matrix<double, 100, 1>::LinSpaced(hours_studying.minCoeff(), hours_studying.maxCoeff());
    Eigen::Matrix<double, 100, 2> myX_augmented = Eigen::Matrix<double, 100, 2>::Ones();
    myX_augmented.col(1) = myX;
    Eigen::Matrix<double, 100, 1> myY = myX_augmented * engine_params.eigen_params();
    matplot::plot(myX, myY);
    matplot::hold(matplot::off);
    matplot::show();

//    matplot::plot(adam.times());
    matplot::hist(adam.times());
    matplot::show();
    matplot::plot(adam.f_x_prev());
    matplot::show();


    return 0;
}