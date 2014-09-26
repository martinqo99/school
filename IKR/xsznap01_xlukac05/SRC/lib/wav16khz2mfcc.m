function [features file_names] = wav16khz2mfcc(dir_name)

D=dir([dir_name '/*.wav']);
file_names = {D.name};
for ii=1:length(file_names)
  disp(['Processing file: ' dir_name '/' file_names{ii}])
  s = wavread([dir_name '/' file_names{ii}]);
  features{ii} = mfcc(s, 400, 240, 512, 16000, 23, 13);
end
