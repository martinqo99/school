function [blocks, cells] = HOG( data, cellCountX, cellCountY, blockWidth, blockHeight, binCount)
%
% Author: Michal Hradiš
% Contact: ihradis@fit.vutbr.cz
%
% Function implements a simple version of Histograms of Oriented Gradients
% (HOG) similar to the one presented in Navneet Dalal, Bill Triggs: 
% Histograms of Oriented Gradients for Human Detection, CVPR, 2005.
%
% This function works for all inputs; however, I did not test if it 
% returns correct values or if the returned features work as they should
% for recognition. If you find any errors, let me know.
%
% data         - image/matrix of gray-scale values
% cellCountX   - number of histograms of oriented gradients on horizontal line
% cellCountY   - number of histograms of oriented gradients no vertical column
% blockWidth   - how many cells are horizontally merged into single block
% blockWidth   - how many cells are vertically merged into single block
% binCount     - number of orientational bins in each histogram
%
% Histogram of oriented gradients is computed for each cell. Cells are
% merged into block by concatenating their histograms. The blocks overlap
% and each cell can belong to multiple blocks. Block are normalized by L2
% norm.
%
% This is a very simple implementation which does not take care of boundary
% effects (errors when one value falls in one bin or another one based on
% noise value or similar influence when it is close to bin boundary). Using
% linear interpolation or other methods may significantly improve
% performance.
%
% Each column of the resulting matrix is a vector of concatenated HOG
% corresponding to one block - the block matrix is 3D with the other two
% dimensions being y and x.
%
% The function returns the unnormalized cell histograms as well.
%
% If you want to use the output simply as a feauture vector, simpli do 
% blocks(:) or cells(:).


% compute horizontal differences
dx = data - [data( :, 2:end), data( :, end)];

% compute vertical differences
dy = data - [data( 2:end, :); data( end, :)];

% compute gradient direction and magnitude
[theta, rho] = cart2pol( dx, dy);

% discretize orientations
theta = round( (theta + pi) ./ (2*pi) .* (binCount - 1));
theta = theta + 1;

% this is for histograms for each cell - 3D array
cells = zeros( binCount, cellCountY, cellCountX);
counts = zeros( 1, cellCountY, cellCountX);

% compute histograms for each cell 
for x = 1:size( data, 2)
    for y = 1:size( data, 1)
        cx = floor( (x - 1) * cellCountX / size( data, 2) + 1);
        cy = floor( (y - 1) * cellCountY / size( data, 1) + 1);
        cells( theta( y, x), cy, cx) = cells( theta( y, x), cy, cx) + rho( y, x);
        counts( 1, cy, cx) = counts( 1, cy, cx) + 1;
    end
end

% normalize histograms by cell size 
cells = cells ./ repmat( counts, [binCount, 1, 1]);

% concatenate cells to blocks
blocks = []; %the size should  be zeros( blockWidth * blockHeight * binCount, floor( cellCountY / blockHeight) * blockHeight,  floor( cellCountX / blockWidth) * blockWidth);

for x = 1:blockWidth
    for y = 1:blockHeight
      blocks = [blocks; cells( :, y:(end - blockHeight + y), x:(end - blockWidth + x))];
    end
end


% normalize the concatenated block histograms by L2
regularizer = mean( mean( mean( cells))) ./ 10; % This is used to suppress noise in flat regions.
normalizer = (sum( blocks .* blocks) +  regularizer).^0.5; % sqrt( L2-norm^2 + regularizer)
blocks = blocks ./ repmat( normalizer, [ size( blocks, 1), 1, 1]); 

end


