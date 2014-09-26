function [W1,W2,Ed] = train_nnet(X,T,W1,W2,epsilon,show_training)
%TRAIN_NNET Single iteration of Stochastic Gradient Descent 
%training algorithm for Arificial Neural Networks 
%(ie. on-line backpropagation alg.)
% [W1,W2,Ed] = train_nnet(X,T,W1,W2,epsilon,show_training)
%
% X are the input features, datapoints are stored column-wise
% T target variables (0/1)
% W1 weights of 1st layer (first column contain bias)
% W2 weights of 2nd layer (first column contain bias)
% epsilon learning rate (to be tuned, for cross-validation use 0)
% show_training enables visualization (0/1)
% Ed accumulated error on the training set



%default arguments
if nargin < 6; show_training = 0; end
if nargin < 5; epsilon = 0; end

%%%%%%%%%%%%%% SYMBOLIC FUNCTIONS WE WILL USE LATER
%define several symbolic functions that we 
%will need in the futures:
%sigmoid function
sigmoid=inline('1./(1+exp(-x))','x');
%derivative of sigmoid function (the argument is output from neuron 'y')
diff_sigmoid=inline('y.*(1-y)','y');
%binary cross-entropy
ErrCrossEntropy=inline('-(t.*log(y) + (1-t).*log(1-y))','y','t');
%derivative dE/dy, note the minus sign
diff_ErrCrossEntropy=inline('-(t.*1/y + (1-t).*1/(1-y).*(-1))','y','t'); 


%%%%%%%%%%%%%% RE-SHUFFLE THE DATA ON EACH ITERATION
mixer=randperm(size(X,2)); %use shuffling mask of indices...
X=X(:,mixer);
T=T(:,mixer);


%%%%%%%%%%%%%% PERFORM ONE EPOCH OF THE TRAINING
% The implemnted algorithm is: Stochastic Gradient Descent
% (on-line training)

Ed=0; %reset the error accumulator

%The weight updates are performed after 
%getting gradient from each datapoint 
for d_idx=1:size(X,2)
  %select the feature-target pair
  str = sprintf('in training iteration  %d',d_idx);
  disp(str);
  x=X(:,d_idx); 
  t=T(:,d_idx);

  %VARIABLE NAMING:
  %x ... input layer
  %h ... hidden layer
  %y ... output layer

  %W1 ... weight matrix for first layer of neurons
  %W2 ... weight matrix for second layer of neurons

  %%% 1) PROPRAGATION
  h=sigmoid(W1*[1; x]);
  y=sigmoid(W2*[1; h]);
  
  %%% 2) GLOBAL ERROR EVALUTAION
  %use cross entropy
  E=ErrCrossEntropy(y,t);
  d_y=diff_ErrCrossEntropy(y,t); %error on network output level
  
  %%% 3) BACKPROPAGATION
  d_ya=d_y.*diff_sigmoid(y); %error on activation of second layer neurons
  d_h=(W2(:,2:end)')*d_ya; %error on hidden layer level, don't backpropagate by bias which is in the first column of the weight matrix
  d_ha=d_h.*diff_sigmoid(h); %error on activation of first layer neurons
  
  %%% 4) WEIGHT UPDATE
  W1=W1-epsilon*(d_ha*[1 x']);
  W2=W2-epsilon*(d_ya*[1 h']);

  %accumulate the errors per epoch
  Ed = Ed + E;

  %visualize the network function
  n_redraw=50; %redraw every n frames
  if(show_training && mod(d_idx,n_redraw)==0)
    %define feedforward function handler
    clear nnet_fwd;
    nnet_fwd = @(X) eval_nnet(X,W1,W2);
    %plot the function
    %plot3dfun(nnet_fwd, [-5 5 -5 5], 20);
    %title(['The output function of the network after seeing datapoint #' int2str(d_idx)]);
    %xlabel('x1'); ylabel('x2'); zlabel(' y = f(x1,x2) ');
    %pause(0.1);
  end
end  


