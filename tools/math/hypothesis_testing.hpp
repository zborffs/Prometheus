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

    enum Tail {
        LEFT = 0,
        RIGHT = 1,
        BOTH = 2
    };

    template <class T, int ColIndexType>
    struct FTestResult {
        Eigen::Matrix<T, 1, ColIndexType> f_stat;
        Eigen::Matrix<T, 1, ColIndexType> s1;
        Eigen::Matrix<T, 1, ColIndexType> s2;
        Eigen::Matrix<bool, 1, ColIndexType> reject_null_hypothesis;
        int dof1;
        int dof2;
        double crit_val;
        double alpha;
        Tail tail;

        friend std::ostream &operator<<(std::ostream &os, FTestResult<T, ColIndexType> &test) {
            os << std::boolalpha;

            std::string tail_string{};
            switch (test.tail) {
                case zaamath::Tail::LEFT: tail_string += "Left"; break;
                case zaamath::Tail::RIGHT: tail_string += "Right"; break;
                case zaamath::Tail::BOTH: tail_string += "Two-Tail"; break;
            }

            os << "F-Test for Equality of Two Variances" << std::endl;
            os << "- F-statistic: " << test.f_stat << std::endl;
            os << "- s1: " << test.s1 << std::endl;
            os << "- s2: " << test.s2 << std::endl;
            os << "- reject null hypothesis: " << test.reject_null_hypothesis << std::endl;
            os << "- dof: [" << test.dof1 << ", " << test.dof2 << "]" << std::endl;
            os << "- critical value: " << test.crit_val << std::endl;
            os << "- alpha: " << test.alpha << std::endl;
            os << "- tail: " << tail_string;

            os << std::noboolalpha;
            return os;
        }
    };

    template <class T, int RowIndexType1, int RowIndexType2, int ColIndexType>
    FTestResult<T, ColIndexType> f_test(Eigen::Matrix<T, RowIndexType1, ColIndexType>& data1, Eigen::Matrix<T, RowIndexType2, ColIndexType>& data2, double alpha, Tail tail) {
        FTestResult<T, ColIndexType> ret;


        int N1 = data1.size();
        int N2 = data2.size();
        ret.dof1 = N1 - 1;
        ret.dof2 = N2 - 1;
        ret.alpha = alpha;
        ret.tail = tail;


        ret.s1 = sample_var(data1);
        ret.s2 = sample_var(data2);


        ret.f_stat = ret.s1.array() * ret.s2.array().inverse();

        double x_abstol = 1e-10;
        double f_x_abstol = 1e-12;

        // why in function? make into a class, struct, or separate function in optimization.hpp
        std::function<double()> critval_func = [&]() {
            double sub_alpha;
            switch (tail) {
                case RIGHT: sub_alpha = alpha; break;
                case LEFT: sub_alpha = 1 - alpha; break;
                case BOTH: sub_alpha = alpha / 2.0; break;
            }

            double x_high{5000};
            double x_low{0.0};
            double x_middle = (x_high + x_low) / 2;
            double f_x_high = stats::pf(x_high, N1-1, N2-1) - sub_alpha;
            double f_x_low = stats::pf(x_low, N1-1, N2-1) - sub_alpha;
            double f_x_middle = stats::pf(x_middle, N1-1, N2-1) - sub_alpha;

            while (x_high - x_low > x_abstol && f_x_high - f_x_low > f_x_abstol) {
                if (f_x_middle > 0.0) {
                    x_high = x_middle;
                    f_x_high = f_x_middle;
                } else {
                    x_low = x_middle;
                    f_x_low = f_x_middle;
                }

                x_middle = (x_high + x_low) / 2;
                f_x_middle = stats::pf(x_middle, N1-1, N2-1) - sub_alpha;
            }

            return x_middle;
        };

        ret.crit_val = critval_func();
        int i{0};
        for (auto f_stat : ret.f_stat) {
            switch (tail) {
                case LEFT:
                    ret.reject_null_hypothesis(i) = f_stat > ret.crit_val;
                    break;
                case RIGHT:
                    ret.reject_null_hypothesis(i) = f_stat < ret.crit_val;
                    break;
                case BOTH:
                    ret.reject_null_hypothesis(i) = f_stat < ret.crit_val;
                    break;
            }

            ++i;
        }


        return ret;
    }
};



#endif //PROMETHEUS_HYPOTHESIS_TESTING_HPP
