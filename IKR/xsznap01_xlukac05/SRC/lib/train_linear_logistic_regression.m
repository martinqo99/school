function [W W0] = train_llr_NR(X, class_ids, Wold, W0old)
%TRAIN_LLR_NR Reestimates Linear classifier parameters using Linear Logistic
%   Regression, making one Newton-Raphson method step.

% artificially add one more dimension to the data with value "1". This will
% infer the bias parameter
X     = [X; ones(1,size(X,2))];
Wold  = [Wold; W0old];

% compute the posteriors from the log likelihood ratios
llrs = Wold' * X;
posteriors = logistic_sigmoid(llrs);

R = posteriors .* (1-posteriors);
Wnew = Wold - inv(X * diag(R) * X') * X * (posteriors - class_ids)';

W = Wnew(1:(end-1),1);
W0 = Wnew(end,1);
