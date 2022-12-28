close all;
clear all;

folder = "DMC_responses/";


%% N100Nu50D650L1
% workspace = "N100Nu50D650L1";
% chwila_skoku = 24;
% dlugosc_skoku = 315;
% przedskok = 10;

%% N100Nu50D650L10
% workspace = "N100Nu50D650L10";
% chwila_skoku = 18;
% dlugosc_skoku = 315;
% przedskok = 10;

%% N100Nu50D650L20
% workspace = "N100Nu50D650L20";
% chwila_skoku = 44;
% dlugosc_skoku = 315;
% przedskok = 10;

%% N100Nu50D650L30
% workspace = "N100Nu50D650L30";
% chwila_skoku = 39;
% dlugosc_skoku = 315;
% przedskok = 10;

%% N100Nu100D650L10
workspace = "N100Nu100D650L10";
chwila_skoku = 51;
dlugosc_skoku = 315;
przedskok = 10;

%% odp_skokowa
% workspace = "odp_skokowa";
% chwila_skoku = 17;
% dlugosc_skoku = 500;
% przedskok = 10;

%% Wykresy

nazwa_work = folder + workspace;
% nazwa_work = workspace; % do odpowiedzi skokowej

y=load(nazwa_work+".mat").y;
u=load(nazwa_work+".mat").u;

y=y(chwila_skoku-przedskok:chwila_skoku+dlugosc_skoku);
u=u(chwila_skoku-przedskok:chwila_skoku+dlugosc_skoku);


k=0:przedskok+dlugosc_skoku;

y_zad = ones(dlugosc_skoku+przedskok+1,1) * y(1);
y_zad(przedskok+1:end) = 50;

E=sum((y_zad-y).^2);
disp(E)

%%
figure;
subplot(2,1,1)
stairs(k, y);
hold on
stairs(k, y_zad,':');
xlim([0 przedskok+dlugosc_skoku])
ylim([40 max(y)*1.1])
xlabel('$k$', 'Interpreter','latex');
ylabel('$y$', 'Interpreter','latex')
title(strrep(sprintf("$E=%0.5e$", E),'.',','), 'Interpreter','latex')
legend({'$y$','$y^{zad}$'}, 'Interpreter','latex', 'Location','southeast')


subplot(2,1,2)
stairs(k, u, 'r');
xlim([0 przedskok+dlugosc_skoku])
ylim([min(u)-50 max(u)*1.1])
xlabel('$k$', 'Interpreter','latex');
ylabel('$u$', 'Interpreter','latex')
legend({'$u$'}, 'Interpreter','latex', 'Location','southeast')


set(groot,'defaultAxesTickLabelInterpreter','latex'); 
set(gcf,'units','points','position',[100 100 450 300]);
% print(nazwa_work,'-depsc','-r400')


%% zapisywanie workspace
% nazwa_work_mod = nazwa_work + "_mod.mat";
% save(nazwa_work_mod)