% Project: IKR Evaluation
% Authors: Jakub Sznapka xsznap01@stud.fit.vutbr.cz
%          Michal Lukac xlukac05@stud.fit.vutbr.cz
% Date: 2.5.2012
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%Read all the training and test data into cell-arrays 
addpath('lib');

% % % % get mfcc features
train_m  = png2fea('IKR_projekt12_data/target_train');
train_f  = png2fea('IKR_projekt12_data/non-target_train');
[test_m files_m] = png2fea('eval');

% % For training, wi do not need to know which frame come from which training
% % segment. So, for each gender, concatenate all the training feature
% % matrices into single matrix
X1=cell2mat(train_m);
X2=cell2mat(train_f);

X1 = X1/256;
X2 = X2/256;

%generate corresponding target values:
%ones for class C1, zeros for class C2
T1=ones(1,size(X1,2));
T2=zeros(1,size(X2,2));

%%%%%%%%%%%%%% NORMALIZING INPUT
% E[X]=0 D[X]=1 (ie. zero mean and unit variance)
mu=mean([X1 X2]')' %get mean
sig=sqrt(var([X1 X2]'))' %get variance

%normalize
X1=(X1-repmat(mu,1,size(X1,2))) ./ repmat(sig,1,size(X1,2));
X2=(X2-repmat(mu,1,size(X2,2))) ./ repmat(sig,1,size(X2,2));

%make sure we did it correct
mu=mean([X1 X2]')' %this will produce zeros
sig=sqrt(var([X1 X2]'))' %this will produce ones

%%%%%%%%%%%%%% MERGE AND RANDOMIZE ORDER OF TRAINING DATA
X=[X1 X2];
T=[T1 T2];

%%%%%%%%%%%%%% NETWORK INITIALIZATION
DIM_IN=6400; %dimension of input layer
DIM_HIDDEN=500; %dimension of hidden layer, this can be adjusted
DIM_OUT=1;%dimension of output layer
%we use samples from Normal distribution: No(0,I*0.01)
W1=randn(DIM_HIDDEN,DIM_IN+1)*0.1; %+1 due to bias
W2=randn(DIM_OUT,DIM_HIDDEN+1)*0.1; %+1 due to bias
% Epsilon is a learning rate (also step-size),
epsilon=0.05;

% Training of NNET
% now we will perform 50 iterations with fixed learning rate
for iter=1:75
  %perform one epoch of the training
  show_training = 1;% subplot 223;
  [W1,W2,Ed] = train_nnet(X,T,W1,W2,epsilon,show_training);
  %define feedforward-function handler
  nnet_fwd = @(X) eval_nnet(X,W1,W2);
  %print the error Ed
  fprintf('\n\n=== EPOCH %d FINISHED ===\n', iter);
  fprintf('error in epoch %d is %g (err/frm:%g; prob_correct/frame:%g)\n',iter,Ed,Ed/size(X,2),exp(-Ed/size(X,2)));
  if Ed < 0.016
      break;
  end;
end

W1 = W1(:,1:6400);
W2 = W2(:,1:DIM_HIDDEN);

test_set = train_f;
for ii=1:length(test_set)
  b = test_set{ii}/256;
  n1 = W1*b;
  A1 = logsig(n1);
  n2 = W2*A1;
  A2 = logsig(n2);
  score_NonTarget(ii)=A2*1000;
end

test_set = train_m;
for ii=1:length(test_set)
  b = test_set{ii}/256;
  n1 = W1*b;
  A1 = logsig(n1);
  n2 = W2*A1;
  A2 = logsig(n2);
  score_Target(ii)=A2*1000;
end

% test values of net
test_set = test_m;
for ii=1:length(test_set)
  b = test_set{ii}/256;
  n1 = W1*b;
  A1 = logsig(n1);
  n2 = W2*A1;
  A2 = logsig(n2);
  score_Final(ii)=A2*1000;
end

% evaluate threshold
mean_NonTarget = mean(score_NonTarget);
mean_Target = mean(score_Target);
threshold = (mean_NonTarget + mean_Target) / 2

for ii=1:length(score_NonTarget)
  if mean_NonTarget > threshold
      result_score_NT(ii) = real(score_NonTarget(ii) < threshold);
  else
      result_score_NT(ii) = real(score_NonTarget(ii) > threshold);
  end
end

for ii=1:length(score_Target)
  if mean_NonTarget > threshold
      result_score_T(ii) = real(score_Target(ii) < threshold);
  else
      result_score_T(ii) = real(score_Target(ii) > threshold);
  end
  
end

subplot(2,1,1)
plot(score_Target, 'b'); hold on
plot(score_NonTarget, 'r');

for ii=1:length(score_Final)
  if mean_NonTarget > threshold
      result_score_Fin(ii) = real(score_Final(ii) > threshold);
  else
      result_score_Fin(ii) = real(score_Final(ii) < threshold);
  end
end

% get mfcc features
train_m  = wav16khz2mfcc('IKR_projekt12_data/target_train');
train_f  = wav16khz2mfcc('IKR_projekt12_data/non-target_train');
[test_m files_m] = wav16khz2mfcc('eval');

% For training, wi do not need to know which frame come from which training
% segment. So, for each gender, concatenate all the training feature
% matrices into single matrix
train_m=cell2mat(train_m);
train_f=cell2mat(train_f);

% Distribution in this single dimensional space are reasonable separated
% Lets define uniform a-priori probabilities of classes:
% A-priori probabilities of classes:
P_m = 0.5;
P_f = 1 - P_m;

%2 gaussian TRAINING!
disp('2xgaussian training...');
[mean_m cov_m] = train_gauss(train_m);
[mean_f cov_f] = train_gauss(train_f);

% 2 gaussian models for target
disp('2xgaussian target evaluation...');
test_set = test_m;
for ii=1:length(test_set)
  ll_m = logpdf_gauss(test_set{ii}, mean_m, cov_m);
  ll_f = logpdf_gauss(test_set{ii}, mean_f, cov_f);
  score_GAUSS_target(ii)=(sum(ll_m) + log(P_m)) - (sum(ll_f) + log(P_f));
end
score_GAUSS_target;
 
% 256 gaussian models for targets!
% Initialize mean vectors, covariance matrices and weights of mixture componments
% Initialize mean vectors to randomly selected data points from corresponding class
% Initialize all variance vectors (diagonals of the full covariance matrices) to
% the same variance vector computed using all the data from the given class
% Use uniform distribution as initial guess for the weights
M_m = 256;
MUs_m  = train_m(:,random('unid', size(train_m, 2), 1, M_m));
COVs_m = repmat(var(train_m', 1)', 1, M_m);
Ws_m   = ones(1,M_m) / M_m;

% Initialize parameters of non-targets!
M_f = 256;
MUs_f  = train_f(:,random('unid', size(train_f, 2), 1, M_f));  
COVs_f = repmat(var(train_f', 1)', 1, M_f);
Ws_f   = ones(1,M_f) / M_f;

% Run 20 iterations of EM algorithm to train the two GMMs from males and females
disp('running itterations of EM/GMM training...');
for jj=1:20
  [Ws_m, MUs_m, COVs_m, TTL_m] = train_gmm(train_m, Ws_m, MUs_m, COVs_m); 
  [Ws_f, MUs_f, COVs_f, TTL_f] = train_gmm(train_f, Ws_f, MUs_f, COVs_f); 
  %disp(['Iteration: ' num2str(jj) ' Total log-likelihood: ' num2str(TTL_m) ' for males; ' num2str(TTL_f) ' for frmales' ])
end

disp('evaluating gmm results for targets...');
test_set=test_m;
for ii=1:length(test_set)
  sprintf('gmm %d',ii);
  ll_m = logpdf_gmm(test_set{ii}, Ws_m, MUs_m, COVs_m);
  ll_f = logpdf_gmm(test_set{ii}, Ws_f, MUs_f, COVs_f);
  score_GMM_target(ii)=(sum(ll_m) + log(P_m)) - (sum(ll_f) + log(P_f));
end
score_GMM_target;

disp('evaluating Final Score...');
for ii=1:length(test_set)
  score_target(ii) = score_GAUSS_target(ii)*0.5 + score_GMM_target(ii)*0.5;
end

subplot(2,1,1)
plot(score_GAUSS_target, 'b'); hold on
plot(score_GMM_target, 'r'); hold on
plot(score_target, 'black'); 

% normalization
for ii=1:length(score_Final)
  score_Final(ii) = score_Final(ii) - threshold;
  score_Final(ii) = score_Final(ii) * 5;
end

fileID = fopen('result_xsznap01_xlukac05.txt','w+');
for ii=1:length(score_Final)
  score_weight(ii) = score_target(ii) - score_Final(ii) + 1000; % 1000 is threshold
  score(ii) = real(score_weight(ii) > 0);
  fprintf(fileID,'%s %f %d\n',files_m{ii}(1:10),score_weight(ii),score(ii));
end
fclose(fileID)

score

