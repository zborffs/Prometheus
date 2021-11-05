#ifndef PROMETHEUS_HYPOTHESIS_TESTING_HPP
#define PROMETHEUS_HYPOTHESIS_TESTING_HPP

#include <iostream>
#include "summary_statistics.hpp"
#include "optimization.hpp"

// zach's-attempt-at-math
namespace zaamath {

    /**
     * maintains data precipitating from two-sample t-test for testing equal means hypothesis test
     * @tparam T datatype of input data (float, int, custom, etc.)
     * @tparam ColIndexType the number of populations (or samples of populations)
     */
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

    /**
     * implementation of the two-sample t-test for testing equal means hypothesis test
     * @tparam T datatype of the two input datasets
     * @tparam RowIndexType1 number of samples in dataset 1
     * @tparam RowIndexType2 number of samples in dataset 2
     * @tparam ColIndexType number of populations per dataset
     * @param data1 the first dataset
     * @param data2 the second dataset
     * @param alpha the significance level of the t-test
     * @return two-sample t-test struct containing the t-statistics, degrees-of-freedom, critical values, booleans
     * representing whether to reject the null hypothesis, and the significance level
     */
    template <class T, int RowIndexType1, int RowIndexType2, int ColIndexType>
    TwoSampleTTestResult<T, ColIndexType> two_sample_t_test(Eigen::Matrix<T, RowIndexType1, ColIndexType>& data1, Eigen::Matrix<T, RowIndexType2, ColIndexType>& data2, double alpha) {
        TwoSampleTTestResult<T, ColIndexType> ret;
        ret.alpha = alpha;

        int N1 = data1.size();
        int N2 = data2.size();

        auto Y1bar = sample_mean(data1);
        auto Y2bar = sample_mean(data2);
        auto s1bar = sample_var(data1);
        auto s2bar = sample_var(data2);

        auto t1 = (s1bar / N1 + s2bar / N2).array(); // this value is used multiple times, so compute once here (not significant otherwise)
        ret.dof = (t1 * (((s1bar / N1).array().square() / (N1-1) + (s2bar / N2).array().square() / (N2-1)).inverse())); // degrees of freedom

        double x_abstol = 1e-10; // parameter convergence threshold
        double f_x_abstol = 1e-12; // evaluation function convergence threshold

        // bisection method for root finding the critical value for the given data
        std::function<double(double)> critval_func = [&](double my_nu) {
            double x_high{4}; // highest value I've seen in critical value tables is 3.9..., so 4 should be good upper-bound for standard purposes
            double x_low{0.5}; // lowest value I've seen in critical value tables is 0.6..., so 0.5 should be good lower-bound for standard purposes

            // standard bisection method implementation (converges reasonably fast)
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
                f_x_middle = stats::pt(x_middle, my_nu) - (1-alpha/2);  // cdf of t-distribution - target alpha value
            }

            return x_middle;
        };

        ret.t_stat = (Y1bar - Y2bar).array() * t1.rsqrt(); // t-statistic
        int i{0};
        for (auto t_stat : ret.t_stat) {
            // for each degree of freedom, compute the corresponding critical value and see if we should reject the null
            ret.crit_val(i) = critval_func(ret.dof(i));
            ret.reject_null_hypothesis(i) = std::abs(t_stat) > ret.crit_val(i);
            ++i;
        }

        return ret;
    }

    /**
     * enumerated type outlining the Tail type
     */
    enum Tail {
        LEFT = 0,
        RIGHT = 1,
        BOTH = 2
    };

    /**
     * maintains data precipitating from f-test for equality of two variances
     * @tparam T the datatype of the data used for the test (double, int, custom, etc.)
     * @tparam ColIndexType the number of populations tested
     */
    template <class T, int ColIndexType>
    struct FTestResult {
        Eigen::Matrix<T, 1, ColIndexType> f_stat; // each f-statistic (1 for every population tested)
        Eigen::Matrix<T, 1, ColIndexType> s1; // the variance of the 1st sample (1 for every population)
        Eigen::Matrix<T, 1, ColIndexType> s2; // the variance of the 2nd sample (1 for every population)
        Eigen::Matrix<bool, 1, ColIndexType> reject_null_hypothesis; // whether reject null hypothesis
        int dof1; // degrees-of-freedom of first sample
        int dof2; // degrees-of-freedom of second sample
        double crit_val; // critical value
        double alpha; // significance level
        Tail tail; // which tail was tested

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

    /**
     * implements the F-test for equality of two variances
     * @tparam T data type of input dataset
     * @tparam RowIndexType1  the number of samples in the first dataset
     * @tparam RowIndexType2  the number of samples in the second dataset
     * @tparam ColIndexType  the number of populations in each dataset (number of tests we want to do)
     * @param data1  the first dataset
     * @param data2  the second dataset
     * @param alpha  the significance level
     * @param tail  the tail of the test
     * @return f-test struct
     */
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

        ret.f_stat = ret.s1.array() * ret.s2.array().inverse(); // f-statistics

        double x_abstol = 1e-10; // parameter tolerance for root-finding algorithm convergence
        double f_x_abstol = 1e-12; // error tolerance for root-finding algorithm convergence

        // bisection method implementation for computing the critical value from data and significance level
        std::function<double()> critval_func = [&]() {

            // change the alpha used for testing based upon the tail
            double sub_alpha;
            switch (tail) {
                case RIGHT: sub_alpha = alpha; break;
                case LEFT: sub_alpha = 1 - alpha; break;
                case BOTH: sub_alpha = alpha / 2.0; break;
            }

            double x_high{5000}; // upper-bound for critical value (max i've seen from tables is upper 4000, so 5000 should be sufficient for standard usage
            double x_low{0.0}; // lower-bound for critical value (min i've seen from tables is upper 0.5, so 0 should be sufficient for standard usage

            // standard bisection method
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
                f_x_middle = stats::pf(x_middle, N1-1, N2-1) - sub_alpha; // cdf of f-distribution - target alpha value
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
