function [MU, COV]=train_gauss(X)
%TRAIN_GAUSS Estimates gaussian distribution from data.
%   [MU, COV]= TRAIN_GAUSS(X) return Maximum Likelihood estimates of mean
%   vector and covariance matrix estimated using training data X
MU = mean(X', 1)';
COV = cov(X', 1)';
