function H = plot3dfun(FUNC, LIMITS, RESOLUTION, PROB)
ylim=LIMITS(1):(LIMITS(2)-LIMITS(1))/RESOLUTION:LIMITS(2);
xlim=LIMITS(3):(LIMITS(4)-LIMITS(3))/RESOLUTION:LIMITS(4);
[a, b]=meshgrid(ylim, xlim);
%show 3d plot
surf(ylim, xlim,reshape(FUNC([a(:) b(:)]') ,length(xlim),length(ylim)));
alpha(0.5); 
axis([min(xlim) max(xlim) min(ylim) max(ylim) 0 1]);
colormap gray;
