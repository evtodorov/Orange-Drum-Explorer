#include <iostream>
#include <vector>
#include <cassert>
#include <Eigen/Core>
#include <Eigen/LU>
#include <adept.h>
#include <adept_source.h>
 
using Eigen::Matrix3d;
using Eigen::MatrixXd;
using Eigen::Matrix; 
using adept::adouble;


//adouble algorithm(const std::vector<adouble> x) {
//    adouble y = 4.0;
//    adouble s = 2.0*x[0] + 3.0*x[1]*x[1];
//    y *= sin(s);
//    return y;
//}
double algorithm_and_gradient( const std::vector<double> x_val,  std::vector<double>& dy_dx) {
    adept::Stack stack; // Where the derivative information is stored
    const std::vector<adouble> x = {x_val[0], x_val[1]}; // Initialize active input variables
    stack.new_recording();
    std::function<adouble(adouble t, const std::vector<adouble>&)> algorithm =
    [](adouble t, const std::vector<adouble> x){
       // adouble y = 4.0;
       // adouble s = 2.0*x[0] + 3.0*x[1]*x[1];
        return adouble(t + 4.0*sin(2.0*x[0] + 3.0*x[1]*x[1]));
    };
    double t = 0;
    adouble y = algorithm(t, x); // Call version overloaded for adouble args
    y.set_gradient(1.0); // Defines y as the objective function
    stack.compute_adjoint(); // Run the adjoint algorithm
    dy_dx[0] = x[0].get_gradient(); // Store the first gradient
    dy_dx[1] = x[1].get_gradient(); // Store the second gradient
    return y.value(); // Return the result of the simple computation
}

int main()
{
    Matrix3d m = Matrix3d::Random();
    std::cout << m << std::endl;
    Eigen::Vector3d v = Eigen::Vector3d::Random();
    v(1) = 10;
    MatrixXd inverse(3,3);
    std::cout << v << std::endl;
    bool invertible;
    m.computeInverseWithCheck(inverse,invertible);
    if(invertible) {
        std::cout << "It is invertible, and its inverse is:" << std::endl << inverse << std::endl;
    }
    else {
        std::cout << "It is not invertible." << std::endl;
    }
    Eigen::Vector3d sol = m.fullPivLu().solve(v);
    std::cout << sol << std::endl;
    std::cout << "Infinity: " << std::numeric_limits<double>::has_infinity << std::endl;
    std::cout << "Not-a-number: " <<std::numeric_limits<double>::has_quiet_NaN << std::endl;
    MatrixXd m2(3,3);
    m2 << 2,  1,  2,
          1,  0,  1,
          4,  1,  4;
    std::cout << m2 << std::endl;
    Eigen::FullPivLU<Matrix<double, 3, 3>> lu(m2);
    invertible = lu.isInvertible();
    inverse = lu.inverse();
    if(invertible) {
        std::cout << "It is invertible, and its inverse is:" << std::endl << inverse << std::endl;
    }
    else {
        std::cout << "It is not invertible." << std::endl;
    }
    Eigen::Vector3d sol2 = lu.solve(v);
    std::cout << sol2 << std::endl;
    if((m2*sol2).isApprox(v)){
        std::cout << "Not correct!" << std::endl;   
    }
    std::vector<double> x_val = {2,3};
    std::vector<double> dy_dx(2);
    auto y_val = algorithm_and_gradient( x_val, dy_dx);
    std::cout << "Value: " << y_val << " Gradient:" << dy_dx[0] << " " << dy_dx[1] << std::endl;
}