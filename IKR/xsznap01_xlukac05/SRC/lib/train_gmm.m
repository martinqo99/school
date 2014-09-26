function [Ws, MUs, COVs, TLL]=train_gmm(X, Ws, MUs, COVs)
%TRAIN_GMM Single iteration of EM algorithm for training Gaussian Mixture Model
%   [Ws_new,MUs_new, COVs_new, TLL]= TRAIN_GMM(X,Ws,NUs,COVs) performs single
%   iteration of EM algorithm (Maximum Likelihood estimation of GMM parameters)
%   using training data X and current model parameters Ws, MUs, COVs and returns
%   updated model parameters Ws_new, MUs_new, COVs_new and total log likelihood
%   TLL evaluated using the current (old) model parameters. The model
%   parameters are mixture component mean vectors given by columns of D-by-M
%   matrix MUs, covariance matrices given by D-by-D-by-M matrix COVs and vector
%   of weights Ws.
%
%   Alternatively, COVs can be D-by-M matrix, in wich case its columns
%   represent diagonal of diagonal covariance matrices.

M = size(MUs, 2); % number of mixture components
N = size(X, 2);   % number of training data points
D = size(X, 1);   % data dimensionality
gama=zeros(M, N); 

% For each Gaussian, compute log-likelihoods of all frames
for ii = 1:M 
  if ndims(COVs) == 2
    gama(ii,:) = log(Ws(ii)) + logpdf_gauss(X, MUs(:,ii), COVs(:,ii));
  else
    gama(ii,:) = log(Ws(ii)) + logpdf_gauss(X, MUs(:,ii), COVs(:,:,ii));
  end
end

% Normalize likelihoods to get posteriors probabilities of frames 
% occupying individual Gaussians. All the computation is carried
% out in log domain to avoid numerical problems
logevidence = logsumexp(gama);
gama =  exp(gama - repmat(logevidence, M, 1));

% Compute ML objective: total log likelihood evaluated using the current (old) model
TLL = sum(logevidence); 

% Update model parameters:
gamasum = sum(gama');

% weights,
Ws = gamasum / N; 

% mean vectors,
MUs = X * gama' ./ repmat(gamasum, D, 1);

if ndims(COVs) == ndims(MUs) && all(size(COVs) == size(MUs))
  % variance vectors in case of diagonal covariance model
  COVs = X.^2 * gama' ./ repmat(gamasum, D, 1) - MUs.^2;
else
  % covariance matrices in case of full covariance model
  for ii=1:M
    COVs(:,:,ii) = repmat(gama(ii,:), D, 1) .* X * X' / gamasum(ii) - MUs(:,ii) * MUs(:,ii)';
  end
end
