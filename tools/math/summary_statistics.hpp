#ifndef PROMETHEUS_SUMMARY_STATISTICS_HPP
#define PROMETHEUS_SUMMARY_STATISTICS_HPP

#include <Eigen/Dense>

// zach's-attempt-at-math
namespace zaamath {
    /**
     * computes the sample mean of a given dataset represented by a matrix whose columns represent different populations
     * @tparam T the datatype of the input matrix (int, float, double, custom, etc.)
     * @tparam RowIndexType the number of rows of the input datatype
     * @tparam ColIndexType the number of columns of the input datatype
     * @param data the data represented by an Eigen::Matrix (vector or matrix)
     * @return the sample mean of the data
     */
    template <class T, int RowIndexType, int ColIndexType>
    Eigen::Matrix<T, 1, ColIndexType> sample_mean(const Eigen::Matrix<T, RowIndexType, ColIndexType>& data) {
        return data.colwise().mean();
    }

    /**
     * computes the sample variance of a given dataset represented by a matrix whose columns represent different
     * populations
     * @tparam T the datatype of the input matrix (int, float, double, custom, etc.)
     * @tparam RowIndexType the number of rows of the input datatype
     * @tparam ColIndexType the number of columns of the input datatype
     * @param data the data represented by an Eigen::Matrix (vector or matrix)
     * @return the sample variance of the data
     */
    template <class T, int RowIndexType, int ColIndexType>
    Eigen::Matrix<T, 1, ColIndexType> sample_var(const Eigen::Matrix<T, RowIndexType, ColIndexType>& data) {
        auto means = data.colwise().mean();
        return (data.rowwise() - means).colwise().squaredNorm() / (data.rows() - 1);
    }

    /**
     * computes the sample standard deviation of a given dataset represented by a matrix whose columns represent
     * different populations
     * @tparam T the datatype of the input matrix (int, float, double, custom, etc.)
     * @tparam RowIndexType the number of rows of the input datatype
     * @tparam ColIndexType the number of columns of the input datatype
     * @param data the data represented by an Eigen::Matrix (vector or matrix)
     * @return the sample standard deviation of the data
     */
    template <class T, int RowIndexType, int ColIndexType>
    Eigen::Matrix<T, 1, ColIndexType> sample_std_dev(const Eigen::Matrix<T, RowIndexType, ColIndexType>& data) {
        auto var = sample_var(data);
        return var.array().sqrt();
    }

    template <class T, int RowIndexType, int ColIndexType>
    Eigen::Matrix<T, 1, ColIndexType> range(const Eigen::Matrix<T, RowIndexType, ColIndexType>& data) {
        return data.colwise().maxCoeff() - data.colwise().minCoeff();
    }

};

#endif //PROMETHEUS_SUMMARY_STATISTICS_HPP
