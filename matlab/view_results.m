clc;
clear all;
close all;

plotlowestmisfitmodels=false;
plotmisfits   = true;
plotsynthetic = true;
depthplotlim  = [0 1500];
resplotlim    = [0.1 100000];


%syntheticdir = '..\examples\SouthernThomson\edifiles\';
%resultsdir   = '..\examples\SouthernThomson\output\';

syntheticdir = '..\examples\Synthetic\edifiles\';
resultsdir   = '..\examples\Synthetic\output\';

sdir = dir([resultsdir '*']);

for si=1:1:length(sdir)
    clear D;
    clear BFFM;
    
    if(sdir(si).isdir==0)continue; end;
    if(strcmp(sdir(si).name,'.'))continue; end;
    if(strcmp(sdir(si).name,'..'))continue; end;
    
    stationdir = [resultsdir sdir(si).name '\'];
    disp(stationdir);
    
    %%Read the station info file
    stationinfofile = [stationdir 'station_info.txt'];
    fp=fopen(stationinfofile,'r');
    station=fgetl(fp);
    datatype=fgetl(fp);
    nchains=str2num(fgetl(fp));
    burnin_samples=str2num(fgetl(fp));
    total_samples=str2num(fgetl(fp));
    fclose(fp);
    
    %Check if we inverted Impeadance or AppRes and AppPhase
    dt_impedance = 0;
    dt_appres    = 1;
    if(strcmp(datatype,'Impedance'))
        dt_impedance = 1;
        dt_appres    = 0;
    end
    
    %Plot file names
    jpgfile = sprintf('%s%s_lineardepth.jpg',stationdir,station);
    epsfile = sprintf('%s%s_lineardepth.eps',stationdir,station);
    pdffile = sprintf('%s%s_lineardepth.pdf',stationdir,station);
    %jpgfile = sprintf('%s%s_lineardepth_ex.jpg',stationdir,station);
    %epsfile = sprintf('%s%s_lineardepth_ex.eps',stationdir,station);
    %pdffile = sprintf('%s%s_lineardepth_ex.pdf',stationdir,station);
    
    %Title
    titlestring = sprintf('MT Station %s - 1D rj-McMC Inversion',station);
    
    %Data data
    a=load([stationdir 'data.txt']);
    n=load([stationdir 'noise.txt']);
    D.f      = a(:,1);
    D.data1  = a(:,2);
    D.data2  = a(:,3);
    D.noise1 = n(:,2);
    D.noise2 = n(:,3);
    D.period = 1./D.f;
    
    flim = [min(D.f)*0.5 max(D.f)*2.0];
    plim = [min(D.period)*0.5 max(D.period)*2];
    ndata = 2*length(D.f);
    
    if(plotsynthetic == true)
        truemodelfile = [syntheticdir station '.txt'];
        [m.thickness m.rho] = textread(truemodelfile,'%f %f','headerlines',1);
        SYN.model=m;
    end
    
    
    %Read best fit forward models for each chain / process
    for chain=0:1:nchains-1;
        bff = sprintf('%sbest_model_forward.%03d.txt',stationdir,chain);
        a = load(bff);
        BFFM.f(chain+1,:)         =  a(:,1);
        BFFM.data1(chain+1,:)   =  a(:,2);
        BFFM.data2(chain+1,:) =  a(:,3);
        BFFM.period(chain+1,:)    =  1./BFFM.f(chain+1,:);
        
        bff = sprintf('%sbest_model.%03d.txt',stationdir,chain);
        [m.layer m.from m.to m.rho] = textread(bff,'%f %f %f %f','headerlines',1);
        BFFM.model(chain+1)=m;
    end
    
    depth_bins=load([stationdir 'depth_bins.txt']);
    res_bins=load([stationdir 'prop_1_bins.txt']);
    reslim  = [res_bins(1) res_bins(end)];
    
    
    a=load([stationdir 'interface_depth_hist.txt']);
    partition_depth = a(:,1);
    partition_depth_hist = a(:,2);
    a=load([stationdir 'npartitions_hist.txt']);
    layer_num_hist = a(:,1);
    layer_num      = [1:length(a)];
    
    
    nprops=1;
    for i=1:1:nprops
        p(i).name   = sprintf('prop_%d',i);
        p(i).limits = reslim(i,:);
        
        file = sprintf('%s%s_mean.txt',stationdir,p(i).name); v=load(file);
        p(i).depth = v(:,1);
        p(i).mean  = v(:,2);
        
        file = sprintf('%s%s_median.txt',stationdir,p(i).name); v=load(file);
        p(i).median  = v(:,2);
        
        file = sprintf('%s%s_mode.txt',stationdir,p(i).name); v=load(file);
        p(i).mode  = v(:,2);
        
        file = sprintf('%s%s_credmin.txt',stationdir,p(i).name); v=load(file);
        p(i).credmin  = v(:,2);
        
        file = sprintf('%s%s_credmax.txt',stationdir,p(i).name); v=load(file);
        p(i).credmax  = v(:,2);
        
        file = sprintf('%s%s_hist.txt',stationdir,p(i).name); v=load(file);
        p(i).hist  = v;
    end
    depth  = 10.^p(1).depth;
    
    
    
    %%
    close(gcf);
    %bkgcolor = [1 1 1]; fgcolor  = [0 0 0]; cmap = gray(256); cmap=flip(cmap,1);
    bkgcolor = [0 0 0]; fgcolor  = [1 1 1]; cmap = gray(256);
    figure;
    paperwidth  = 25;
    paperheight = 15;
    set(gcf,'PaperUnits','Centimeters');
    set(gcf,'PaperSize',[paperwidth paperheight]);
    set(gcf,'PaperPositionMode','Manual');
    set(gcf,'PaperPosition',[0 0 paperwidth paperheight]);
    
    set(gcf,'defaultaxesfontsize',8);
    set(gcf,'color',bkgcolor);
    set(gcf,'position',[121 1 1560 951]);
    
    %Misfits
    if(plotmisfits==true)
        %Misfits
        for chain=0:1:nchains-1;
            mffile = sprintf('%smisfit.%03d.txt',stationdir,chain);
            a = load(mffile);
            %MF.sample(:,chain+1) = a(:,1);
            %MF.misfit(:,chain+1) = a(:,2);                        
            MF.misfit(:,chain+1) = a;
            MF.sample(:,chain+1) = 1:length(a);
        end
        
        xl=[1 100+max(MF.sample(:,1))];
        yl=[0.1 max(MF.misfit(:))];
        subplot(3,8,[17,18]);
        set(gca,'ActivePositionProperty','position');
        loglog(MF.sample,MF.misfit,'b');
        hold on;
        plot([burnin_samples burnin_samples],yl,':','color',fgcolor);
        plot(xl,[ndata ndata],':','color',fgcolor);
        %loglog(misfit,'b');
        xlim(xl); ylim(yl);
        box on; grid off;
        set(gca,'xtick',10.^[1 2 3 4 5 6]);
        set(gca,'layer','top');
        set(gca,'color',bkgcolor);
        set(gca,'xcolor',fgcolor);
        set(gca,'ycolor',fgcolor);
        xlabel('Sample#');
        ylabel('Data Misfit');
    end
    
    %Layers histogram
    subplot(3,8,[19,20]);
    set(gca,'ActivePositionProperty','position');
    bar(layer_num,layer_num_hist,'b');
    box on;
    set(gca,'layer','top');
    set(gca,'color',bkgcolor);
    set(gca,'xcolor',fgcolor);
    set(gca,'ycolor',fgcolor);
    xlim([0 20]);
    xlabel('#Layer');
    set(gca,'yticklabel',[]);
    
    
    %Resistivity-Depth
    subplot(3,8,[5,6,7, 13,14,15, 21,22,23]);
    set(gca,'ActivePositionProperty','outerposition');
    colormap(cmap);
    im = p(i).hist;
    a  = sort(im(:));
    clipmax = a(round(length(a)*0.99));%%99 clip
    for ri=1:1:size(im,1)
        im(ri,:) = im(ri,:) ./ max(im(ri,:));
    end
    ch=pcolor(10.^res_bins, 10.^depth_bins,im);
    set(ch,'edgecolor','none');
    hold on;
    k=1;
    
    %Resistivity grid lines
    for xg=0:1:4
        plot([10^xg 10^xg],depthplotlim,':','linewidth',1,'color',[0.5 0.5 0.5]);
    end
    
    %Depth grid lines
    for yg=0:50:max(depthplotlim)
        plot(resplotlim,[yg yg],':','linewidth',1,'color',[0.5 0.5 0.5]);
    end
    
    if(plotsynthetic == true)
        m=SYN.model;
        [r,d]=ct2cd(m.rho,m.thickness);
        h(k)=plot(r,d,'-m','linewidth',3);lab(k)={'True'};k=k+1;
    end
    
    if(plotlowestmisfitmodels)
        for ci=1:1:nchains
            grey=[0.5 0.5,0.5];
            m=BFFM.model(ci);
            [r,d]=ct2cd(m.rho,m.to - m.from);
            h(k)=plot(r,d,'-m','linewidth',1,'color',grey);lab(k)={'Lowest misfit on chain'};
        end
        k=k+1;
    end
        
    h(k)=plot(10.^p(i).median,depth,'-r','linewidth',1);lab(k)={'Median'};k=k+1;
    h(k)=plot(10.^p(i).credmin,depth,'--r','linewidth',1);lab(k)={'10th & 90th percentile'};k=k+1;
    plot(10.^p(i).credmax,depth,'--r','linewidth',1);
    h(k)=plot(10.^p(i).mean,depth,'-b','linewidth',1);lab(k)={'Mean'};k=k+1;
    h(k)=plot(10.^p(i).mode,depth,'-g','linewidth',1);lab(k)={'Mode'};k=k+1;
    
    box on;
    %grid on;
    set(gca,'ydir','reverse');
    set(gca,'layer','top');
    %set(gca,'xgrid','off');    
    set(gca,'xaxislocation','top');    
    set(gca,'xscale','log');
    set(gca,'yscale','linear');
    set(gca,'color',bkgcolor);
    set(gca,'xcolor',fgcolor);
    set(gca,'ycolor',fgcolor);
    xlim(resplotlim);
    ylim(depthplotlim);
    set(gca,'xtick',10.^[-1:1:5]);
    xlabel('Resistivity (Ohm.m)');
    ylabel('Depth (m)');
    lh=legend(h,lab);
    set(lh,'location','southwest','fontsize',6,'color',bkgcolor,'textcolor',fgcolor);
    
    %Partition depths
    subplot(3,8,[8,16,24]);
    set(gca,'ActivePositionProperty','position');
    plot(partition_depth_hist,10.^partition_depth,'b');
    box on; grid on;
    xlim( [-10 max(partition_depth_hist)]);
    ylim(depthplotlim);
    set(gca,'ydir','reverse');
    set(gca,'xaxislocation','top');
    set(gca,'yaxislocation','right');
    set(gca,'xticklabel',[]);
    set(gca,'yticklabel',[]);
    set(gca,'color',bkgcolor);
    set(gca,'xcolor',fgcolor);
    set(gca,'ycolor',fgcolor);
    xlabel('Changepoint');
    
    if(true)
        %Data 1 (Real impedance or App Res data)
        subplot(3,8,[1,2,3,4]);
        set(gca,'ActivePositionProperty','position');
        set(gca,'xscale','log'); set(gca,'yscale','log'); hold on;
        h1=errorbar(D.period, D.data1, D.noise1,'r.', 'LineWidth', 1);
        for i=1:1:nchains
            h2=loglog(BFFM.period(i,:), BFFM.data1(i,:), '-b', 'LineWidth', 2);
        end
        xlim(plim);
        box on;grid off;
        set(gca,'color',bkgcolor);
        set(gca,'xcolor',fgcolor);
        set(gca,'ycolor',fgcolor);
        title(titlestring,'color',fgcolor,'fontsize',12,'interpreter','none');
        xlabel('Period (s)');
        if(dt_impedance)
            ylabel('Real Impedance (mv/km/nT)');
        else
            ylabel('Apparent Resistivity (Ohm.m)');
        end
        
        lh=legend([h1 h2],'Data','Modelled Data (best-fit)');
        set(lh,'location','southwest','fontsize',6,'color',bkgcolor,'textcolor',fgcolor);
        
        %Data 2 (Imaginary impedance or App phase data)
        subplot(3,8,[9,10,11,12]);
        set(gca,'ActivePositionProperty','position');
        if(dt_impedance)
            set(gca,'xscale','log'); set(gca,'yscale','log'); hold on;
        else
            set(gca,'xscale','log'); set(gca,'yscale','linear'); hold on;
        end
        h1=errorbar(D.period, D.data2,D.noise2,'r.', 'LineWidth', 2);
        for i=1:1:nchains
            h2=loglog(BFFM.period(i,:), BFFM.data2(i,:),'-b', 'LineWidth', 1);
        end
        xlim(plim);
        box on;grid off;
        set(gca,'color',bkgcolor);
        set(gca,'xcolor',fgcolor);
        set(gca,'ycolor',fgcolor);
        xlabel('Period (s)');
        if(dt_impedance)
            ylabel('Imag Impedance (mv/km/nT)');
        else
            ylabel('Apparent Phase (deg)');
        end
        lh=legend([h1 h2],'Data','Modelled Data (best-fit)');
        set(lh,'location','southwest','fontsize',8,'color',bkgcolor,'textcolor',fgcolor);
    end
    
    
    
    set(gcf,'InvertHardcopy','off');
    set(gcf,'visible','on');
    pause(0.1);
    print('-painters','-dpdf','-r300',pdffile);
    %print('-painters','-djpeg','-r300',jpgfile);
    %print('-painters','-depsc','-r300',epsfile);
    
end
