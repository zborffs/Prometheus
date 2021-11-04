#ifndef PROMETHEUS_OPTIMIZATION_HPP
#define PROMETHEUS_OPTIMIZATION_HPP

#include <Eigen/Dense>
#include "stopwatch.hpp"

namespace zaamath {

    template <int RowIndexType>
    class EngineParameters {
    protected:
        Eigen::Matrix<double, RowIndexType, 1> eigen_params_;
        // EvalutionParameters -> enable us to change the values that the engine uses
        // SearchParameters -> enable us to change the values that the engine uses

    public:
        explicit EngineParameters(Eigen::Matrix<double, RowIndexType, 1>& eigen_params) : eigen_params_(eigen_params) {

        }

        void update(Eigen::Matrix<double, RowIndexType, 1>& eigen_params) {
            eigen_params_ = eigen_params;

            // update actual evaluation parameters used by the engine given new EigenMatrices

            // update actual search parameters used by the engine given new EigenMatrices
        }

        Eigen::Matrix<double, RowIndexType, 1> eigen_params() {
            return eigen_params_;
        }
    };

    class Adam {
    private:
        bool show_trace_{true};
        int show_trace_every_{15};
        int f_x_tol_interval_{20};
        double f_x_tol_{1e-10};
        int x_tol_interval_{20};
        double x_tol_{1e-12};
        int max_iterations_{10000};
        double alpha_{0.01}; // learning rate / step size
        std::size_t batch_size_{1}; // not sure how to implement
        double beta1_{0.9};
        double beta2_{0.999};
        double epsilon_{1e-8};
        Eigen::Matrix<double, Eigen::Dynamic, 1> f_x_prev_; // don't initialize to any size yet
        Eigen::Matrix<double, Eigen::Dynamic, 1> times_;



        template <int RealRowIndexType>
        bool reached_termination(Eigen::Matrix<double, Eigen::Dynamic, 1>& f_x_prev, Eigen::Matrix<double, Eigen::Dynamic, RealRowIndexType>& x_prev, int iterations) {
            if (iterations >= max_iterations_) {
                std::cout << "Quitting b/c reached max iterations" << std::endl;
                return true;
            }

            if (iterations >= f_x_tol_interval_) {
                Eigen::Matrix<double, Eigen::Dynamic, 1> last_n_items = f_x_prev(Eigen::lastN(f_x_tol_interval_));
                if ((zaamath::range(last_n_items).cwiseAbs().array() < f_x_tol_).all()) {
                    std::cout << "Quitting b/c Error Converged" << std::endl;
                    std::cout << " - Range: " << zaamath::range(last_n_items).cwiseAbs().array() << " < " << f_x_tol_ << std::endl;
                    return true;
                }
            }

            if (iterations >= x_tol_interval_) {
                if ((zaamath::range(x_prev).cwiseAbs().array() < x_tol_).all()) {
                    std::cout << "Quitting b/c Parameters Converged" << std::endl;
                    std::cout << " - Range: " << zaamath::range(x_prev).cwiseAbs().array() << " < " << x_tol_ << std::endl;
                    return true;
                }
            }

            return false;
        }

