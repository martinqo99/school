function varargout=gellipse(MU, SIGMA, N, varargin)
%GELLIPSE   Conture plot of 2D Multivariate Gaussian distribution.
%   GELLIPSE(MU, SIGMA, N) plots ellipse given by neam vector MU and 
%   covariance matrix SIGMA. Ellipse is plotted using N (default is 100)
%   pionts. Additional parameters can specify various line types and
%   proerties. See description of PLOT for more details.
%
%   See also RANDG, PLOT


if size(MU, 2) ~= 1,  MU = MU'; end

if size(MU) ~= [2 1] | size(SIGMA) ~= [2 2]
  error('MU must be a two element vector and SIGMA must be 2 x 2 matrix');
end

if nargin < 3, S = '';  end
if nargin < 4, N = 100; end

[V D] = eig(4*SIGMA);
t=linspace(0, 2*pi, N);
X = [cos(t); sin(t)];
X = real(V) * diag(sign(diag(D))) * sqrt(abs(D)) * X + repmat(MU, 1, N);
h=plot(X(1,:), X(2,:), varargin{:});

if nargout>0, varargout{1}=h; end

