function score=k_nearest_neighbours(test_data, class1_examples, class2_examples, k)

euclidean_distances=dist([class1_examples class2_examples]', test_data);
[v i]=sort(euclidean_distances);
score = (sum(i(1:k,:) > size(class1_examples,2), 1) / k);
