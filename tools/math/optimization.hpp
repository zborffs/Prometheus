#ifndef PROMETHEUS_OPTIMIZATION_HPP
#define PROMETHEUS_OPTIMIZATION_HPP

#include <Eigen/Dense>
#include "stopwatch.hpp"

namespace zaamath {

    template <int RealRowIndexType, int IntegerRowIndexType>
    class EngineParameters {
    protected:
        Eigen::Matrix<double, RealRowIndexType, 1> real_params_;
        Eigen::Matrix<int, IntegerRowIndexType, 1> int_params_;
        // EvalutionParameters -> enable us to change the values that the engine uses
        // SearchParameters -> enable us to change the values that the engine uses

    public:
        virtual ~EngineParameters() = default;
        virtual void update(Eigen::Matrix<double, RealRowIndexType, 1>& real_params, Eigen::Matrix<int, IntegerRowIndexType, 1>& int_params) {
            real_params_ = real_params;
            int_params_ = int_params;

            // update actual evaluation parameters used by the engine given new EigenMatrices

            // update actual search parameters used by the engine given new EigenMatrices
        }

        Eigen::Matrix<double, RealRowIndexType, 1> real() {
            return real_params_;
        }

        Eigen::Matrix<int, IntegerRowIndexType, 1> integer() {
            return int_params_;
        }
    };

    class Adam {
    private:
        bool show_trace_{true};
        int show_trace_every_{5};
        int f_x_tol_interval_{20};
        double f_x_tol_{1e-6};
        int x_tol_interval_{20};
        double x_tol_{1e-12};
        int max_iterations_{1000};
        double alpha_{0.001}; // learning rate / step size
        std::size_t batch_size{1}; // not sure how to implement
        double beta1_{0.9};
        double beta2_{0.999};
        double epsilon_{1e-8};

        template <int RealRowIndexType, int IntegerRowIndexType>
        bool reached_termination(Eigen::Matrix<double, Eigen::Dynamic, 1>& f_x_prev, Eigen::Matrix<double, Eigen::Dynamic, RealRowIndexType>& x_prev_real, Eigen::Matrix<int, Eigen::Dynamic, IntegerRowIndexType>& x_prev_int, int iterations) {
            if (iterations >= max_iterations_) {
                return true;
            }

            if (iterations >= f_x_tol_interval_) {
                Eigen::Matrix<double, Eigen::Dynamic, 1> last_n_items = f_x_prev(Eigen::lastN(f_x_tol_interval_));
                if ((zaamath::range(last_n_items).cwiseAbs().array() < f_x_tol_).all()) {
                    return true;
                }
            }

            if (iterations >= x_tol_interval_) {
                if ((zaamath::range(x_prev_real).cwiseAbs().array() < x_tol_).all() && (zaamath::range(x_prev_int).cwiseAbs().array() < x_tol_).all()) {
                    return true;
                }
            }

            return false;
        }

    public:
        template <int RealRowIndexType, int IntegerRowIndexType>
        void optimize(EngineParameters<RealRowIndexType, IntegerRowIndexType>& initial_params) {

            // initialize to zero
            Eigen::Matrix<double, RealRowIndexType, 1> mt_real = Eigen::Matrix<double, RealRowIndexType, 1>::Zero();
            Eigen::Matrix<double, RealRowIndexType, 1> vt_real = Eigen::Matrix<double, RealRowIndexType, 1>::Zero();
            Eigen::Matrix<double, RealRowIndexType, 1> mthat_real = Eigen::Matrix<double, RealRowIndexType, 1>::Zero();
            Eigen::Matrix<double, RealRowIndexType, 1> vthat_real = Eigen::Matrix<double, RealRowIndexType, 1>::Zero();
            Eigen::Matrix<int, IntegerRowIndexType, 1> mt_int = Eigen::Matrix<int, IntegerRowIndexType, 1>::Zero();
            Eigen::Matrix<int, IntegerRowIndexType, 1> vt_int = Eigen::Matrix<int, IntegerRowIndexType, 1>::Zero();
            Eigen::Matrix<int, IntegerRowIndexType, 1> mthat_int = Eigen::Matrix<int, IntegerRowIndexType, 1>::Zero();
            Eigen::Matrix<int, IntegerRowIndexType, 1> vthat_int = Eigen::Matrix<int, IntegerRowIndexType, 1>::Zero();

            // Grab the last 'f_x_tol_interval' results
            Eigen::Matrix<double, Eigen::Dynamic, 1> f_x_prev; // don't initialize to any size yet

            // Grab the last 'x_tol_interval' inputs
            Eigen::Matrix<double, Eigen::Dynamic, RealRowIndexType> x_prev_real = Eigen::Matrix<double, Eigen::Dynamic, RealRowIndexType>::Zero(x_tol_interval_, RealRowIndexType);
            Eigen::Matrix<int, Eigen::Dynamic, IntegerRowIndexType> x_prev_int = Eigen::Matrix<int, Eigen::Dynamic, IntegerRowIndexType>::Zero(x_tol_interval_, IntegerRowIndexType);
            x_prev_real.row(0) = initial_params.real();
            x_prev_int.row(0) = initial_params.integer();

            // Grab the time
            Eigen::Matrix<double, Eigen::Dynamic, 1> times;

            // track number of iterations
            int iterations{0};

            // trace table headers
            std::size_t header_widths[2] = {
                std::string(" Iteration ").size()-1,
                std::string("    Time    ").size()-std::string(" [us] ").size()
            };

            // Grab the stopwatch
            StopWatch stop_watch;

            while (!reached_termination(f_x_prev, x_prev_real, x_prev_int, iterations)) {
                stop_watch.start();
                // User-defined Jacobian

                // update mt_real
                // update vt_real

                // update mt_int
                // update vt_int

                // update mthat_real
                // update vthat_real

                // update mthat_int
                // update vthat_int

                // update theta

                // CHANGE THIS LINE ONCE WE HAVE ACCESS TO THE UPDATED THETA
//                initial_params.update(initial_params.real(), initial_params.integer());

                stop_watch.stop();
                times.conservativeResize(times.rows()+1, Eigen::NoChange);
                times(times.rows()-1) = stop_watch.duration() * 1e-3;

                f_x_prev.conservativeResize(f_x_prev.rows()+1, Eigen::NoChange);
                f_x_prev(f_x_prev.rows()-1) = iterations; // fake!

                if (show_trace_) {
                    if (iterations == 0) {
                        std::cout << "| Iteration |    Time    |" <<  std::endl;
                        std::cout << "+-----------+------------+" << std::endl;
                        std::cout << "|";
                        std::cout << std::setw(header_widths[0]) << (iterations+1) << " ";
                        std::cout << "|";
                        double duration = times(0);
                        std::cout << std::setw(header_widths[1]) << std::setprecision(4) << duration << " [us] ";
                        std::cout << "|" << std::endl;
                    } else if ((iterations + 1) % show_trace_every_ == 0) {
                        std::cout << "|";
                        std::cout << std::setw(header_widths[0]) << (iterations+1) << " ";
                        std::cout << "|";
                        double duration = times.middleRows(iterations+1-show_trace_every_+1, show_trace_every_-1).sum();
                        std::cout << std::setw(header_widths[1]) << std::setprecision(4) << duration << " [us] ";
                        std::cout << "|" << std::endl;
                    }
                }
                ++iterations;
            }
        }
    };

};

#endif //PROMETHEUS_OPTIMIZATION_HPP
