function [features file_names] = raw8khz2mfcc(dir_name)

D=dir([dir_name '/*.raw']);
file_names = {D.name};
for ii=1:length(file_names)
  disp(['Processing file: ' dir_name '/' file_names{ii}])
  fid = fopen([dir_name '/' file_names{ii}]);
  s = fread(fid, inf, 'short');
  fclose(fid);
  features{ii} = mfcc(s, 200, 120, 256, 8000, 23, 13);
end