    public:
        template <int RowIndexType>
        void optimize(EngineParameters<RowIndexType>& initial_params, std::function<double(EngineParameters<RowIndexType>&)>& obj_func, std::function<Eigen::Matrix<double, RowIndexType, 1>(EngineParameters<RowIndexType>&, int)>& jacobian) {

            // initialize to zero
            Eigen::Matrix<double, RowIndexType, 1> mt = Eigen::Matrix<double, RowIndexType, 1>::Zero();
            Eigen::Matrix<double, RowIndexType, 1> vt = Eigen::Matrix<double, RowIndexType, 1>::Zero();
            Eigen::Matrix<double, RowIndexType, 1> mthat = Eigen::Matrix<double, RowIndexType, 1>::Zero();
            Eigen::Matrix<double, RowIndexType, 1> vthat = Eigen::Matrix<double, RowIndexType, 1>::Zero();

            // Grab the last 'f_x_tol_interval' results


            // Grab the last 'x_tol_interval' inputs
            Eigen::Matrix<double, Eigen::Dynamic, RowIndexType> x_prev = Eigen::Matrix<double, Eigen::Dynamic, RowIndexType>::Zero(x_tol_interval_, RowIndexType);
            x_prev.row(0) = initial_params.eigen_params();

            // track number of iterations
            int iterations{0};

            // trace table headers
            std::size_t header_widths[4] = {
                std::string(" Iteration ").size()-1,
                std::string("    Time    ").size()-std::string(" [us] ").size(),
                std::string("   Error   ").size()-1,
                std::string(" Min. Error ").size()-1
            };

            // Grab the stopwatch
            StopWatch stop_watch;

            while (!reached_termination(f_x_prev_, x_prev, iterations)) {
                stop_watch.start();
                Eigen::Matrix<double, RowIndexType, 1> grad = jacobian(initial_params, iterations);
                int batch{1};
                do {
                    grad += jacobian(initial_params, iterations + batch);
                    ++batch;
                } while(batch < batch_size_);

                // update mt and vt
                mt = beta1_ * mt + (1 - beta1_) * grad;
                vt = beta2_ * vt + (1 - beta2_) * grad.array().square().matrix();

                // update mthat and vthat
                mthat = mt * 1 / (1 + std::pow(beta1_, iterations + 1));
                vthat = vt * 1 / (1 + std::pow(beta2_, iterations + 1));

                // update theta
                Eigen::Matrix<double, RowIndexType, 1> new_engine_params = initial_params.eigen_params() - (alpha_ * mthat.array() * (vthat.array().sqrt() + epsilon_).inverse()).matrix();
                initial_params.update(new_engine_params);

                stop_watch.stop();
                times_.conservativeResize(times_.rows()+1, Eigen::NoChange);
                times_(times_.rows()-1) = stop_watch.duration() * 1e-3;

                f_x_prev_.conservativeResize(f_x_prev_.rows()+1, Eigen::NoChange);
                f_x_prev_(f_x_prev_.rows()-1) = obj_func(initial_params);

                if (show_trace_) {
                    if (iterations == 0) {
                        std::cout << "| Iteration |    Time    |   Error   | Min. Error |" <<  std::endl;
                        std::cout << "+-----------+------------+-----------+------------+" << std::endl;
                        std::cout << "|";
                        std::cout << std::setw(header_widths[0]) << (iterations+1) << " ";
                        std::cout << "|";
                        double duration = times_(0);
                        std::cout << std::setw(header_widths[1]) << std::setprecision(4) << duration << " [us] ";
                        std::cout << "|";
                        double error = f_x_prev_(0);
                        std::cout << std::setw(header_widths[2]) << std::setprecision(4) << error << " ";
                        std::cout << "|";
                        std::cout << std::setw(header_widths[3]) << std::setprecision(4) << error << " ";
                        std::cout << "|" << std::endl;
                    } else if ((iterations + 1) % show_trace_every_ == 0) {
                        std::cout << "|";
                        std::cout << std::setw(header_widths[0]) << (iterations+1) << " ";
                        std::cout << "|";
                        double duration = times_.middleRows(iterations+1-show_trace_every_+1, show_trace_every_-1).sum();
                        std::cout << std::setw(header_widths[1]) << std::setprecision(4) << duration << " [us] ";
                        std::cout << "|";
                        double error = f_x_prev_(f_x_prev_.rows()-1);
                        std::cout << std::setw(header_widths[2]) << std::setprecision(4) << error << " ";
                        std::cout << "|";
                        double min_error = f_x_prev_.minCoeff();
                        std::cout << std::setw(header_widths[3]) << std::setprecision(4) << min_error << " ";
                        std::cout << "|" << std::endl;
                    }
                }
                ++iterations;
            }
        }

        Eigen::Matrix<double, Eigen::Dynamic, 1> f_x_prev() {
            return f_x_prev_;
        }

        Eigen::Matrix<double, Eigen::Dynamic, 1> times() {
            return times_;
        }
    };

};

#endif //PROMETHEUS_OPTIMIZATION_HPP
