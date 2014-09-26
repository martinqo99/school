function Y = logpdf_gauss(X, MU, COV)
%LOGPDF_GAUSS Log density function for multivariate Gaussian distribution.
%   Y = LOG_PDF_GAUSS(X,MU,SIGMA) returns a row vector of values of the log probability
%   density function for multivariate Gaussian distribution with with mean
%   column vector MU, and covariance matrix COV, evaluated at the vectors given
%   by collumns of X.
%
%   Alternatively, COV can be column vector, in wich case it represents a
%   diagonal covariance matrix, vector COV in the diagonal.

X=X-repmat(MU, 1, size(X, 2));

if(size(COV) == size(MU))
  % Simplified evaluation for diagonal covariance matrix (no need for matrix inversion)
  Y=log(1/(((2*pi)^(size(X, 1)/2))*sqrt(prod(COV)))) -0.5*sum(diag(1./COV) * X.^2, 1);
else
  Y=log(1/(((2*pi)^(size(X, 1)/2))*sqrt(det(COV))))  -0.5*sum(inv(COV) * X .* X, 1);
end
