function H = plot2dfun(FUNC, LIMITS, RESOLUTION)
ylim=LIMITS(1):(LIMITS(2)-LIMITS(1))/RESOLUTION:LIMITS(2);
xlim=LIMITS(3):(LIMITS(4)-LIMITS(3))/RESOLUTION:LIMITS(4);
[a, b]=meshgrid(ylim, xlim);
H = imagesc(ylim, xlim,reshape(FUNC([a(:) b(:)]') ,length(xlim),length(ylim)));
set(get(H, 'Parent'), 'YDir', 'normal');
axis tight
colormap gray