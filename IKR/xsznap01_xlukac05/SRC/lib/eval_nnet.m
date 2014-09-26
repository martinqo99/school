function Y = eval_nnet(X,W1,W2)
%EVAL_NNET propagate the data through the network
% Y = eval_nnet(X,W1,W2)
% X are the input features, datapoints are stored column-wise
% W1 weights of 1st layer (first column contain bias)
% W2 weights of 2nd layer (first column contain bias)
% Y column-wise network outputs


%%%%%%%%%%%%%% SYMBOLIC FUNCTIONS WE WILL USE LATER
sigmoid=inline('1./(1+exp(-x))','x');

%%%%%%%%%%%%%% FEEDFORWARD THE DATA THROUGH THE NETWORK
h=sigmoid(W1*([ones(1,size(X,2)); X]));
Y=sigmoid(W2*([ones(1,size(h,2)); h]));

