function [W W0] = train_generative_linear_classifier(X, class_id)

true_ids   = find(class_id == 1);
false_ids  = find(class_id == 0);

true_data  = X(:, true_ids);
false_data = X(:, false_ids);

% although we know what mean and covariance matrices we used, we just compute them once more
[true_mean   true_cov] = train_gauss(true_data); 
[false_mean false_cov] = train_gauss(false_data); 

% overall average covariance matrix
data_cov  = (true_cov * size(true_data, 2) + false_cov * size(false_data, 2)) / (size(true_data, 2) + size(false_data, 2));

W0  = -0.5 * true_mean' / data_cov * true_mean + 0.5 * false_mean' / data_cov * false_mean;
W   = data_cov \ (true_mean - false_mean);
