% Cteni obrazovych dat v MATLABu
% zadani projektu C.
% Petr Fedra, cviceni BUMI 2011.

% Autor: vypracovania: Michal Lukac, xlukac05@stud.fit.vutbr.cz, 138643
% Datum: 6.11.2011
function michallukac_C()
    clear all
    close all
    
    % Zaciatok upravy obrazu
    I_info = imfinfo('FingerPrint.bmp','bmp');
    [I,map] = imread('FingerPrint.bmp','bmp');
    scrsz = get(0,'ScreenSize');
    magnification = 160;
    figure();
    subplot(2,2,1)
    imshow(I,map, 'InitialMagnification', magnification)
    title('Indexovy format obrazu - leva smycka')
    I_BW = im2bw(I, map, 0.8);
    subplot(2,2,2);
    imshow(I_BW, 'InitialMagnification', magnification)
    title('Cerno-bily format obrazu')
    I_neg_BW = ~I_BW;
    subplot(2,2,3)
    imshow(I_neg_BW, 'InitialMagnification', magnification)
    title('Negativ cerno-bileho obrazu')
    I_morf=bwmorph(I_neg_BW,'thin', inf);
    subplot(2,2,4);
    %pomocou funkcie bwmorph sme si ztensili obraz
    imshow(I_morf, 'InitialMagnification', magnification)
    title('Ztenceni obrazu');
    
    % Aplikujeme sub funkciu drobnosti pomocou nlfiltru na ztenseny obrazok
    L = nlfilter(I_morf, [3 , 3], @drobnosti);
    L_zacatek = (L == 1);
    L_vydlice = (L == 3);
    L_uvnitr = (L == 2);
    
    % Zistim pocet vidlic
    pocetVidlic = 0;
    for (x=1:336)
        for (y=1:256)
            if (L(x,y) == 3)
                pocetVidlic = pocetVidlic + 1;
            end;
        end;
    end;
    
    % Zistim pocet zaciatkov
    pocetZaciatkov = 0;
    for (x=1:336)
        for (y=1:256)
            if (L(x,y)==1) pocetZaciatkov = pocetZaciatkov + 1;
            end;
        end;
    end;
    
    % Vykreslime vidlice, pociatocne a vnutorne body
    figure();
    subplot(1,3,1);
    imshow(L_uvnitr, 'InitialMagnification', magnification);
    title('Body vo vnutri')
    subplot(1,3,2);
    imshow(L_zacatek, 'InitialMagnification', magnification);
    title('Pociatocne body')
    subplot(1,3,3);
    imshow(L_vydlice, 'InitialMagnification', magnification);
    title('Vidlice')
    
    %vykreslenie final
    figure();
    subplot(1,1,1);
    imshow(L,'InitialMagnification', magnification)
    hold on;
    % vyhladanie a vykreslenie vydlic do grafu modrym koleckom
    [x_p,y_p] = find(L_vydlice);
    plot(y_p,x_p,'ob');
    hold on;
    % vyhladanie a vykreslenie zaciatkov cervenym koleckom
    [x_p,y_p] = find(L_zacatek);
    plot(y_p,x_p,'or');
    tit= sprintf('Celkova analyza otisku. Pocet linii: %i.', round(pocetZaciatkov/2));
    title(tit);
    zac = sprintf('Vidlice %d', pocetVidlic);
    kon = sprintf('Zaciatky %d', pocetZaciatkov);
    legend(zac,kon);
    set(h_legend,'FontSize',14);
    hold off;
    
    % Subfunction drobnosti
    function y = drobnosti(x)
        % najdeme pocet bielych bodov v obrazku
        i = ceil(size(x)/2);
        if x(i,i) == 0
            y = 0;
        else
            y = sum(x(:)) - 1;
        end
    end
end

