#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Eigen/LU>
#include <adept.h>
#include <adept_source.h>
 
using Eigen::Matrix3d;
using Eigen::MatrixXd;
using Eigen::Matrix; 
using adept::adouble;


adouble algorithm(const adouble x[2]) {
    adouble y = 4.0;
    adouble s = 2.0*x[0] + 3.0*x[1]*x[1];
    y *= sin(s);
    return y;
}
double algorithm_and_gradient( const std::vector<double> x_val,  std::vector<double>& dy_dx) {
    adept::Stack stack; // Where the derivative information is stored
    adouble x[2] = {x_val[0], x_val[1]}; // Initialize active input variables
    stack.new_recording(); // Start recording
    adouble y = algorithm(x); // Call version overloaded for adouble args
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
    MatrixXd inverse(3,3);
    bool invertible;
    m.computeInverseWithCheck(inverse,invertible);
    if(invertible) {
        std::cout << "It is invertible, and its inverse is:" << std::endl << inverse << std::endl;
    }
    else {
        std::cout << "It is not invertible." << std::endl;
    }

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
    std::vector<double> x_val = {2,3};
    std::vector<double> dy_dx(2);
    auto y_val = algorithm_and_gradient( x_val, dy_dx);
    std::cout << "Value: " << y_val << " Gradient:" << dy_dx[0] << " " << dy_dx[1] << std::endl;
}