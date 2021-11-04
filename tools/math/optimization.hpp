#ifndef PROMETHEUS_OPTIMIZATION_HPP
#define PROMETHEUS_OPTIMIZATION_HPP

namespace zaamath {

    class ADAM {
    private:
        double step_size_{0.001};
        std::size_t batch_size{32};
        double beta1{0.9};
        double beta2{0.999};
        double eps{1e-8};
        std::size_t max_iterations{100000};
        double tolerance{1e-5};

    public:

    };

};

#endif //PROMETHEUS_OPTIMIZATION_HPP
