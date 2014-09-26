function Y = logistic_sigmoid(X)
% LOGISTIC_SIGMOID Logistic sigmoid function
%    Function converts log odds (log-likelihood ratio plus log prior ratio)
%    into posterior probabilities

Y = 1 ./ (1 + exp(-X));
