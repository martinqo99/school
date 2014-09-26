function X = randg(N, MU, SIGMA)
%RANDG  Gaussian distributed random numbers.
%   X = RANDG(N, MU, SIGMA) returns matrix with N columns, where each column
%   is a vector chosen from a Multivariate Gaussian distribution with column
%   mean vector MU and covariance matrix SIGMA. 
%
%   RANDG uses RANDN to produces pseudo-random numbers. See RANDN for more
%   details on pseudo-random numbers generation and its initialization.
%
%    Example:
%
%       Plot data generated from 2D Multivariate Gaussian distribution
%       with mean [1 1] and covariance matrix [2, 0.2; 0.2 1] and plot
%       ellipse representing this distribution.
%          x = randg(500, [1 1], [2, 0.9; 0.9 1]);
%          plot(x(1,:), x(2,:), '.');
%          hold on;
%          gellipse([1 1], [2, 0.9; 0.9 1], 100, 'r');
%
%   See also GELLIPSE, RANDN.


if size(MU, 2) ~= 1
  MU = MU';
end

if size(MU, 2) ~= 1
  error('MU must be a vector');
end

if size(SIGMA) ~= [size(MU, 1) size(MU, 1)]
  error('SIGMA must be a square matrix matching to MU');
end

X = randn(size(MU, 1), N);
[V D] = eig(SIGMA);
X = (real(V) * diag(sign(diag(D))) * sqrt(abs(D)) * X + repmat(MU, 1, N));
