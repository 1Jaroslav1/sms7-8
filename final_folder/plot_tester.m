close all;
clear all;

folder = "DMC_responses/";

% workspace = "N100Nu50D650L1_mod";
% workspace = "N100Nu50D650L10_mod";
% workspace = "N100Nu50D650L20_mod";
% workspace = "N100Nu50D650L30_mod";
% workspace = "N100Nu100D650L10_mod";


% nazwa_work = folder + workspace;
nazwa_work = "odp_skokowa";

%% Wizualizacja - Gdy potrzebujemy tylko podglądu 

y=load(nazwa_work+".mat").y;
u=load(nazwa_work+".mat").u;


figure;
subplot(2,1,1)
stairs(y);
ylim([0 max(y)*1.05])
xlabel('$k$', 'Interpreter','latex');
ylabel('$y$', 'Interpreter','latex'),
legend({'$y$','$y^{zad}$'}, 'Interpreter','latex')


subplot(2,1,2)
stairs(u, 'r');
ylim([-52 52])
xlabel('$k$', 'Interpreter','latex');
ylabel('$u$', 'Interpreter','latex')
legend({'$u$'}, 'Interpreter','latex')

set(groot,'defaultAxesTickLabelInterpreter','latex'); 
set(gcf,'units','points','position',[100 100 800 600]);


%% Wizualizacja - Gdy chcemy zobaczyć gotowy przetworzony wykres (i ewentualnie go zapisać)

y=load(nazwa_work+".mat").y;
u=load(nazwa_work+".mat").u;
y_zad=load(nazwa_work+".mat").y_zad;

% y(201:255) = y(201:255)*1.0015;

E=sum((y_zad-y).^2);
disp(E)

k=0:length(y)-1;

figure;
subplot(2,1,1)
stairs(k, y);
hold on
stairs(k, y_zad,':');
xlim([0 length(y)])
ylim([40 max(y)*1.05])
xlabel('$k$', 'Interpreter','latex');
ylabel('$y$', 'Interpreter','latex')
title(strrep(sprintf("$E=%0.5e$", E),'.',','), 'Interpreter','latex')
legend({'$y$','$y^{zad}$'}, 'Interpreter','latex', 'Location','southeast')


subplot(2,1,2)
stairs(k, u, 'r');
xlim([0 length(u)])
ylim([-52 52])
xlabel('$k$', 'Interpreter','latex');
ylabel('$u$', 'Interpreter','latex')
legend({'$u$'}, 'Interpreter','latex', 'Location','southeast')


set(groot,'defaultAxesTickLabelInterpreter','latex'); 
set(gcf,'units','points','position',[100 100 450 300]);

print(nazwa_work,'-depsc','-r400') % zapis do eps (żeby wykresy ładne były)