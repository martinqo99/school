function [features file_names sizes] = png2fea(dir_name)

D=dir([dir_name '/*.png']);
file_names = {D.name};
for ii=1:length(file_names)
  disp(['Processing file: ' dir_name '/' file_names{ii}])
  s = imread([dir_name '/' file_names{ii}]);
  s = double(rgb2gray(s));
  features{ii} = s(:);
  sizes{ii}    = size(s);
end


% I = imread('IKR_projekt12_data/target_train/mtrg_01_f12_i0_0.png');
% imshow(I);
% I = double(rgb2gray(I));
% BW1 = edge(I,'sobel');
% figure, imshow(BW1);
% figure, imshow(BW2);
