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

    return 0;
}