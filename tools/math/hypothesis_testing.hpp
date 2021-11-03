#ifndef PROMETHEUS_HYPOTHESIS_TESTING_HPP
#define PROMETHEUS_HYPOTHESIS_TESTING_HPP

#include <iostream>
#include "summary_statistics.hpp"
#include "optimization.hpp"

namespace zaamath {

    template <class T, int ColIndexType>
    struct TwoSampleTTestResult {
        Eigen::Matrix<T, 1, ColIndexType> t_stat;
        Eigen::Matrix<double, 1, ColIndexType> dof;
        Eigen::Matrix<double, 1, ColIndexType> crit_val;
        Eigen::Matrix<bool, 1, ColIndexType> reject_null_hypothesis;
        double alpha;


        friend std::ostream& operator<<(std::ostream& os, TwoSampleTTestResult<T, ColIndexType>& test) {
            os << std::boolalpha;

            os << "Two-Sample t-Test for Equal Means" << std::endl;
            os << "- t-statistic: " << test.t_stat << std::endl;
            os << "- dof: " << test.dof << std::endl;
            os << "- critical values: " << test.crit_val << std::endl;
            os << "- reject null hypothesis: " << test.reject_null_hypothesis << std::endl;
            os << "- alpha: " << test.alpha;

            os << std::noboolalpha;

            return os;
        }
    };

    template <class T, int RowIndexType1, int RowIndexType2, int ColIndexType>
    TwoSampleTTestResult<T, ColIndexType> two_sample_t_test(Eigen::Matrix<T, RowIndexType1, ColIndexType>& data1, Eigen::Matrix<T, RowIndexType2, ColIndexType>& data2, double alpha) {
        int N1 = data1.size();
        int N2 = data2.size();

        auto Y1bar = sample_mean(data1);
        auto Y2bar = sample_mean(data2);
        auto s1bar = sample_var(data1);
        auto s2bar = sample_var(data2);

        auto t1 = (s1bar / N1 + s2bar / N2).array();
        Eigen::Matrix<double, 1, ColIndexType> nu = (t1 * (((s1bar / N1).array().square() / (N1-1) + (s2bar / N2).array().square() / (N2-1)).inverse()));

        double x_abstol = 1e-10;
        double f_x_abstol = 1e-12;

        // why in function? make into a class, struct, or separate function in optimization.hpp
        std::function<double(double)> critval_func = [&](double my_nu) {
            double x_high{4};
            double x_low{0.5};
            double x_middle = (x_high + x_low) / 2;
            double f_x_high = stats::pt(x_high, my_nu) - (1-alpha/2);
            double f_x_low = stats::pt(x_low, my_nu) - (1-alpha/2);
            double f_x_middle = stats::pt(x_middle, my_nu) - (1-alpha/2);

            while (x_high - x_low > x_abstol && f_x_high - f_x_low > f_x_abstol) {
                if (f_x_middle > 0.0) {
                    x_high = x_middle;
                    f_x_high = f_x_middle;
                } else {
                    x_low = x_middle;
                    f_x_low = f_x_middle;
                }

                x_middle = (x_high + x_low) / 2;
                f_x_middle = stats::pt(x_middle, my_nu) - (1-alpha/2);
            }

            return x_middle;
        };


        Eigen::Matrix<T, 1, ColIndexType> t_stats = (Y1bar - Y2bar).array() * t1.rsqrt();
        Eigen::Matrix<double, 1, ColIndexType> crit_vals;
        Eigen::Matrix<bool, 1, ColIndexType> reject_null_hypothesis;
        int i{0};
        for (auto t_stat : t_stats) {
            crit_vals(i) = critval_func(nu(i));
            reject_null_hypothesis(i) = std::abs(t_stats(i)) > crit_vals(i);
            ++i;
        }



        TwoSampleTTestResult<T, ColIndexType> ret;
        ret.t_stat = t_stats;
        ret.dof = nu;
        ret.crit_val = crit_vals;
        ret.reject_null_hypothesis = reject_null_hypothesis;
        ret.alpha = alpha;


        return ret;
    }
};



#endif //PROMETHEUS_HYPOTHESIS_TESTING_HPP
